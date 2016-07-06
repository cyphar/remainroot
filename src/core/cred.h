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

#if !defined(REMAINROOT_CRED_H) || defined(SYSCALL0) || defined(LIBCALL0)
#include <sys/types.h>
#include <stdbool.h>
#include <limits.h>

#if !defined(REMAINROOT_CRED_H)
#define REMAINROOT_CRED_H

/* This effectively mirrors the cred structure in the Linux kernel. */
struct cred_t {
	bool cap_setuid;
	uid_t uid,
		  euid,
		  suid,
		  fsuid;

	bool cap_setgid;
	gid_t gid,
		  egid,
		  sgid,
		  fsgid;

	int ngroups;
	gid_t groups[NGROUPS_MAX];

	/* TODO: ->securebits and ->*_set support. */
};

/* Initiates a new cred_t with the current process context. */
void new_cred(struct cred_t *cred);

/* Clones a cred_t, so it can be used for another process */
void clone_cred(struct cred_t *new, struct cred_t *old);

#endif /* !defined(REMAINROOT_CRED_H) */

/* TODO: Separate all of this into a separate header. */

/* SYSCALL and LIBCALL definitions. */
#include "syscalls-def.h"

/* Shims for uid-based syscalls. */
SYSCALL1(int, setuid, uid_t, uid)
SYSCALL0(uid_t, getuid)
SYSCALL1(int, setfsuid, uid_t, fsuid)
SYSCALL2(int, setreuid, uid_t, ruid, uid_t, euid)
SYSCALL3(int, setresuid, uid_t, ruid, uid_t, euid, uid_t, suid)
SYSCALL3(int, getresuid, uid_t *, ruid, uid_t *, euid, uid_t *, suid)
LIBCALL1(int, seteuid, uid_t, euid)
SYSCALL0(uid_t, geteuid)

/* Shims for gid-based syscalls. */
SYSCALL1(int, setgid, gid_t, gid)
SYSCALL0(gid_t, getgid)
SYSCALL1(int, setfsgid, gid_t, fsgid)
SYSCALL2(int, setregid, gid_t, rgid, gid_t, egid)
SYSCALL3(int, setresgid, gid_t, rgid, gid_t, egid, gid_t, sgid)
SYSCALL3(int, getresgid, gid_t *, rgid, gid_t *, egid, gid_t *, sgid)
LIBCALL1(int, setegid, gid_t, egid)
SYSCALL0(gid_t, getegid)

/* Shims for supplementary gids. */
SYSCALL2(int, setgroups, int, size, const gid_t *, list)
SYSCALL2(int, getgroups, int, size, gid_t *, list)

/* Clean up. */
#include "syscalls-undef.h"

#endif /* !defined(REMAINROOT_CRED_H) || defined(SYSCALL) || defined(LIBCALL) */
