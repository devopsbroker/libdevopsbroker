/*
 * httprequest.c - DevOpsBroker C source file for the org.devopsbroker.cgi.HttpRequest struct
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

#include <stdlib.h>
#include <stdio.h>

#include "httprequest.h"

#include <org/devopsbroker/lang/error.h>
#include <org/devopsbroker/lang/integer.h>
#include <org/devopsbroker/lang/memory.h>
#include <org/devopsbroker/lang/string.h>
#include <org/devopsbroker/lang/stringbuilder.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static char *sanitizeInput(char *string);

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

int a2465172_initHttpRequest(HttpRequest *request, uint32_t numParams) {
	int retValue = 0;
	request->method = getenv(HTTP_REQUEST_METHOD);

	c47905f7_initHashMap(&request->parameters, &f6215943_hashCode, &f6215943_isEqual, numParams);

	if (f6215943_isEqual(request->method, HTTP_REQUEST_METHOD_GET)) {
		request->queryString = getenv(HTTP_QUERY_STRING);

		if (request->queryString != NULL) {
			request->length = f6215943_getLength(request->queryString);

			if (request->length > MAX_URL_SIZE) {
				retValue = SYSTEM_ERROR_CODE;
			} else {
				request->paramStr = f668c4bd_stralloc(request->length);
				a2465172_urldecode(request->queryString, request->paramStr);

				retValue = a2465172_mapQueryString(request, request->paramStr);
			}
		}

	} else if (f6215943_isEqual(request->method, HTTP_REQUEST_METHOD_POST)) {
		request->queryString = NULL;
		request->paramStr = NULL;
		retValue = f45efac2_parseInt(getenv(HTTP_CONTENT_LENGTH), &request->length);
	}

	return retValue;
}

void a2465172_cleanUpHttpRequest(HttpRequest *request) {
	c47905f7_cleanUpHashMap(&request->parameters);

	// Free the queryString if POST
	if (f6215943_isEqual(request->method, HTTP_REQUEST_METHOD_POST) && request->queryString != NULL) {
		free(request->queryString);
	}

	if (request->paramStr != NULL) {
		free(request->paramStr);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char *a2465172_getString(HttpRequest *request, char *name, uint32_t maxLen) {
	char *value = c47905f7_get(&request->parameters, name);

	return f6215943_truncate(value, maxLen);
}

int a2465172_mapPostData(HttpRequest *request) {
	StringBuilder strBuilder;
	int retValue = 0;

	if (request->length > 0) {
		c598a24c_initStringBuilder_uint32(&strBuilder, request->length);
		request->queryString = f668c4bd_stralloc(request->length);

		while (fgets(request->queryString, request->length+1, stdin) != NULL) {
			if (strBuilder.length > 0) {
				c598a24c_append_char(&strBuilder, '&');
			}

			c598a24c_append_string(&strBuilder, sanitizeInput(request->queryString));
		}

		f6215943_copyToBuffer(strBuilder.buffer, request->queryString, strBuilder.length+1);

		request->paramStr = f668c4bd_stralloc(strBuilder.length);
		a2465172_urldecode(strBuilder.buffer, request->paramStr);

		retValue = a2465172_mapQueryString(request, request->paramStr);
		c598a24c_cleanUpStringBuilder(&strBuilder);
	}

	return retValue;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
