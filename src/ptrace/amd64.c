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

/* ptrace/amd64.c is the register-specific magic for amd64. */

#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/reg.h>

#include "core/cred.h"
#include "generic.h"
#include "generic-shims.h"

long ptrace_syscall(pid_t pid)
{
	return ptrace(PTRACE_PEEKUSER, pid, sizeof(long)*ORIG_RAX);
}

uintptr_t ptrace_argument(pid_t pid, int arg)
{
	int reg = 0;
	switch (arg) {
		/* %rdi, %rsi, %rdx, %rcx, %r8 and %r9 */
		case 0:
			reg = RDI;
			break;
		case 1:
			reg = RSI;
			break;
		case 2:
			reg = RDX;
			break;
		case 3:
			reg = R10;
			break;
		case 4:
			reg = R8;
			break;
		case 5:
			reg = R9;
			break;
	}

	return ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * reg, NULL);
}

int ptrace_return(pid_t pid, uintptr_t ret)
{
	return ptrace(PTRACE_POKEUSER, pid, sizeof(long) * RAX, ret);
}

uintptr_t ptrace_deref_data(pid_t pid, uintptr_t addr)
{
	return ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
}

int ptrace_assign_data(pid_t pid, uintptr_t addr, uintptr_t value)
{
	return ptrace(PTRACE_POKEDATA, pid, addr, value);
}
