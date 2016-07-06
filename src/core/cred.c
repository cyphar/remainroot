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
 * cred.c defines the shims for all of the credential-related functions.
 * The shims are of the form __rr_do_<syscall>, so they'll show up in
 * backtraces and debugging logs. Look at preload-symbols.c to see where
 * the actual libc functions are being overridden.
 *
 * Note that this whole concept doesn't account for some incredibly
 * tricky ways that a process could find its current user id. For
 * example, a process could mmap(/proc/self/status) and then parse the
 * memory to figure out what its {uid, gid} are. A process could also
 * just open that file. Or create a file and then stat it in order to
 * figure out what user it is. Currently we don't fix this (because it
 * would be _incredibly_ hard and would require tracking so many more
 * things than the current {uid, gid}).
 *
 * TODO: Is it even possible to handle all of those cases?
 */

#define _GNU_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <syscall.h>

/* Verify that we don't break the default prototypes. */
#include "cred.h"

/*
 * Okay, so it turns out that glibc implements a POSIX-compatible thread
 * library called nptl(7). According to POSIX, each thread should have
 * the same credentials (this is not the case on GNU/Linux, where each
 * task has its own credential set). glibc implements the POSIX API by
 * syncing all of the tasks together through SIGRT32 and SIGRT33.
 *
 * The upshot is that if we're using LD_PRELOAD we don't need to store
 * any thread-local state for the credentials, just global because
 * that's what glibc would emulate anyway. I'm not sure how musl does
 * this, but if it implements POSIX it must do something similar.
 *
 * Further information on the Linux credentials API can be found in
 * linux/kernel/sys.c, as well as the relevant man pages.
 */

/******************************************************************
 * This section implements all of the uid-based credential shims. *
 ******************************************************************/

/* Shorten tedious code. */
#define BSD_UID_ACCESS(current, var, check_suid) \
	((var) == (uid_t) -1 || \
	 (var) == (current)->uid || \
	 (var) == (current)->euid || \
	 ((check_suid) && (var) == (current)->suid))

int __rr_do_setuid(struct cred_t *current, uid_t uid)
{
	if (current->cap_setuid)
		current->uid = current->euid = current->suid = uid;
	else if (uid == current->uid || uid == current->suid)
		current->euid = uid;
	else
		goto error;

	current->fsuid = current->euid;
	return 0;

error:
	return -EPERM;
}

uid_t __rr_do_getuid(struct cred_t *current)
{
	return current->uid;
}

int __rr_do_setfsuid(struct cred_t *current, uid_t fsuid)
{
	uid_t old_fsuid = fsuid;

	if (!current->cap_setuid)
		if (!BSD_UID_ACCESS(current, fsuid, true) && fsuid != current->fsuid)
			goto error;

	current->fsuid = fsuid;
	/* fallthrough */

error:
	return old_fsuid;
}

int __rr_do_setreuid(struct cred_t *current, uid_t ruid, uid_t euid)
{
	uid_t old_uid = current->uid;

	if (!current->cap_setuid) {
		if (!BSD_UID_ACCESS(current, ruid, false))
			goto error;
		if (!BSD_UID_ACCESS(current, euid, true))
			goto error;
	}

	if (ruid != (uid_t) -1)
		current->uid = ruid;
	if (euid != (uid_t) -1)
		current->euid = euid;
	if (ruid != (uid_t) -1 || (euid != (uid_t) -1 && euid != old_uid))
		current->suid = current->euid;

	current->fsuid = current->euid;
	return 0;

error:
	return -EPERM;
}

/* There is no getreuid(2). */

int __rr_do_setresuid(struct cred_t *current, uid_t ruid, uid_t euid, uid_t suid)
{
	if (!current->cap_setuid) {
		if (!BSD_UID_ACCESS(current, ruid, true))
			goto error;
		if (!BSD_UID_ACCESS(current, euid, true))
			goto error;
		if (!BSD_UID_ACCESS(current, suid, true))
			goto error;
	}

	if (ruid != (uid_t) -1)
		current->uid = ruid;
	if (euid != (uid_t) -1)
		current->euid = euid;
	if (suid != (uid_t) -1)
		current->suid = suid;

	current->fsuid = current->euid;
	return 0;

error:
	return -EPERM;
}

int __rr_do_getresuid(struct cred_t *current, uid_t *ruid, uid_t *euid, uid_t *suid)
{
	*ruid = current->uid;
	*euid = current->euid;
	*suid = current->suid;
	return 0;
}

/*
 * NOTE: On GNU/Linux these are library calls, that are implemented
 * using setreuid and getresuid.
 */

int __rr_do_seteuid(struct cred_t *current, uid_t euid)
{
	if (euid == (uid_t) -1)
		goto error;

	return __rr_do_setreuid(current, -1, euid);

error:
	return -EPERM;
}

uid_t __rr_do_geteuid(struct cred_t *current)
{
	return current->euid;
}

/******************************************************************
 * This section implements all of the gid-based credential shims. *
 ******************************************************************/

/* Shorten tedious code. */
#define BSD_GID_ACCESS(current, var, check_sgid) \
	((var) == (gid_t) -1 || \
	 (var) == (current)->gid || \
	 (var) == (current)->egid || \
	 ((check_sgid) && (var) == (current)->sgid))

int __rr_do_setgid(struct cred_t *current, gid_t gid)
{
	if (current->cap_setgid)
		current->gid = current->egid = current->sgid = gid;
	else if (gid == current->gid || gid == current->sgid)
		current->egid = gid;
	else
		goto error;

	current->fsgid = current->egid;
	return 0;

error:
	return -EPERM;
}

gid_t __rr_do_getgid(struct cred_t *current)
{
	return current->gid;
}

int __rr_do_setfsgid(struct cred_t *current, gid_t fsgid)
{
	gid_t old_fsgid = fsgid;

	if (!current->cap_setgid)
		if (!BSD_GID_ACCESS(current, fsgid, true) && fsgid != current->fsgid)
			goto error;

	current->fsgid = fsgid;
	/* fallthrough */

error:
	return old_fsgid;
}

int __rr_do_setregid(struct cred_t *current, gid_t rgid, gid_t egid)
{
	gid_t old_gid = current->gid;

	if (!current->cap_setgid) {
		if (!BSD_GID_ACCESS(current, rgid, false))
			goto error;
		if (!BSD_GID_ACCESS(current, egid, true))
			goto error;
	}

	if (rgid != (gid_t) -1)
		current->gid = rgid;
	if (egid != (gid_t) -1)
		current->egid = egid;
	if (rgid != (gid_t) -1 || (egid != (gid_t) -1 && egid != old_gid))
		current->sgid = current->egid;

	current->fsgid = current->egid;
	return 0;

error:
	return -EPERM;
}

/* There is no getregid(2). */

int __rr_do_setresgid(struct cred_t *current, gid_t rgid, gid_t egid, gid_t sgid)
{
	if (!current->cap_setgid) {
		if (!BSD_GID_ACCESS(current, rgid, true))
			goto error;
		if (!BSD_GID_ACCESS(current, egid, true))
			goto error;
		if (!BSD_GID_ACCESS(current, sgid, true))
			goto error;
	}

	if (rgid != (gid_t) -1)
		current->gid = rgid;
	if (egid != (gid_t) -1)
		current->egid = egid;
	if (sgid != (gid_t) -1)
		current->sgid = sgid;

	current->fsgid = current->egid;
	return 0;

error:
	return -EPERM;
}

int __rr_do_getresgid(struct cred_t *current, gid_t *rgid, gid_t *egid, gid_t *sgid)
{
	*rgid = current->gid;
	*egid = current->egid;
	*sgid = current->sgid;
	return 0;
}

/*
 * NOTE: On GNU/Linux these are library calls, that are implemented
 * using setregid and getresgid.
 */

int __rr_do_setegid(struct cred_t *current, gid_t egid)
{
	if (egid == (gid_t) -1)
		goto error;

	return __rr_do_setregid(current, -1, egid);

error:
	return -EPERM;
}

gid_t __rr_do_getegid(struct cred_t *current)
{
	return current->egid;
}

/*
 * It is always invalid to call setgroups(2) in a recent Linux kernel
 * version rootless container.
 */

int __rr_do_setgroups(struct cred_t *current, int size, const gid_t *list)
{
	if (size <= 0 || size > NGROUPS_MAX)
		goto error_value;

	if (!current->cap_setgid)
		goto error_perm;

	for (int i = 0; i < size; i++)
		current->groups[i] = list[i];

	return 0;

error_value:
	return -EINVAL;

error_perm:
	return -EPERM;
}

int __rr_do_getgroups(struct cred_t *current, int size, gid_t *list)
{
	if (size == 0)
		goto exit;

	if (size < current->ngroups || size > NGROUPS_MAX)
		goto error;

	for (int i = 0; i < current->ngroups; i++)
		list[i] = current->groups[i];

	/* fallthrough */

exit:
	return current->ngroups;

error:
	return -EINVAL;
}

/* TODO: Actually get this from /proc/sys/kernel/overflowgid. */
#define OVERFLOW_GID 65534

void new_cred(struct cred_t *current)
{
	/*
	 * Set up defaults. Still need to make this use the current set of
	 * priviliges.
	 */
	*current = (struct cred_t) {
		.cap_setuid = true,
		.uid        = 0,
		.euid       = 0,
		.suid       = 0,
		.fsuid      = 0,
		.cap_setgid = true,
		.gid        = 0,
		.egid       = 0,
		.sgid       = 0,
		.fsgid      = 0,
	};

	/* Set up supplementary groups. */
	gid_t groups[NGROUPS_MAX] = {0};
	current->ngroups = 0;

	/* If this fails, we can't do anything about it. */
	int len = syscall(SYS_getgroups, NGROUPS_MAX, groups);
	if (len < 0)
		return;

	for (int i = 0; i < len; i++) {
		if (groups[i] != OVERFLOW_GID)
			current->groups[current->ngroups++] = groups[i];
	}
}

void clone_cred(struct cred_t *new, struct cred_t *old)
{
	*new = *old;
}
