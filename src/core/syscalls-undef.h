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

/* Clean up automatic syscall definition. */
#if defined(AUTO_SYSCALL)
#	undef AUTO_SYSCALL
#	undef SYSCALL0
#	undef SYSCALL1
#	undef SYSCALL2
#	undef SYSCALL3
#	undef SYSCALL4
#	undef SYSCALL5
#	undef SYSCALL6
#endif

/* Clean up automatic libcall definition. */
#if defined(AUTO_LIBCALL)
#	undef AUTO_LIBCALL
#	undef LIBCALL0
#	undef LIBCALL1
#endif
