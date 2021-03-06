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
#
# Please set the following environment variables:
#    o OSTYPE (win64, linux)
#    o MACHTYPE (x86_64, armv7l)
# -----------------------------------------------------------------------------
#

################################### Includes ##################################

include /etc/devops/globals.mk

################################## Variables ##################################

ASM := /usr/bin/nasm
CC := /usr/bin/gcc

ifeq ($(DEBUG), 1)
    ifeq ($(MACHTYPE), 'x86_64')
        ASMFLAGS := -felf64 -gdwarf
        CFLAGS := -Wall -gdwarf -m64 -Og -fdiagnostics-color=always -DDEBUG -DMACHTYPE=$(MACHTYPE)
    else
        ASMFLAGS := -gdwarf
        CFLAGS := -Wall -gdwarf -Og -fdiagnostics-color=always -DDEBUG -DMACHTYPE=$(MACHTYPE)
    endif
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

ASM_SOURCES := $(shell /usr/bin/find $(SRC_DIR) -type f -name "*.$(OSTYPE)-$(MACHTYPE).asm")
ASM_OBJECTS := $(subst src/,obj/,$(ASM_SOURCES:.asm=.o))

EXEC_CHMOD := $(shell which chmod)
EXEC_CP :=  $(shell which cp)
EXEC_MKDIR := $(shell which mkdir)
EXEC_RM := $(shell which rm)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Exports ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

export ASM
export ASMFLAGS
export CC
export CFLAGS

################################### Targets ###################################

.ONESHELL:
.SILENT:
.PHONY: default clean prepare printenv

default: $(C_OBJECTS) $(ASM_OBJECTS)

clean:
	$(call printInfo,Cleaning $(OBJ_DIR) directory)
	$(EXEC_RM) -rfv $(OBJ_DIR)
	$(call printInfo,Cleaning $(LIB_DIR))
	$(EXEC_RM) -fv lib/$(LIB_NAME)

prepare:
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/adt
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/compress
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/fs
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/hash
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/info
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/io
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/lang
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/log
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/memory
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/net
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/socket
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/sysfs
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/system
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/terminal
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/test
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/text
	$(EXEC_MKDIR) --parents --mode=750 $(OBJ_DIR)/time
	$(EXEC_MKDIR) --parents --mode=750 $(LIB_DIR)

# Obtain object files for the C utilities
$(OBJ_DIR)/adt/%.o: $(SRC_DIR)/adt/%.c $(SRC_DIR)/adt/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/compress/%.o: $(SRC_DIR)/compress/%.c $(SRC_DIR)/compress/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/fs/%.o: $(SRC_DIR)/fs/%.c $(SRC_DIR)/fs/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/hash/%.o: $(SRC_DIR)/hash/%.c $(SRC_DIR)/hash/%.h | prepare
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

$(OBJ_DIR)/memory/%.o: $(SRC_DIR)/memory/%.c $(SRC_DIR)/memory/%.h | prepare
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

$(OBJ_DIR)/system/%.o: $(SRC_DIR)/system/%.c $(SRC_DIR)/system/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/terminal/%.o: $(SRC_DIR)/terminal/%.c $(SRC_DIR)/terminal/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test/%.o: $(SRC_DIR)/test/%.c $(SRC_DIR)/test/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/text/%.o: $(SRC_DIR)/text/%.c $(SRC_DIR)/text/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/time/%.o: $(SRC_DIR)/time/%.c $(SRC_DIR)/time/%.h | prepare
	$(call printInfo,Compiling $(@F))
	$(CC) $(CFLAGS) -c $< -o $@

# Obtain object files for the ASM libraries
$(OBJ_DIR)/adt/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/adt/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/compress/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/compress/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/fs/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/fs/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/hash/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/hash/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/info/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/info/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/io/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/io/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/lang/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/lang/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/log/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/log/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/memory/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/memory/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/net/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/net/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/socket/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/socket/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/sysfs/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/sysfs/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/system/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/system/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/terminal/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/terminal/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/test/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/test/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/text/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/text/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJ_DIR)/time/%.$(OSTYPE)-$(MACHTYPE).o: $(SRC_DIR)/time/%.$(OSTYPE)-$(MACHTYPE).asm | prepare
	$(call printInfo,Compiling $(@F))
	$(ASM) $(ASMFLAGS) $< -o $@

lib/libdevopsbroker.a: $(C_OBJECTS) $(ASM_OBJECTS)
	$(call printInfo,Building libdevopsbroker.a static library)
	$(EXEC_RM) -f lib/libdevopsbroker.a
	/usr/bin/ar cr lib/libdevopsbroker.a $(C_OBJECTS) $(ASM_OBJECTS)
	/usr/bin/ranlib lib/libdevopsbroker.a

deploy: lib/libdevopsbroker.a
	$(call printInfo,Deploying libdevopsbroker C header files)
	$(foreach headerFile, \
		$(wildcard $(SRC_DIR)/**/*.h), \
		dirName=$(subst src/,,$(dir $(headerFile))); \
		/usr/bin/sudo $(EXEC_MKDIR) -v --parents /usr/include/$${dirName}; \
		/usr/bin/sudo $(EXEC_CP) -uv $(headerFile) /usr/include/$${dirName}; \
	)
	/usr/bin/sudo /usr/bin/find /usr/include/org -type d -exec $(EXEC_CHMOD) -c 755 {} \;
	/usr/bin/sudo /usr/bin/find /usr/include/org -type f -exec $(EXEC_CHMOD) -c 644 {} \;
	$(call printInfo,Deploying libdevopsbroker.a static library)
	libName=$(subst lib/,,$<);
	/usr/bin/sudo $(EXEC_CP) -uv $< /usr/lib/
	/usr/bin/sudo $(EXEC_CHMOD) -c 644 /usr/lib/$${libName}

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
