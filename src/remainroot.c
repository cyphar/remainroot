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

struct shim_t {
	char *name;
	void (*fn)(int argc, char **argv);
};

static struct shim_t shims[] = {
	{
		.name = "ptrace",
		.fn = shim_ptrace,
	},
	{
		.name = "preload",
		.fn = shim_preload,
	},
	{0},
};

struct shim_t *get_shim(char *name)
{
	for (struct shim_t *p = shims; p->fn != NULL; p++)
		if (!strcmp(name, p->name))
			return p;
	return NULL;
}

/* TODO: Make this an autoconf option. */
#define DEFAULT_SHIM "ptrace"

struct config_t {
	struct shim_t shim;
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

	/* Parse the default shim. */
	struct shim_t *shim = get_shim(DEFAULT_SHIM);
	if (shim)
		config->shim = *shim;

	/*
	 * It's critical that we don't parse and of the arguments for the
	 * child process. Since we're on GNU/Linux, we can use the "+" GNU
	 * extension. But we could similarly use POSIXLY_CORRECT.
	 */

	while ((c = getopt_long(argc, argv, "+s:hL", long_options, NULL)) != -1) {
		switch (c) {
			case 's':
				shim = get_shim(optarg);
				if (shim)
					config->shim = *shim;
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

	if (!config->shim.fn)
		rtfm("shim type required");
}

int main(int argc, char **argv)
{
	struct config_t config = {0};
	bake_args(&config, argc, argv);

	/* Fix up our arguments to point to the slave process arguments. */
	argv += optind;
	argc -= optind;

	/* In to the shim we go. */
	config.shim.fn(argc, argv);

	/* We should never get here. */
	return 1;
}
