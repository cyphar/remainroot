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

/* To get the prototypes. */
#include <errno.h>
#include "core/cred.h"

/* Generic shim body that deals with errno. */
#define SYSCALL_BODY(type, func, ...) \
	{ \
		type ret = __rr_do_ ## func(__VA_ARGS__); \
		if (ret < 0) { \
			errno = -ret; \
			return -1; \
		} \
		return ret; \
	}

/* Automatically generates all of the relevant shims. */
#define SYSCALL0(type, func) \
	type func(void) \
	SYSCALL_BODY(type, func)
#define SYSCALL1(type, func, type0, arg0) \
	type func(type0 arg0) \
	SYSCALL_BODY(type, func, arg0)
#define SYSCALL2(type, func, type0, arg0, type1, arg1) \
	type func(type0 arg0, type1 arg1) \
	SYSCALL_BODY(type, func, arg0, arg1)
#define SYSCALL3(type, func, type0, arg0, type1, arg1, type2, arg2) \
	type func(type0 arg0, type1 arg1, type2 arg2) \
	SYSCALL_BODY(type, func, arg0, arg1, arg2)
#define SYSCALL4(type, func, type0, arg0, type1, arg1, type2, arg2, type3, arg3) \
	type func(type0 arg0, type1 arg1, type2 arg2, type3 arg3) \
	SYSCALL_BODY(type, func, arg0, arg1, arg2, arg3)
#define SYSCALL5(type, func, type0, arg0, type1, arg1, type2, arg2, type3, arg3, type4, arg4) \
	type func(type0 arg0, type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
	SYSCALL_BODY(type, func, arg0, arg1, arg2, arg3, arg4)
#define SYSCALL6(type, func, type0, arg0, type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5) \
	type func(type0 arg0, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) \
	SYSCALL_BODY(type, func, arg0, arg1, arg2, arg3, arg4, arg5)

/* On the libc layer, LIBCALL == SYSCALL. */
#define LIBCALL0(type, func) SYSCALL0(type, func)
#define LIBCALL1(type, func, type0, arg0) SYSCALL1(type, func, type0, arg0)

#include "core/cred.h"
