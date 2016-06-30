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

/* generic-shims.h is the prototypes for the generic ptrace shims */

#if !defined(PTRACE_GENERIC_SHIMS_H)
#define PTRACE_GENERIC_SHIMS_H

#include <stdint.h>
#include <sys/types.h>

/* XXX: I think I'm overusing this hack. */
#define SYSCALL(func) int ptrace_rr_ ## func(pid_t, uintptr_t *);
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
#undef SYSCALL
#undef SYSCALL0
#undef SYSCALL1
#undef SYSCALL2
#undef SYSCALL3
#undef SYSCALL4
#undef SYSCALL5
#undef SYSCALL6
#undef LIBCALL0
#undef LIBCALL1

#endif /* !defined(PTRACE_GENERIC_SHIMS_H) */
