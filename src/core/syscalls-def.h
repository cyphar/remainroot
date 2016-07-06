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
 * If we were included without SYSCALL definitions, then we default to
 * generating the prototype of the __rr_do_<syscall> shims. The reason
 * that LIBCALL and SYSCALL are separate is so that our different shim
 * methods can differentiate between libc and syscall functions.
 */

#if !defined(SYSCALL0)
#define AUTO_SYSCALL
#define SYSCALL0(type, func) \
	type __rr_do_ ## func(struct cred_t *current);
#define SYSCALL1(type, func, type0, arg0) \
	type __rr_do_ ## func(struct cred_t *current, type0 arg0);
#define SYSCALL2(type, func, type0, arg0, type1, arg1) \
	type __rr_do_ ## func(struct cred_t *current, type0 arg0, type1 arg1);
#define SYSCALL3(type, func, type0, arg0, type1, arg1, type2, arg2) \
	type __rr_do_ ## func(struct cred_t *current, type0 arg0, type1 arg1, type2 arg2);
#define SYSCALL4(type, func, type0, arg0, type1, arg1, type2, arg2, type3, arg3) \
	type __rr_do_ ## func(struct cred_t *current, type0 arg0, type1 arg1, type2 arg2, type3 arg3);
#define SYSCALL5(type, func, type0, arg0, type1, arg1, type2, arg2, type3, arg3, type4, arg4) \
	type __rr_do_ ## func(struct cred_t *current, type0 arg0, type1 arg1, type2 arg2, type3 arg3, type4 arg4);
#define SYSCALL6(type, func, type0, arg0, type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5) \
	type __rr_do_ ## func(struct cred_t *current, type0 arg0, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5);
#endif /* !defined(SYSCALL0) */

#if !defined(LIBCALL0)
#define AUTO_LIBCALL
#define LIBCALL0(type, func) SYSCALL0(type, func)
#define LIBCALL1(type, func, type0, arg0) SYSCALL1(type, func, type0, arg0)
#endif /* !defined(LIBCALL0) */
