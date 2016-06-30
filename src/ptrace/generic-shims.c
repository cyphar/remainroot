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
#include "core/cred.h"
#include "generic.h"
#include "generic-shims.h"

/* SYSCALL1(int, setuid, uid_t, uid) */
int ptrace_rr_setuid(pid_t pid, uintptr_t *ret)
{
	uid_t uid = ptrace_argument(pid, 0);
	*ret = __rr_do_setuid(uid);
	return 0;
}

/* SYSCALL0(uid_t, getuid) */
int ptrace_rr_getuid(pid_t pid, uintptr_t *ret)
{
	*ret = __rr_do_getuid();
	return 0;
}

/* SYSCALL1(int, setfsuid, uid_t, fsuid) */
int ptrace_rr_setfsuid(pid_t pid, uintptr_t *ret)
{
	uid_t fsuid = ptrace_argument(pid, 0);
	*ret = __rr_do_setfsuid(fsuid);
	return 0;
}

/* SYSCALL2(int, setreuid, uid_t, ruid, uid_t, euid) */
int ptrace_rr_setreuid(pid_t pid, uintptr_t *ret)
{
	uid_t ruid = ptrace_argument(pid, 0);
	uid_t euid = ptrace_argument(pid, 1);
	*ret = __rr_do_setreuid(ruid, euid);
	return 0;
}

/* SYSCALL3(int, setresuid, uid_t, ruid, uid_t, euid, uid_t, suid) */
int ptrace_rr_setresuid(pid_t pid, uintptr_t *ret)
{
	uid_t ruid = ptrace_argument(pid, 0);
	uid_t euid = ptrace_argument(pid, 1);
	uid_t suid = ptrace_argument(pid, 2);
	*ret = __rr_do_setresuid(ruid, euid, suid);
	return 0;
}

/* SYSCALL3(int, getresuid, uid_t *, ruid, uid_t *, euid, uid_t *, suid) */
int ptrace_rr_getresuid(pid_t pid, uintptr_t *ret)
{
	uid_t ruid, euid, suid;
	*ret = __rr_do_getresuid(&ruid, &euid, &suid);

	uintptr_t p_ruid = ptrace_argument(pid, 0);
	uintptr_t p_euid = ptrace_argument(pid, 1);
	uintptr_t p_suid = ptrace_argument(pid, 2);

	ptrace_assign_data(pid, p_ruid, ruid);
	ptrace_assign_data(pid, p_euid, euid);
	ptrace_assign_data(pid, p_suid, suid);
	return 0;
}

/* SYSCALL0(uid_t, geteuid) */
int ptrace_rr_geteuid(pid_t pid, uintptr_t *ret)
{
	*ret = __rr_do_geteuid();
	return 0;
}

/* SYSCALL1(int, setgid, gid_t, gid) */
int ptrace_rr_setgid(pid_t pid, uintptr_t *ret)
{
	gid_t gid = ptrace_argument(pid, 0);
	*ret = __rr_do_setgid(gid);
	return 0;
}

/* SYSCALL0(gid_t, getgid) */
int ptrace_rr_getgid(pid_t pid, uintptr_t *ret)
{
	*ret = __rr_do_getgid();
	return 0;
}

/* SYSCALL1(int, setfsgid, gid_t, fsgid) */
int ptrace_rr_setfsgid(pid_t pid, uintptr_t *ret)
{
	gid_t fsgid = ptrace_argument(pid, 0);
	*ret = __rr_do_setfsgid(fsgid);
	return 0;
}

/* SYSCALL2(int, setregid, gid_t, rgid, gid_t, egid) */
int ptrace_rr_setregid(pid_t pid, uintptr_t *ret)
{
	gid_t rgid = ptrace_argument(pid, 0);
	gid_t egid = ptrace_argument(pid, 1);
	*ret = __rr_do_setregid(rgid, egid);
	return 0;
}

/* SYSCALL3(int, setresgid, gid_t, rgid, gid_t, egid, gid_t, sgid) */
int ptrace_rr_setresgid(pid_t pid, uintptr_t *ret)
{
	gid_t rgid = ptrace_argument(pid, 0);
	gid_t egid = ptrace_argument(pid, 1);
	gid_t sgid = ptrace_argument(pid, 2);
	*ret = __rr_do_setresgid(rgid, egid, sgid);
	return 0;
}

/* SYSCALL3(int, getresgid, gid_t *, rgid, gid_t *, egid, gid_t *, sgid) */
int ptrace_rr_getresgid(pid_t pid, uintptr_t *ret)
{
	gid_t rgid, egid, sgid;
	*ret = __rr_do_getresgid(&rgid, &egid, &sgid);

	uintptr_t p_rgid = ptrace_argument(pid, 0);
	uintptr_t p_egid = ptrace_argument(pid, 1);
	uintptr_t p_sgid = ptrace_argument(pid, 2);

	ptrace_assign_data(pid, p_rgid, rgid);
	ptrace_assign_data(pid, p_egid, egid);
	ptrace_assign_data(pid, p_sgid, sgid);
	return 0;
}

/* SYSCALL0(gid_t, getegid) */
int ptrace_rr_getegid(pid_t pid, uintptr_t *ret)
{
	*ret = __rr_do_getegid();
	return 0;
}

/* SYSCALL2(int, setgroups, int, size, const gid_t *, list) */
int ptrace_rr_setgroups(pid_t pid, uintptr_t *ret)
{
	int size = ptrace_argument(pid, 0);
	uintptr_t head = ptrace_argument(pid, 1);
	gid_t list[NGROUPS_MAX] = {0};

	for (int i = 0; i < size; i++)
		list[i] = ptrace_deref_data(pid, (uintptr_t) ((gid_t *) head) + i);

	*ret = __rr_do_setgroups(size, list);
	return 0;
}

/* SYSCALL2(int, getgroups, int, size, gid_t *, list) */
int ptrace_rr_getgroups(pid_t pid, uintptr_t *ret)
{
	int size = ptrace_argument(pid, 0);
	uintptr_t head = ptrace_argument(pid, 1);
	gid_t list[NGROUPS_MAX] = {0};

	*ret = __rr_do_getgroups(size, list);
	for (int i = 0; i < size; i++)
		ptrace_assign_data(pid, (uintptr_t) ((gid_t *) head) + i, list[i]);

	return 0;
}
