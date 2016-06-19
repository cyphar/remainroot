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
#

CC=gcc
LIBRARY=libremain.so

# All headers.
HEADERS=$(wildcard src/*.h) $(wildcard src/*/*.h)

# Core code.
CSOURCES=$(wildcard src/core/*.c)
COBJECTS=$(CSOURCES:.c=.o)

# libremain.so code.
LSOURCES=$(wildcard src/preload/*.c)
LOBJECTS=$(LSOURCES:.c=.o)

.PHONY: all clean

all: $(LIBRARY)

clean:
	@echo " [CLEAN] ."
	@rm -f $(COBJECTS) $(LOBJECTS) $(LIBRARY)

%.o: %.c $(HEADERS)
	@echo "    [CC] $<"
	@$(CC) -shared -fPIC -Isrc/ $(CFLAGS) -c $< -o $@

$(LIBRARY): $(COBJECTS) $(LOBJECTS)
	@echo "  [LINK] $@"
	@$(CC) -fPIC -shared -Wl,-soname,$@ $(COBJECTS) $(LOBJECTS) -o $@
