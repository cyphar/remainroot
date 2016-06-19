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
 * file.c deals with faking chown and other file-related syscalls. This
 * is incredibly ugly, since it's not nearly as simple to track as fake
 * credentials. In order to do this properly, we would need to keep the
 * entire list of all inodes accessed in memory so that we can correctly
 * swap out the owner of a file when it is stat(2)ed. In addition, there
 * are so many issues that could arise by another process attempting to
 * chown(2) a file even though we're faking the syscall -- how on earth
 * do we keep track of that? So, we attempt to do this the simple way.
 *
 * In addition, many of the caveats in cred.c apply to this set of
 * shims. Luckily, there isn't such a trivial bypass in /proc (in
 * contrast to /proc/self/status).
 */

/* TODO */
