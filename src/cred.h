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

#if !defined(REMAINROOT_CRED_H)
#	define REMAINROOT_CRED_H
#endif /* !defined(REMAINROOT_CRED_H) */

/*
 * If we were included without SYSCALL definitions, then we default to
 * generating the prototype of the __rr_do_<syscall> shims. The reason
 * that LIBCALL and SYSCALL are separate is so that our different shim
 * methods can differentiate between libc and syscall functions.
 */

#if !defined(SYSCALL0)
#define AUTO_SYSCALL
#define SYSCALL0(type, func) \
	type __rr_do_ ## func(void)
#define SYSCALL1(type, func, type0, arg0) \
	type __rr_do_ ## func(type0 arg0)
#define SYSCALL2(type, func, type0, arg0, type1, arg1) \
	type __rr_do_ ## func(type0 arg0, type1 arg1)
#define SYSCALL3(type, func, type0, arg0, type1, arg1, type2, arg2) \
	type __rr_do_ ## func(type0 arg0, type1 arg1, type2 arg2)
#endif /* !defined(SYSCALL0) */

#if !defined(LIBCALL0)
#define AUTO_LIBCALL
#define LIBCALL0(type, func) \
	type __rr_do_ ## func(void)
#define LIBCALL1(type, func, type0, arg0) \
	type __rr_do_ ## func(type0 arg0)
#endif /* !defined(LIBCALL0) */

/* Shims for uid-based syscalls. */
SYSCALL1(int, setuid, uid_t, uid);
SYSCALL0(uid_t, getuid);
SYSCALL1(int, setfsuid, uid_t, fsuid);
SYSCALL2(int, setreuid, uid_t, ruid, uid_t, euid);
SYSCALL3(int, setresuid, uid_t, ruid, uid_t, euid, uid_t, suid);
SYSCALL3(int, getresuid, uid_t *, ruid, uid_t *, euid, uid_t *, suid);
LIBCALL1(int, seteuid, uid_t, euid);
LIBCALL0(uid_t, geteuid);

/* Shims for gid-based syscalls. */
SYSCALL1(int, setgid, gid_t, gid);
SYSCALL0(gid_t, getgid);
SYSCALL1(int, setfsgid, gid_t, fsgid);
SYSCALL2(int, setregid, gid_t, rgid, gid_t, egid);
SYSCALL3(int, setresgid, gid_t, rgid, gid_t, egid, gid_t, sgid);
SYSCALL3(int, getresgid, gid_t *, rgid, gid_t *, egid, gid_t *, sgid);
LIBCALL1(int, setegid, gid_t, egid);
LIBCALL0(gid_t, getegid);

/* Shims for supplementary gids. */
SYSCALL2(int, setgroups, int, size, const gid_t *, list);
SYSCALL2(int, getgroups, int, size, gid_t *, list);

/* Clean up automatic syscall definition. */
#if defined(AUTO_SYSCALL)
#	undef AUTO_SYSCALL
#	undef SYSCALL0
#	undef SYSCALL1
#	undef SYSCALL2
#	undef SYSCALL3
#endif

/* Clean up automatic libcall definition. */
#if defined(AUTO_LIBCALL)
#	undef AUTO_LIBCALL
#	undef LIBCALL0
#	undef LIBCALL1
#endif

#endif /* !defined(REMAINROOT_CRED_H) || defined(SYSCALL) || defined(LIBCALL) */
