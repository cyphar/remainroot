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

HEADERS=$(wildcard src/*.h)
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)

LSOURCES=$(wildcard src/preload/*.c)
LOBJECTS=$(LSOURCES:.c=.o)

.PHONY: all clean

all: $(SOURCES) $(LIBRARY)

clean:
	@echo " [CLEAN] ."
	@rm -f $(OBJECTS) $(LOBJECTS) $(LIBRARY)

%.o: %.c $(HEADERS)
	@echo "    [CC] $<"
	@$(CC) -shared -fPIC $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJECTS) $(LOBJECTS)
	@echo "  [LINK] $@"
	@$(CC) -shared $(OBJECTS) $(LOBJECTS) -o $@
