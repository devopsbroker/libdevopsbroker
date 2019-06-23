#
# makefile - DevOpsBroker makefile for generating the libdevopsbroker-cgi.a static library
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
# NOTE: Deterministic archives do not store file timestamps, so the /usr/bin/ar
# 'u' option to only replace newer files cannot work
# -----------------------------------------------------------------------------
#

################################### Includes ##################################

include /etc/devops/globals.mk

################################## Variables ##################################

ASM := /usr/bin/nasm
CC := /usr/bin/gcc

ifeq ($(DEBUG), 1)
    ASMFLAGS := -felf64 -gdwarf
    CFLAGS := -Wall -gdwarf -m64 -Og -fdiagnostics-color=always -DDEBUG
else
    ASMFLAGS := -felf64
    CFLAGS := -Wall -m64 -O2 -fdiagnostics-color=always -DNDEBUG
endif

SRC_DIR := src/org/devopsbroker
OBJ_DIR := obj/org/devopsbroker

INCLUDE_DIRS := -I$(CURDIR)/src -I$(realpath $(CURDIR)/../C/src)

C_SOURCES := $(shell /usr/bin/find $(SRC_DIR) -type f -name "*.c")
C_OBJECTS := $(subst src/,obj/,$(C_SOURCES:.c=.o))

ASM_SOURCES := $(shell /usr/bin/find $(SRC_DIR) -type f -name "*.asm")
ASM_OBJECTS := $(subst src/,obj/,$(ASM_SOURCES:.asm=.o))

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Exports ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

export ASM
export ASMFLAGS
export CC
export CFLAGS

################################### Targets ###################################

.ONESHELL:
.PHONY: default clean prepare printenv

default: lib/libdevopsbroker-cgi.a

clean:
	$(call printInfo,Cleaning $(OBJ_DIR) directory)
	/bin/rm -rfv $(OBJ_DIR)
	$(call printInfo,Cleaning lib/libdevopsbroker-cgi.a)
	/bin/rm -fv lib/libdevopsbroker-cgi.a

prepare:
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/cgi
	/bin/mkdir --parents --mode=750 lib/

# Obtain object files for the C utilities
$(OBJ_DIR)/cgi/%.o: $(SRC_DIR)/cgi/%.c $(SRC_DIR)/cgi/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< $(INCLUDE_DIRS) -o $@

# Obtain object files for the ASM libraries
$(OBJ_DIR)/cgi/%.o: $(SRC_DIR)/cgi/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

lib/libdevopsbroker-cgi.a: $(C_OBJECTS) $(ASM_OBJECTS)
	$(call printInfo,Building libdevopsbroker-cgi.a static library)
	/bin/rm -f lib/libdevopsbroker-cgi.a
	/usr/bin/ar cr lib/libdevopsbroker-cgi.a $(C_OBJECTS) $(ASM_OBJECTS)
	/usr/bin/ranlib lib/libdevopsbroker-cgi.a

printenv:
	echo "  MAKEFILE_LIST: $(MAKEFILE_LIST)"
	echo "   MAKECMDGOALS: $(MAKECMDGOALS)"
	echo "          DEBUG: $(DEBUG)"
	echo "         TMPDIR: $(TMPDIR)"
	echo "         CURDIR: $(CURDIR)"
	echo "            ASM: $(ASM)"
	echo "       ASMFLAGS: $(ASMFLAGS)"
	echo "             CC: $(CC)"
	echo "         CFLAGS: $(CFLAGS)"
	echo "   INCLUDE_DIRS: $(INCLUDE_DIRS)"
	echo "      C_OBJECTS: $(C_OBJECTS)"
	echo "    ASM_OBJECTS: $(ASM_OBJECTS)"
	echo
