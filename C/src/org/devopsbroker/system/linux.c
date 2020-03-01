/*
 * linux.c - DevOpsBroker C source file for procfs and sysfs Linux system information
 *
 * Copyright (C) 2020 Edward Smith <edwardsmith@devopsbroker.org>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------------
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-40
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>
#include <string.h>

#include "linux.h"

#include "../lang/error.h"
#include "../lang/integer.h"
#include "../lang/stringbuilder.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

void e0004944_printError(char *infoPath, int errorNum) {
	StringBuilder strBuilder;
	char *errorMessage;

	if (errorNum == ENOENT) {
		errorMessage = "no such file or directory for path " ANSI_WHITE "'";
	} else if (errorNum == EBADF || errorNum == EACCES) {
		errorMessage = "permission denied on path " ANSI_WHITE "'";
	} else {
		errorMessage = f45efac2_toString_int32(errorNum);
	}

	c598a24c_initStringBuilder(&strBuilder);
	c598a24c_append_string(&strBuilder, errorMessage);
	c598a24c_append_string(&strBuilder, infoPath);
	c598a24c_append_char(&strBuilder, '\'' );

	fprintf(stderr, ANSI_BOLD "%s: " ANSI_RED "%s\n" ANSI_RESET, programName, strBuilder.buffer);

	c598a24c_cleanUpStringBuilder(&strBuilder);
}
