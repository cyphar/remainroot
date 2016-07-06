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

/* core/ is the current state. */

#include "core/proc.h"
#include "core/cred.h"

void proc_new(struct proc_t *proc)
{
	cred_new(&proc->cred);
}

/* Clones a proc_t, so it can be used for another process */
void proc_clone(struct proc_t *new, struct proc_t *old)
{
	new->pid = old->pid;
	cred_clone(&new->cred, &old->cred);
}
