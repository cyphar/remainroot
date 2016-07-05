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
#define BSD_UID_ACCESS(var, suid) \
	((var) == (uid_t) -1 || \
	 (var) == current_uid || \
	 (var) == current_euid || \
	 ((suid) && (var) == current_suid))

static bool current_cap_setuid = true;
static uid_t current_uid = 0;
static uid_t current_euid = 0;
static uid_t current_suid = 0;
static uid_t current_fsuid = 0;

int __rr_do_setuid(uid_t uid)
{
	if (current_cap_setuid)
		current_uid = current_euid = current_suid = uid;
	else if (uid == current_uid || uid == current_suid)
		current_euid = uid;
	else
		goto error;

	current_fsuid = current_euid;
	return 0;

error:
	return -EPERM;
}

uid_t __rr_do_getuid(void)
{
	return current_uid;
}

int __rr_do_setfsuid(uid_t fsuid)
{
	uid_t old_fsuid = fsuid;

	if (!current_cap_setuid)
		if (!BSD_UID_ACCESS(fsuid, true) && fsuid != current_fsuid)
			goto error;

	current_fsuid = fsuid;
	/* fallthrough */

error:
	return old_fsuid;
}

int __rr_do_setreuid(uid_t ruid, uid_t euid)
{
	uid_t old_uid = current_uid;

	if (!current_cap_setuid) {
		if (!BSD_UID_ACCESS(ruid, false))
			goto error;
		if (!BSD_UID_ACCESS(euid, true))
			goto error;
	}

	if (ruid != (uid_t) -1)
		current_uid = ruid;
	if (euid != (uid_t) -1)
		current_euid = euid;
	if (ruid != (uid_t) -1 || (euid != (uid_t) -1 && euid != old_uid))
		current_suid = current_euid;

	current_fsuid = current_euid;
	return 0;

error:
	return -EPERM;
}

/* There is no getreuid(2). */

int __rr_do_setresuid(uid_t ruid, uid_t euid, uid_t suid)
{
	if (!current_cap_setuid) {
		if (!BSD_UID_ACCESS(ruid, true))
			goto error;
		if (!BSD_UID_ACCESS(euid, true))
			goto error;
		if (!BSD_UID_ACCESS(suid, true))
			goto error;
	}

	if (ruid != (uid_t) -1)
		current_uid = ruid;
	if (euid != (uid_t) -1)
		current_euid = euid;
	if (suid != (uid_t) -1)
		current_suid = suid;

	current_fsuid = current_euid;
	return 0;

error:
	return -EPERM;
}

int __rr_do_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid)
{
	/* This is so we can get EFAULT without segfaulting. */
	if (getresuid(ruid, euid, suid) < 0)
		return -errno;

	*ruid = current_uid;
	*euid = current_euid;
	*suid = current_suid;
	return 0;
}

/*
 * NOTE: On GNU/Linux these are library calls, that are implemented
 * using setreuid and getresuid.
 */

int __rr_do_seteuid(uid_t euid)
{
	if (euid == (uid_t) -1)
		goto error;

	return __rr_do_setreuid(-1, euid);

error:
	return -EPERM;
}

uid_t __rr_do_geteuid(void)
{
	return current_euid;
}

/******************************************************************
 * This section implements all of the gid-based credential shims. *
 ******************************************************************/

/* Shorten tedious code. */
#define BSD_GID_ACCESS(var, sgid) \
	((var) == (gid_t) -1 || \
	 (var) == current_gid || \
	 (var) == current_egid || \
	 ((sgid) && (var) == current_sgid))

static bool current_cap_setgid = true;
static gid_t current_gid = 0;
static gid_t current_egid = 0;
static gid_t current_sgid = 0;
static gid_t current_fsgid = 0;

int __rr_do_setgid(gid_t gid)
{
	if (current_cap_setgid)
		current_gid = current_egid = current_sgid = gid;
	else if (gid == current_gid || gid == current_sgid)
		current_egid = gid;
	else
		goto error;

	current_fsgid = current_egid;
	return 0;

error:
	return -EPERM;
}

gid_t __rr_do_getgid(void)
{
	return current_gid;
}

int __rr_do_setfsgid(gid_t fsgid)
{
	gid_t old_fsgid = fsgid;

	if (!current_cap_setgid)
		if (!BSD_GID_ACCESS(fsgid, true) && fsgid != current_fsgid)
			goto error;

	current_fsgid = fsgid;
	/* fallthrough */

error:
	return old_fsgid;
}

int __rr_do_setregid(gid_t rgid, gid_t egid)
{
	gid_t old_gid = current_gid;

	if (!current_cap_setgid) {
		if (!BSD_GID_ACCESS(rgid, false))
			goto error;
		if (!BSD_GID_ACCESS(egid, true))
			goto error;
	}

	if (rgid != (gid_t) -1)
		current_gid = rgid;
	if (egid != (gid_t) -1)
		current_egid = egid;
	if (rgid != (gid_t) -1 || (egid != (gid_t) -1 && egid != old_gid))
		current_sgid = current_egid;

	current_fsgid = current_egid;
	return 0;

error:
	return -EPERM;
}

/* There is no getregid(2). */

int __rr_do_setresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
	if (!current_cap_setgid) {
		if (!BSD_GID_ACCESS(rgid, true))
			goto error;
		if (!BSD_GID_ACCESS(egid, true))
			goto error;
		if (!BSD_GID_ACCESS(sgid, true))
			goto error;
	}

	if (rgid != (gid_t) -1)
		current_gid = rgid;
	if (egid != (gid_t) -1)
		current_egid = egid;
	if (sgid != (gid_t) -1)
		current_sgid = sgid;

	current_fsgid = current_egid;
	return 0;

error:
	return -EPERM;
}

int __rr_do_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid)
{
	/* This is so we can get EFAULT without segfaulting. */
	if (getresgid(rgid, egid, sgid) < 0)
		return -errno;

	*rgid = current_gid;
	*egid = current_egid;
	*sgid = current_sgid;
	return 0;
}

/*
 * NOTE: On GNU/Linux these are library calls, that are implemented
 * using setregid and getresgid.
 */

int __rr_do_setegid(gid_t egid)
{
	if (egid == (gid_t) -1)
		goto error;

	return __rr_do_setregid(-1, egid);

error:
	return -EPERM;
}

gid_t __rr_do_getegid(void)
{
	return current_egid;
}

/*
 * It is always invalid to call setgroups(2) in a recent Linux kernel
 * version rootless container.
 */

/*
 * TODO: We need to initialise this with a filtered version of
 *       getgroups(2). Filtering essentially means that we remove every
 *       overflow uid and gid.
 */

static int current_ngids = 0;
static gid_t current_gids[NGROUPS_MAX] = {0};

int __rr_do_setgroups(int size, const gid_t *list)
{
	if (size <= 0 || size > NGROUPS_MAX)
		goto error_value;

	if (!current_cap_setgid)
		goto error_perm;

	for (int i = 0; i < size; i++)
		current_gids[i] = list[i];

	return 0;

error_value:
	return -EINVAL;

error_perm:
	return -EPERM;
}

int __rr_do_getgroups(int size, gid_t *list)
{
	if (size == 0)
		goto exit;

	if (size < current_ngids || size > NGROUPS_MAX)
		goto error;

	for (int i = 0; i < current_ngids; i++)
		list[i] = current_gids[i];

	/* fallthrough */

exit:
	return current_ngids;

error:
	return -EINVAL;
}

/* TODO: Actually get this from /proc/sys/kernel/overflowgid. */
#define OVERFLOW_GID 65534

/* This sets up current_gids. */
void __rr_init_cred(void) __attribute__((constructor));
void __rr_init_cred(void)
{
	gid_t gids[NGROUPS_MAX] = {0};
	current_ngids = 0;

	/* If this fails, we can't do anything about it. */
	int len = syscall(SYS_getgroups, NGROUPS_MAX, gids);
	if (len < 0)
		return;

	for (int i = 0; i < len; i++) {
		if (gids[i] != OVERFLOW_GID)
			current_gids[current_ngids++] = gids[i];
	}
}
