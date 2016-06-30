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

/* Main wrapper for core/, preload/ and ptrace/ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* All of the boilerplate text. */
#include "info.h"
#include "common.h"
#include "shims.h"

void usage(void)
{
	fprintf(stderr, REMAINROOT_USAGE, __progname);
}

void license(void)
{
	fprintf(stderr, REMAINROOT_LICENSE);
}

enum shim_t {
	PRELOAD,
	PTRACE,
};

struct config_t {
	enum shim_t shim;
};

void bake_args(struct config_t *config, int argc, char **argv)
{
	int c;
	struct option long_options[] = {
		{"shim-type", required_argument, NULL, 's'},
		{  "license",       no_argument, NULL, 'L'},
		{     "help",       no_argument, NULL, 'h'},
		{          0,                 0, NULL,   0},
	};

	/*
	 * It's critical that we don't parse and of the arguments for the
	 * child process. Since we're on GNU/Linux, we can use the "+" GNU
	 * extension. But we could similarly use POSIXLY_CORRECT.
	 */

	bool found_shim = false;
	while ((c = getopt_long(argc, argv, "+s:hL", long_options, NULL)) != -1) {
		switch (c) {
			case 's':
				found_shim = true;
				if (!strcmp(optarg, "ptrace"))
					config->shim = PTRACE;
				else if(!strcmp(optarg, "preload"))
					config->shim = PRELOAD;
				else
					rtfm("invalid shim type: %s", optarg);
				break;
			case 'L':
				license();
				exit(0);
				break;
			case 'h':
			case '?':
			case ':':
			default:
				usage();
				exit(c != 'h');
				break;
		}
	}

	if (!found_shim)
		rtfm("shim type required");
}

int main(int argc, char **argv)
{
	struct config_t config;
	bake_args(&config, argc, argv);

	/* Fix up our arguments to point to the slave process arguments. */
	argv += optind;
	argc -= optind;

	switch (config.shim) {
		case PRELOAD:
			shim_preload(argc, argv);
			break;
		case PTRACE:
			shim_ptrace(argc, argv);
			break;
		default:
			rtfm("shim type required");
			break;
	}

	return 0;
}
