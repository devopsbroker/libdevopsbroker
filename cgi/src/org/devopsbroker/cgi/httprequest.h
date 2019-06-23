/*
 * httprequest.h - DevOpsBroker C header file for the org.devopsbroker.cgi.HttpRequest struct
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
 * echo ORG_DEVOPSBROKER_CGI_HTTPREQUEST | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_CGI_HTTPREQUEST_H
#define ORG_DEVOPSBROKER_CGI_HTTPREQUEST_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <assert.h>

#include "org/devopsbroker/adt/hashmap.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

// HTTP Request Methods
#define HTTP_REQUEST_METHOD_GET   "GET"
#define HTTP_REQUEST_METHOD_POST  "POST"

// HTTP Request Environment Variable Names
#define HTTP_AUTH_TYPE           "AUTH_TYPE"
#define HTTP_CONTENT_LENGTH      "CONTENT_LENGTH"
#define HTTP_CONTENT_TYPE        "CONTENT_TYPE"
#define HTTP_GATEWAY_INTERFACE   "GATEWAY_INTERFACE"
#define HTTP_ACCEPT              "HTTP_ACCEPT"
#define HTTP_USER_AGENT          "HTTP_USER_AGENT"
#define HTTP_PATH_INFO           "PATH_INFO"
#define HTTP_PATH_TRANSLATED     "PATH_TRANSLATED"
#define HTTP_QUERY_STRING        "QUERY_STRING"
#define HTTP_REMOTE_ADDR         "REMOTE_ADDR"
#define HTTP_REMOTE_HOST         "REMOTE_HOST"
#define HTTP_REMOTE_IDENT        "REMOTE_IDENT"
#define HTTP_REMOTE_USER         "REMOTE_USER"
#define HTTP_REQUEST_METHOD      "REQUEST_METHOD"
#define HTTP_SCRIPT_NAME         "SCRIPT_NAME"
#define HTTP_SERVER_NAME         "SERVER_NAME"
#define HTTP_SERVER_PORT         "SERVER_PORT"
#define HTTP_SERVER_PROTOCOL     "SERVER_PROTOCOL"
#define HTTP_SERVER_SOFTWARE     "SERVER_SOFTWARE"

// HTTP Limits
#define MAX_URL_SIZE             131072

// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct HttpRequest {
	HashMap parameters;           // GET / POST parameter keys and values
	char *method;                 // GET or POST
	char *queryString;            // GET form submission parameters
	char *paramStr;               // Parameter string (needs to be free())
	int length;                   // CONTENT_LENGTH from POST
} HttpRequest;

static_assert(sizeof(HttpRequest) == 72, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a2465172_initHttpRequest
 * Description: Initializes an existing HttpRequest struct
 *
 * Parameters:
 *   request    A pointer to the HttpRequest instance to initialize
 *   numParams  The expected number of parameters to be present on the request
 * Returns:     Zero if no issues with data conversion found, SYSTEM_ERROR_CODE otherwise
 * ----------------------------------------------------------------------------
 */
int a2465172_initHttpRequest(HttpRequest *request, uint32_t numParams);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a2465172_cleanUpHttpRequest
 * Description: Frees the memory allocated to the data pointer on a POST
 *
 * Parameters:
 *   request    A pointer to the HttpRequest instance to cleanup
 * ----------------------------------------------------------------------------
 */
void a2465172_cleanUpHttpRequest(HttpRequest *request);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a2465172_getParameter
 * Description: Returns the parameter value associated with the given parameter name
 *
 * Parameters:
 *   request    A pointer to the HttpRequest instance
 *   name       The name of the parameter to retrieve
 * Returns:     The parameter value associate with the parameter name, or NULL if not found
 * ----------------------------------------------------------------------------
 */
char *a2465172_getParameter(HttpRequest *request, char *name);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a2465172_mapQueryString
 * Description: Maps the query string from a GET form submission to the parameterMap
 *
 * Parameters:
 *   request    A pointer to the HttpRequest instance
 *   paramStr   The parameter string to map
 * Returns:     Zero if no issues with data conversion found, SYSTEM_ERROR_CODE otherwise
 * ----------------------------------------------------------------------------
 */
int a2465172_mapQueryString(HttpRequest *request, char *paramStr);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a2465172_readPostData
 * Description: Initializes an existing Http struct with POST form submission data
 *
 * Parameters:
 *   request    A pointer to the Http instance to initalize
 *   maxLen     The maximum length to accept for the POST form submission
 * ----------------------------------------------------------------------------
 */
void a2465172_readPostData(HttpRequest *request, int maxLen);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a2465172_urldecode
 * Description: Decodes the query string from a GET form submission
 *
 * Parameters:
 *   paramStr   The GET form submission data to decode
 *   strLen     Populated with the length of the URL decoded string
 * Returns:     The URL decoded string
 * ----------------------------------------------------------------------------
 */
char *a2465172_urldecode(char *paramStr, int *strLen);

#endif /* ORG_DEVOPSBROKER_CGI_HTTPREQUEST_H */
