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

#if !defined(PTRACE_GENERIC_H)
#define PTRACE_GENERIC_H

#include <stdint.h>
#include <sys/types.h>

/* Gets the syscall number. */
long ptrace_syscall(pid_t pid);

/* Deal with syscall arguments and return values. */
uintptr_t ptrace_argument(pid_t pid, int arg);
int ptrace_return(pid_t pid, uintptr_t ret);

/* TODO: Generic API to modify pointer arguments. */
uintptr_t ptrace_deref_data(pid_t pid, uintptr_t addr);
int ptrace_assign_data(pid_t pid, uintptr_t addr, uintptr_t value);

#endif
