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
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "common.h"
#include "ptrace/generic.h"

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

static int trace_syscall(pid_t pid)
{
	int status;
	while (true) {
		/* Keep retracing the program until we hit a syscall. */
		if (ptrace(PTRACE_SYSCALL, pid, 0, 0) < 0)
			die("ptrace(syscall) failed: %m");
		if (waitpid(pid, &status, 0) < 0)
			die("waitpid failed: %m");

		if (WIFEXITED(status))
			return -status;

		/* We're in a syscall. */
		/* TODO: Deal with children. */
		if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
			return 0;
	}
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
	if (ptrace(PTRACE_SETOPTIONS, pid, 0, TRACE_FLAGS) < 0)
		die("ptrace(setoptions) failed: %m");

	/*
	 * Main tracing loop. We wait until the process is stopped, and then we
	 * evaluate what to do. Most of the complications result because ptrace(2)
	 * doesn't tell us what syscall we are returning from.
	 */
	while (true) {
		/* --> syscall() */
		if (trace_syscall(pid) < 0)
			break;

		/* XXX: This is currently not used. */
		/* Get event information for the trace event. */
		/*
		unsigned long event_data;
		if (ptrace(PTRACE_GETEVENTMSG, pid, NULL, &event_data) < 0)
			die("ptrace(geteventmsg) failed: %m");
		*/

		long number = ptrace_syscall_number(pid);
		if (number < 0)
			die("ptrace_syscall_number failed: %m");

		printf("syscall(%ld)\n", number);

		/* TODO: Implement all of this. */
		/*
		int event_type = (status >> 8) & ~SIGTRAP;
		switch (event_type & ~0xFF) {
			case 0:
				break;
			case PTRACE_EVENT_FORK << 8:
			case PTRACE_EVENT_CLONE << 8:
			case PTRACE_EVENT_VFORK << 8:
				die("{fork,clone,vfork} not implemented");
				break;
			default:
				die("unknown event: %d", event_type);
				break;
		}
		*/

		/* <-- syscall() */
		if (trace_syscall(pid) < 0)
			die("trace_syscall failed: process died inside syscall\n");
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
