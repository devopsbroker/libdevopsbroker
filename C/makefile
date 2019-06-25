#
# makefile - DevOpsBroker makefile for generating the libdevopsbroker.a static library
#
# Copyright (C) 2018-2019 Edward Smith <edwardsmith@devopsbroker.org>
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
# Developed on Ubuntu 18.04.1 LTS running kernel.osrelease = 4.15.0-39
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
LIB_DIR := lib
LIB_NAME := libdevopsbroker.a

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

default: lib/$(LIB_NAME)

clean:
	$(call printInfo,Cleaning $(OBJ_DIR) directory)
	/bin/rm -rfv $(OBJ_DIR)
	$(call printInfo,Cleaning $(LIB_DIR))
	/bin/rm -fv lib/$(LIB_NAME)

prepare:
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/adt
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/info
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/io
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/lang
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/log
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/net
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/socket
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/sysfs
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/terminal
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/text
	/bin/mkdir --parents --mode=750 $(OBJ_DIR)/time
	/bin/mkdir --parents --mode=750 $(LIB_DIR)

# Obtain object files for the C utilities
$(OBJ_DIR)/adt/%.o: $(SRC_DIR)/adt/%.c $(SRC_DIR)/adt/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/info/%.o: $(SRC_DIR)/info/%.c $(SRC_DIR)/info/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/io/%.o: $(SRC_DIR)/io/%.c $(SRC_DIR)/io/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/lang/%.o: $(SRC_DIR)/lang/%.c $(SRC_DIR)/lang/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/log/%.o: $(SRC_DIR)/log/%.c $(SRC_DIR)/log/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/net/%.o: $(SRC_DIR)/net/%.c $(SRC_DIR)/net/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/socket/%.o: $(SRC_DIR)/socket/%.c $(SRC_DIR)/socket/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/sysfs/%.o: $(SRC_DIR)/sysfs/%.c $(SRC_DIR)/sysfs/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/terminal/%.o: $(SRC_DIR)/terminal/%.c $(SRC_DIR)/terminal/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/text/%.o: $(SRC_DIR)/text/%.c $(SRC_DIR)/text/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/time/%.o: $(SRC_DIR)/time/%.c $(SRC_DIR)/time/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

# Obtain object files for the ASM libraries
$(OBJ_DIR)/adt/%.o: $(SRC_DIR)/adt/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/info/%.o: $(SRC_DIR)/info/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/io/%.o: $(SRC_DIR)/io/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/lang/%.o: $(SRC_DIR)/lang/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/log/%.o: $(SRC_DIR)/log/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/net/%.o: $(SRC_DIR)/net/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/socket/%.o: $(SRC_DIR)/socket/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/sysfs/%.o: $(SRC_DIR)/sysfs/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/terminal/%.o: $(SRC_DIR)/terminal/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/text/%.o: $(SRC_DIR)/text/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/time/%.o: $(SRC_DIR)/time/%.asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

lib/libdevopsbroker.a: $(C_OBJECTS) $(ASM_OBJECTS)
	$(call printInfo,Building libdevopsbroker.a static library)
	/bin/rm -f lib/libdevopsbroker.a
	/usr/bin/ar cr lib/libdevopsbroker.a $(C_OBJECTS) $(ASM_OBJECTS)
	/usr/bin/ranlib lib/libdevopsbroker.a

deploy: lib/libdevopsbroker.a
	$(call printInfo,Deploying libdevopsbroker C header files)
	$(foreach headerFile, \
		$(wildcard $(SRC_DIR)/**/*.h), \
		dirName=$(subst src/,,$(dir $(headerFile))); \
		sudo mkdir -v --parents /usr/include/$${dirName}; \
		sudo cp -uv $(headerFile) /usr/include/$${dirName}; \
	)
	sudo find /usr/include -type d -exec chmod -c 755 {} \;
	sudo find /usr/include -type f -exec chmod -c 644 {} \;
	$(call printInfo,Deploying libdevopsbroker.a static library)
	libName=$(subst lib/,,$<);
	sudo cp -uv $< /usr/lib/
	sudo chmod -c 644 /usr/lib/$${libName}

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
	echo "       LIB_NAME: $(LIB_NAME)"
	echo "      C_OBJECTS: $(C_OBJECTS)"
	echo "    ASM_OBJECTS: $(ASM_OBJECTS)"
	echo
