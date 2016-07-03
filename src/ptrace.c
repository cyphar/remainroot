/*
 * remainroot: a shim to trick code to run in a rootless container
 * Copyright (C) 2016 Aleksa Sarai <asarai@suse.de>
 *
 * remainroot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * remainroot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with remainroot.  If not, see <http://www.gnu.org/licenses/>.
 */

/* ptrace.c is the tracing shim. */

#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#include "config.h"
#include "common.h"
#include "ptrace/generic.h"
#include "ptrace/generic-shims.h"
#include "ohmic/ohmic.h"

static struct ohm_t *pid_hm;

/*
 * TODO: Replace this with process context. Due to bad design on my part, ohmic
 *       doesn't allow you to have NULL values so this is just a placeholder
 *       until I implement all of the state variable crap.
 */
static char nothing = '*';

static void ptrace_init(void) __attribute__((constructor));
static void ptrace_init(void)
{
	/* XXX: Do we need to resize this at any point? */
	pid_hm = ohm_init(4096, ohm_hash);
}

static bool still_tracing(void)
{
	return ohm_iter_init(pid_hm).key != NULL;
}

static void tracee(int argc, char **argv)
{
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0)
		die("child: ptrace(traceme) failed: %m");

	/* Make sure tracer starts tracing us. */
	if (raise(SIGSTOP))
		die("child: raise(SIGSTOP) failed: %m");

	/* Start the process. */
	execvp(argv[0], argv);

	/* Should never be reached. */
	die("tracee start failed: %m");
}

static int trace_syscall(pid_t *pid, int *ret)
{
	int status;

	/* We restart tracing the process that we last hit. */
	if (*pid) {
		if (ptrace(PTRACE_SYSCALL, *pid, NULL, NULL) < 0) {
			if (errno == ESRCH)
				ohm_remove(pid_hm, pid, sizeof(pid_t));
			die("ptrace(syscall) failed: %m");
		}
	}

	/* Loop until we get a syscall trace. */
	while (still_tracing()) {
		/*
		 * While this isn't _explicitly_ mentioned in the documentation, ptrace
		 * is implemented such that the tracer is a pseudo-parent of all
		 * tracees. That means that a process cannot ever become a non-"child"
		 * process and using waitpid(-1, ...) is totally fine. At least, that's
		 * what I'm going to tell myself at night.
		 */
		*pid = waitpid(-1, &status, 0);
		if (*pid < 0)
			die("waitpid failed: %m");

		if (ret)
			*ret = status;

		/* Process is dead, remove it from the pool. */
		if (WIFEXITED(status)) {
			ohm_remove(pid_hm, pid, sizeof(pid_t));
			continue;
		}

		/* We're in a syscall. */
		if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
			return 0;

		/* We just hit a fork. */
		if (((status >> 8) & SIGTRAP) == SIGTRAP && (status >> 8) != SIGTRAP)
			return 0;

		/* Restart tracing, it wasn't the state we wanted. */
		if (ptrace(PTRACE_SYSCALL, *pid, NULL, NULL) < 0) {
			if (errno == ESRCH) {
				ohm_remove(pid_hm, pid, sizeof(pid_t));
				continue;
			}
			die("ptrace(syscall) failed: %m");
		}
	}

	/* She's done. */
	return 1;
}

/* TODO: Deal with the case where TRACESYSGOOD isn't defined. */
#define TRACE_FLAGS (PTRACE_O_EXITKILL | PTRACE_O_TRACECLONE | \
	                 PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | \
	                 PTRACE_O_TRACESYSGOOD)

static void tracer(pid_t pid)
{
	int status = 0;

	/* Wait for child to be ready for us to attach. */
	if (waitpid(pid, &status, 0) < 0)
		die("waitpid failed: %m");
	if (!WIFSTOPPED(status) || WSTOPSIG(status) != SIGSTOP) {
		kill(pid, SIGKILL);
		die("tracer: unexpected wait status: %x", status);
	}
	if (ptrace(PTRACE_SETOPTIONS, pid, 0, TRACE_FLAGS) < 0)
		die("ptrace(setoptions) failed: %m");

	/* Add the initial process to the pool. */
	if (!ohm_insert(pid_hm, &pid, sizeof(pid_t), &nothing, sizeof(nothing)))
		die("ohm_insert(init-%d) failed", pid);

	/*
	 * Main tracing loop. We wait until the process is stopped, and then we
	 * evaluate what to do. Most of the complications result because ptrace(2)
	 * doesn't tell us what syscall we are returning from.
	 */
	while (still_tracing()) {
		int status;

		/* --> syscall() */
		if (trace_syscall(&pid, &status))
			break;

		long number = ptrace_syscall(pid);
		if (number < 0)
			die("ptrace_syscall(%d) failed: %m", pid);

		/* TODO: Remove need_replace. */
		bool need_replace = true;
		uintptr_t ret = 0;

		/*
		 * Calculates and modifies all of the relevant state.
		 * XXX: This should probably after <--syscall(), so that we can
		 * completely ignore what the kernel decided.
		 *
		 * Also, this code is ugly.
		 */
		switch (number) {
#define SYSCALL(func) \
			case SYS_ ## func: \
				if (ptrace_rr_ ## func(pid, &ret) < 0) \
					die("ptrace_syscall_%s failed: %m\n", "" # func); \
				break;
#define SYSCALL0(type, func, ...) SYSCALL(func)
#define SYSCALL1(type, func, ...) SYSCALL(func)
#define SYSCALL2(type, func, ...) SYSCALL(func)
#define SYSCALL3(type, func, ...) SYSCALL(func)
#define SYSCALL4(type, func, ...) SYSCALL(func)
#define SYSCALL5(type, func, ...) SYSCALL(func)
#define SYSCALL6(type, func, ...) SYSCALL(func)
#define LIBCALL0(...)
#define LIBCALL1 LIBCALL0
#include "core/cred.h"
#undef SYSCALL0
#undef SYSCALL1
#undef SYSCALL2
#undef SYSCALL3
#undef SYSCALL4
#undef SYSCALL5
#undef SYSCALL6
#undef LIBCALL0
#undef LIBCALL1
				break;
			default:
				need_replace = false;
				break;
		}

		/* <-- syscall() */
		/*
		 * FIXME: I'm 95% sure there's a race condition here if you
		 *        accidentally hit a syscall entry here.
		 */
		if (trace_syscall(&pid, &status)) {
			/* The user called the exit syscall. */
			if (number == SYS_exit || number == SYS_exit_group) {
				ohm_remove(pid_hm, &pid, sizeof(pid_t));
				pid = 0;
				continue;
			}
			die("trace_syscall failed: process died inside syscall\n");
		}

		/* See if we just hit a clone. */
		switch ((status >> 8) & ~SIGTRAP) {
			case PTRACE_EVENT_CLONE << 8:
			case PTRACE_EVENT_VFORK << 8:
			case PTRACE_EVENT_FORK << 8:
				{
					pid_t trace_child;

					if (ptrace(PTRACE_GETEVENTMSG, pid, NULL, &trace_child) < 0)
						die("ptrace(getevntmsg): %m");

					/* #yolo */
					if (!ohm_insert(pid_hm, &trace_child, sizeof(pid_t), &nothing, sizeof(nothing)))
						die("ohm_insert(child-%d) failed", trace_child);
				}
		}

		/*
		 * Replace syscall return value. We have to do this after
		 * ret-from-syscall. XXX: There should be some logic to deal
		 * with errors reported from the kernel.
		 */
		if (need_replace && ptrace_return(pid, ret) < 0)
			die("ptrace_return(%d): %m", ret);
	}

	exit(0);
}

void shim_ptrace(int argc, char **argv)
{
	pid_t pid = fork();
	if (pid < 0)
		die("couldn't fork: %m");
	else if (pid == 0)
		tracee(argc, argv);
	else
		tracer(pid);

	die("should never be reached");
}
