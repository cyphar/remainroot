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

/*
 * fork.c deals with tracking child processes and threads. This just
 * results in us being able to correctly shim all of the child
 * processes. There isn't anything _too_ magical here.
 *
 * XXX: Do we actually need this in the core/ code? LD_PRELOAD just
 *      requires that we always set the environment variable before an
 *      execve. And ptrace is a bit more tricky, but we always track on
 *      a per-process basis, so the actual tracking we have to do is
 *      very different. This means that there really isn't much in
 *      common between the two shim injection methods.
 */

/* TODO */
