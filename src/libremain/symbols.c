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
#include "core/cred.h"

/* Automatically generates all of the relevant shims. */
#define SYSCALL0(type, func) \
	type func(void) \
	{ return __rr_do_ ## func(); }
#define SYSCALL1(type, func, type0, arg0) \
	type func(type0 arg0) \
	{ return __rr_do_ ## func(arg0); }
#define SYSCALL2(type, func, type0, arg0, type1, arg1) \
	type func(type0 arg0, type1 arg1) \
	{ return __rr_do_ ## func(arg0, arg1); }
#define SYSCALL3(type, func, type0, arg0, type1, arg1, type2, arg2) \
	type func(type0 arg0, type1 arg1, type2 arg2) \
	{ return __rr_do_ ## func(arg0, arg1, arg2); }

/* On the libc layer, LIBCALL == SYSCALL. */
#define LIBCALL0(type, func) SYSCALL0(type, func)
#define LIBCALL1(type, func, type0, arg0) SYSCALL1(type, func, type0, arg0)

#include "core/cred.h"
