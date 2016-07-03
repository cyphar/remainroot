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

#if !defined(REMAINROOT_INFO_H)
#define REMAINROOT_INFO_H

#include "config.h"

#define REMAINROOT_USAGE \
"usage: " PACKAGE " --shim-type <shim> <program> [<argument> ...]\n" \
"\n" \
"A collection of shims to allow unmodified programs to run nicely inside\n" \
"rootless containers. In particular, this wrapper will use several\n" \
"techniques to fool a process into thinking that it is successfully\n" \
"running syscalls that are forbidden in rootless containers. It will also\n" \
"fake the side-effects of each syscall to make it even more authentic.\n" \
"\n" \
"DO NOT SHIM OUT A PROGRAM THAT IS NOT RUNNING IN A ROOTLESS CONTAINER!\n" \
"It will almost certainly result in some very unpleasant security issues.\n" \
"\n" \
"options:\n" \
"  -h, --help              show this help page\n" \
"  -L, --license           show the license information\n" \
"  -s, --shim-type <shim>  which shim method to use on the program\n" \
"                          (valid options are 'preload', 'ptrace')\n" \
"\n" \
"The remaining arguments are taken to be the program name and arguments\n" \
"to be fooled by this program.\n"

#define REMAINROOT_LICENSE \
PACKAGE ": a shim to trick code to run in a rootless container\n" \
"Copyright (C) 2016 Aleksa Sarai <asarai@suse.de>\n" \
"\n" \
PACKAGE " is free software: you can redistribute it and/or modify\n" \
"it under the terms of the GNU General Public License as published by\n" \
"the Free Software Foundation, either version 3 of the License, or\n" \
"(at your option) any later version.\n" \
"\n" \
PACKAGE " is distributed in the hope that it will be useful,\n" \
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
"GNU General Public License for more details.\n" \
"\n" \
"You should have received a copy of the GNU General Public License\n" \
"along with " PACKAGE ".  If not, see <http://www.gnu.org/licenses/>.\n"

#endif /* !define(REMAINROOT_INFO_H) */
