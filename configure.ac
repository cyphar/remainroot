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

# Define AC_MSG_ERROR.
m4_define([AC_MSG_ERROR],
       [{ AS_MESSAGE([error: $1], [2])
          exit m4_default([$2], [1]); }])

# Fix up ifdef guards.
AH_TOP([
#if !defined(CONFIG_H)
#define CONFIG_H
])
AH_BOTTOM([
#endif /* !defined(CONFIG_H) */
])

# We need a temporary directory for libremain.so.
AS_TMPDIR([remainroot-build])
AC_SUBST([tmpdir], $tmp)

# Checks for programs.
AC_PROG_CC
AC_PATH_PROG([XXD], [xxd], [])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdint.h stdlib.h string.h unistd.h stdbool.h syscall.h sys/syscall.h])
# ptrace
AC_CHECK_HEADERS([sys/ptrace.h sys/reg.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_SSIZE_T
AC_TYPE_UINTPTR_T

# Checks for library functions.
AC_FUNC_FORK

# We need ptrace.
AC_CHECK_FUNC([ptrace], [AC_DEFINE([HAVE_PTRACE], [1], [Define if ptrace exists.])])

# Compiler characteristics.
AC_LANG([C])

# Check that xxd works.
#AC_COMPILE_IFELSE()

# Checks for system services.

# Output.
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
AM_INIT_AUTOMAKE
