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

#if !defined(REMAINROOT_COMMON_H)
#define REMAINROOT_COMMON_H

#include <stdio.h>

extern char *__progname;

/* Error messages. */
#define die(...) \
	do { \
		fprintf(stderr, "%s: ", __progname); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		exit(1); \
	} while(0)
#define rtfm(...) \
	do { \
		fprintf(stderr, "%s: ", __progname); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		usage(); \
		exit(1); \
	} while(0)

#endif /* !defined(REMAINROOT_COMMON_H) */
