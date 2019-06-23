#
# test.mk - DevOpsBroker makefile for unit testing libdevopsbroker-cgi.a functionality
#
# Copyright (C) 2019 Edward Smith <edwardsmith@devopsbroker.org>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#
# -----------------------------------------------------------------------------
# Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-21
#
# -----------------------------------------------------------------------------
#

################################### Includes ##################################

include /etc/devops/globals.mk

################################## Variables ##################################

CC := /usr/bin/gcc
CFLAGS := -Wall -gdwarf -m64 -Og -fdiagnostics-color=always -DDEBUG
LDFLAGS := -m64

SRC_DIR := test/org/devopsbroker
LIB_DIRS := -Llib/ -L$(realpath $(CURDIR)/../C/lib/)
LIB_NAMES := -ldevopsbroker-cgi -ldevopsbroker

INCLUDE_DIRS := -I$(CURDIR)/src -I$(realpath $(CURDIR)/../C/src)

C_SOURCES := $(shell /usr/bin/find $(SRC_DIR) -type f -name "*.c")
C_OUTPUTS := $(C_SOURCES:.c=.a)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Exports ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

export CC
export CFLAGS

################################### Targets ###################################

.ONESHELL:
.PHONY: default clean prepare printenv test

default: test

clean:
	$(call printInfo,Cleaning $(SRC_DIR)/cgi directory)
	/bin/rm -fv $(SRC_DIR)/cgi/*.a

prepare:
	/bin/cp -uv $(realpath $(CURDIR)/../C/lib/)/libdevopsbroker.a lib/

# Obtain executable files for the C unit tests
$(SRC_DIR)/cgi/%.a: $(SRC_DIR)/cgi/%.c | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) $< $(INCLUDE_DIRS) $(LIB_DIRS) $(LIB_NAMES) -o $@

# Execute C unit tests
test : $(C_OUTPUTS)
	$(call printInfo,Testing libdevopsbroker-cgi.a static library)
	for unitTest in $(C_OUTPUTS); do \
		$(call printInfo,Calling $$unitTest)
		$$unitTest; \
	done

printenv:
	echo "  MAKEFILE_LIST: $(MAKEFILE_LIST)"
	echo "   MAKECMDGOALS: $(MAKECMDGOALS)"
	echo "          DEBUG: $(DEBUG)"
	echo "         TMPDIR: $(TMPDIR)"
	echo "         CURDIR: $(CURDIR)"
	echo "             CC: $(CC)"
	echo "         CFLAGS: $(CFLAGS)"
	echo "   INCLUDE_DIRS: $(INCLUDE_DIRS)"
	echo "       LIB_DIRS: $(LIB_DIRS)"
	echo "      LIB_NAMES: $(LIB_NAMES)"
	echo "      C_SOURCES: $(C_SOURCES)"
	echo "      C_OUTPUTS: $(C_OUTPUTS)"
	echo
