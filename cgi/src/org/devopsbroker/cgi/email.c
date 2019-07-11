/*
 * email.c - DevOpsBroker C source file for the org.devopsbroker.cgi.Email struct
 *
 * Copyright (C) 2019 Edward Smith <edwardsmith@devopsbroker.org>
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
 * Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-21
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include "email.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static char *sanitizeInput(char *string);

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void a9740e57_initEmail(Email *email, char *name, char *address, char *subject, char *message) {
	email->name = sanitizeInput(name);
	email->address = sanitizeInput(address);
	email->subject = sanitizeInput(subject);
	email->message = sanitizeInput(message);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
 * Check for newlines so additional headers are not inserted into the message
 */
 static char *sanitizeInput(char *string) {
 	register char *start = string;
 	register char ch = (*string);

 	while (ch) {
 		if (ch == '\r' || ch == '\n') {
 			(*string) = '\0';
 			return start;
 		}

 		string++;
 		ch = (*string);
 	}

 	return start;
 }
