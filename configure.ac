# remainroot: a shim to trick code to run in a rootless container
# Copyright (C) 2016 Aleksa Sarai <asarai@suse.de>
#
# remainroot is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# remainroot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with remainroot.  If not, see <http://www.gnu.org/licenses/>.

# In order to build remainroot, use `autoreconf -fi` and then do the
# standard `./configure && make && make install` dance.

AC_PREREQ([2.69])
AC_INIT([remainroot], [0.0.0], [], [], [https://github.com/cyphar/remainroot])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_CONFIG_SRCDIR([src/info.h])
AC_CONFIG_HEADERS([src/config.h])

# We need a temporary directory for libremain.so.
AS_TMPDIR([remainroot-build])
AC_SUBST([tmpdir], $tmp)

# Checks for programs.
AC_PROG_CC
AC_PATH_PROG([XXD], [xxd], [])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdint.h stdlib.h string.h unistd.h stdbool.h sys/syscall.h])
# ptrace
AC_CHECK_HEADERS([sys/ptrace.h sys/reg.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_TYPES([uid_t])
AC_CHECK_TYPES([gid_t])
AC_CHECK_TYPES([pid_t])
AC_CHECK_TYPES([ssize_t])
AC_CHECK_TYPES([uintptr_t])

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNC([putenv], [AC_DEFINE([HAVE_PUTENV], [1], [Define if putenv exists.])])
AC_CHECK_FUNC([ptrace], [AC_DEFINE([HAVE_PTRACE], [1], [Define if ptrace exists.])])

# Compiler characteristics.
AC_LANG([C])

# Checks for system services.

# Output.
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
AM_INIT_AUTOMAKE
