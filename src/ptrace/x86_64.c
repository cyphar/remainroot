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

/* ptrace/x86_64.c is the register-specific magic for amd64. */

#include <sys/ptrace.h>
#include <sys/reg.h>

#include "generic.h"

long ptrace_syscall_number(pid_t pid)
{
	return ptrace(PTRACE_PEEKUSER, pid, sizeof(long)*ORIG_RAX);
}

uintptr_t ptrace_syscall_argument(pid_t pid, int arg)
{
	return 0;
}
