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

# remainroot
bin_PROGRAMS = remainroot
remainroot_SOURCES = remainroot.c core/cred.c
remainroot_SOURCES += ptrace.c ptrace/generic-shims.c ptrace/amd64.c
remainroot_SOURCES += preload.c libremain.so.c

# I'm so sorry.
libremain.so.c: libremain/symbols.c core/cred.c
	$(CC) -I$(srcdir)/ -shared -fPIC libremain/symbols.c core/cred.c -o $(tmpdir)/libremain.so
	$(XXD) -i $(tmpdir)/libremain.so > libremain.so.c
