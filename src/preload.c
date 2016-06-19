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

/* preload.c is the front-end for the LD_PRELOAD shim. */

#include <stdlib.h>
#include <unistd.h>

#include "common.h"

void shim_preload(int argc, char **argv)
{
	/* TODO: Fix this. */
	if (putenv("LD_PRELOAD=./libremain.so") < 0)
		die("couldn't set LD_PRELOAD");

	execvp(argv[0], argv);

	/* Should never be reached. */
	die("execvp failed");
}
