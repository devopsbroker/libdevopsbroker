/*
 * email.h - DevOpsBroker C header file for the org.devopsbroker.cgi.Email struct
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
 * echo ORG_DEVOPSBROKER_CGI_EMAIL | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_CGI_EMAIL_H
#define ORG_DEVOPSBROKER_CGI_EMAIL_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct Email {
	char *name;
	char *address;
	char *subject;
	char *message;
} Email;

static_assert(sizeof(Email) == 32, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a9740e57_initEmail
 * Description: Initializes an existing Email struct
 *
 * Parameters:
 *   email	    A pointer to the Email instance to initalize
 *   name       The name of the person
 *   address    The email address of the person
 *   subject    The email subject line
 *   message    The message of the email
 * ----------------------------------------------------------------------------
 */
void a9740e57_initEmail(Email *email, char *name, char *address, char *subject, char *message);

#endif /* ORG_DEVOPSBROKER_CGI_EMAIL_H */
