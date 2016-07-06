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

/* generic-shims.c implements shims using the generic.h API. */

#include <limits.h>
#include "core/proc.h"
#include "core/cred.h"
#include "generic.h"
#include "generic-shims.h"

/* SYSCALL1(int, setuid, uid_t, uid) */
int ptrace_rr_setuid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	uid_t uid = ptrace_argument(pid, 0);
	*ret = __rr_do_setuid(&current->cred, uid);
	return 0;
}

/* SYSCALL0(uid_t, getuid) */
int ptrace_rr_getuid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	*ret = __rr_do_getuid(&current->cred);
	return 0;
}

/* SYSCALL1(int, setfsuid, uid_t, fsuid) */
int ptrace_rr_setfsuid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	uid_t fsuid = ptrace_argument(pid, 0);
	*ret = __rr_do_setfsuid(&current->cred, fsuid);
	return 0;
}

/* SYSCALL2(int, setreuid, uid_t, ruid, uid_t, euid) */
int ptrace_rr_setreuid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	uid_t ruid = ptrace_argument(pid, 0);
	uid_t euid = ptrace_argument(pid, 1);
	*ret = __rr_do_setreuid(&current->cred, ruid, euid);
	return 0;
}

/* SYSCALL3(int, setresuid, uid_t, ruid, uid_t, euid, uid_t, suid) */
int ptrace_rr_setresuid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	uid_t ruid = ptrace_argument(pid, 0);
	uid_t euid = ptrace_argument(pid, 1);
	uid_t suid = ptrace_argument(pid, 2);
	*ret = __rr_do_setresuid(&current->cred, ruid, euid, suid);
	return 0;
}

/* SYSCALL3(int, getresuid, uid_t *, ruid, uid_t *, euid, uid_t *, suid) */
int ptrace_rr_getresuid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	uid_t ruid, euid, suid;
	*ret = __rr_do_getresuid(&current->cred, &ruid, &euid, &suid);

	uintptr_t p_ruid = ptrace_argument(pid, 0);
	uintptr_t p_euid = ptrace_argument(pid, 1);
	uintptr_t p_suid = ptrace_argument(pid, 2);

	ptrace_assign_data(pid, p_ruid, ruid);
	ptrace_assign_data(pid, p_euid, euid);
	ptrace_assign_data(pid, p_suid, suid);
	return 0;
}

/* SYSCALL0(uid_t, geteuid) */
int ptrace_rr_geteuid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	*ret = __rr_do_geteuid(&current->cred);
	return 0;
}

/* SYSCALL1(int, setgid, gid_t, gid) */
int ptrace_rr_setgid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	gid_t gid = ptrace_argument(pid, 0);
	*ret = __rr_do_setgid(&current->cred, gid);
	return 0;
}

/* SYSCALL0(gid_t, getgid) */
int ptrace_rr_getgid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	*ret = __rr_do_getgid(&current->cred);
	return 0;
}

/* SYSCALL1(int, setfsgid, gid_t, fsgid) */
int ptrace_rr_setfsgid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	gid_t fsgid = ptrace_argument(pid, 0);
	*ret = __rr_do_setfsgid(&current->cred, fsgid);
	return 0;
}

/* SYSCALL2(int, setregid, gid_t, rgid, gid_t, egid) */
int ptrace_rr_setregid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	gid_t rgid = ptrace_argument(pid, 0);
	gid_t egid = ptrace_argument(pid, 1);
	*ret = __rr_do_setregid(&current->cred, rgid, egid);
	return 0;
}

/* SYSCALL3(int, setresgid, gid_t, rgid, gid_t, egid, gid_t, sgid) */
int ptrace_rr_setresgid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	gid_t rgid = ptrace_argument(pid, 0);
	gid_t egid = ptrace_argument(pid, 1);
	gid_t sgid = ptrace_argument(pid, 2);
	*ret = __rr_do_setresgid(&current->cred, rgid, egid, sgid);
	return 0;
}

/* SYSCALL3(int, getresgid, gid_t *, rgid, gid_t *, egid, gid_t *, sgid) */
int ptrace_rr_getresgid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	gid_t rgid, egid, sgid;
	*ret = __rr_do_getresgid(&current->cred, &rgid, &egid, &sgid);

	uintptr_t p_rgid = ptrace_argument(pid, 0);
	uintptr_t p_egid = ptrace_argument(pid, 1);
	uintptr_t p_sgid = ptrace_argument(pid, 2);

	ptrace_assign_data(pid, p_rgid, rgid);
	ptrace_assign_data(pid, p_egid, egid);
	ptrace_assign_data(pid, p_sgid, sgid);
	return 0;
}

/* SYSCALL0(gid_t, getegid) */
int ptrace_rr_getegid(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	*ret = __rr_do_getegid(&current->cred);
	return 0;
}

/* SYSCALL2(int, setgroups, int, size, const gid_t *, list) */
int ptrace_rr_setgroups(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	int size = ptrace_argument(pid, 0);
	uintptr_t head = ptrace_argument(pid, 1);
	gid_t list[NGROUPS_MAX] = {0};

	for (int i = 0; i < size; i++)
		list[i] = ptrace_deref_data(pid, (uintptr_t) ((gid_t *) head) + i);

	*ret = __rr_do_setgroups(&current->cred, size, list);
	return 0;
}

/* SYSCALL2(int, getgroups, int, size, gid_t *, list) */
int ptrace_rr_getgroups(struct proc_t *current, pid_t pid, uintptr_t *ret)
{
	int size = ptrace_argument(pid, 0);
	uintptr_t head = ptrace_argument(pid, 1);
	gid_t list[NGROUPS_MAX] = {0};

	*ret = __rr_do_getgroups(&current->cred, size, list);
	for (int i = 0; i < size; i++)
		ptrace_assign_data(pid, (uintptr_t) ((gid_t *) head) + i, list[i]);

	return 0;
}
