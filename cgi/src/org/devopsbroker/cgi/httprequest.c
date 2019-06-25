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

#include "org/devopsbroker/lang/error.h"
#include "org/devopsbroker/lang/integer.h"
#include "org/devopsbroker/lang/string.h"
#include "org/devopsbroker/lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

int a2465172_initHttpRequest(HttpRequest *request, uint32_t numParams) {
	int retValue = 0;
	request->method = getenv(HTTP_REQUEST_METHOD);

	c47905f7_initHashMap(&request->parameters, &f6215943_hashCode, &f6215943_isEqual, numParams);

	if (f6215943_isEqual(request->method, HTTP_REQUEST_METHOD_GET)) {
		request->queryString = getenv(HTTP_QUERY_STRING);

		if (request->queryString != NULL) {
			int strLen;
			a2465172_urldecode(request->queryString, &strLen);

			if (strLen > MAX_URL_SIZE) {
				retValue = SYSTEM_ERROR_CODE;
			} else {
				strLen++;
				request->paramStr = f668c4bd_malloc(strLen);

				f6215943_copyToBuffer(request->queryString, request->paramStr, strLen);
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
	int retValue = 0;

	if (request->length > 0) {
		int strLen;

		request->queryString = f668c4bd_malloc(request->length+1);
		if (fgets(request->queryString, request->length+1, stdin) == NULL) {
			retValue = SYSTEM_ERROR_CODE;
		} else {
			a2465172_urldecode(request->queryString, &strLen);

			request->paramStr = f668c4bd_malloc(strLen+1);

			f6215943_copyToBuffer(request->queryString, request->paramStr, strLen+1);
			retValue = a2465172_mapQueryString(request, request->paramStr);
		}
	}

	return retValue;
}
