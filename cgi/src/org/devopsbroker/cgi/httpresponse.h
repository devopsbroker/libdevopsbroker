/*
 * httpresponse.h - DevOpsBroker C header file for the org.devopsbroker.cgi.HttpResponse struct
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
 * Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-25
 *
 * echo ORG_DEVOPSBROKER_CGI_HTTPRESPONSE | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_CGI_HTTPRESPONSE_H
#define ORG_DEVOPSBROKER_CGI_HTTPRESPONSE_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdbool.h>
#include <stdint.h>

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

// HTTP content type values
#define APPLICATION_JSON                 "application/json"
#define APPLICATION_XML                  "application/xml"
#define TEXT_CSV                         "text/csv"
#define TEXT_HTML                        "text/html"
#define TEXT_JAVASCRIPT                  "text/javascript"
#define TEXT_MARKDOWN                    "text/markdown"
#define TEXT_PLAIN                       "text/plain"
#define TEXT_XML                         "text/xml"

// HTTP response status codes
#define SC_CONTINUE                      "100 Continue"
#define SC_SWITCHING_PROTOCOLS           "101 Switching Protocols"
#define SC_EARLY_HINTS                   "103 Early Hints"
#define SC_OK                            "200 OK"
#define SC_CREATED                       "201 Created"
#define SC_ACCEPTED                      "202 Accepted"
#define SC_NON_AUTH_INFO                 "203 Non-Authoritative Information"
#define SC_NO_CONTENT                    "204 No Content"
#define SC_RESET_CONTENT                 "205 Reset Content"
#define SC_PARTIAL_CONTENT               "206 Partial Content"
#define SC_MULTIPLE_CHOICES              "300 Multiple Choices"
#define SC_MOVED_PERMANENTLY             "301 Moved Permanently"
#define SC_FOUND                         "302 Found"
#define SC_SEE_OTHER                     "303 See Other"
#define SC_NOT_MODIFIED                  "304 Not Modified"
#define SC_TEMPORARY_REDIRECT            "307 Temporary Redirect"
#define SC_PERMANENT_REDIRECT            "308 Permanent Redirect"
#define SC_BAD_REQUEST                   "400 Bad Request"
#define SC_UNAUTHORIZED                  "401 Unauthorized"
#define SC_FORBIDDEN                     "403 Forbidden"
#define SC_NOT_FOUND                     "404 Not Found"
#define SC_METHOD_NOT_ALLOWED            "405 Method Not Allowed"
#define SC_NOT_ACCEPTABLE                "406 Not Acceptable"
#define SC_PROXY_AUTH_REQUIRED           "407 Proxy Authentication Required"
#define SC_REQUEST_TIMEOUT               "408 Request Timeout"
#define SC_CONFLICT                      "409 Conflict"
#define SC_GONE                          "410 Gone"
#define SC_LENGTH_REQUIRED               "411 Length Required"
#define SC_PRECONDITION_FAILED           "412 Precondition Failed"
#define SC_PAYLOAD_TOO_LARGE             "413 Payload Too Large"
#define SC_URI_TOO_LONG                  "414 URI Too Long"
#define SC_UNSUPPORTED_MEDIA_TYPE        "415 Unsupported Media Type"
#define SC_RANGE_NOT_SATISFIABLE         "416 Range Not Satisfiable"
#define SC_EXPECTATION_FAILED            "417 Expectation Failed"
#define SC_TEAPOT                        "418 I'm a teapot"
#define SC_TOO_EARLY                     "425 Too Early"
#define SC_UPGRADE_REQUIRED              "426 Upgrade Required"
#define SC_PRECONDITION_REQUIRED         "428 Precondition Required"
#define SC_TOO_MANY_REQUESTS             "429 Too Many Requests"
#define SC_REQ_HEADER_FIELDS_TOO_LARGE   "431 Request Header Fields Too Large"
#define SC_UNAVAILABLE_FOR_LEGAL         "451 Unavailable For Legal Reasons"
#define SC_INTERNAL_SERVER_ERROR         "500 Internal Server Error"
#define SC_NOT_IMPLEMENTED               "501 Not Implemented"
#define SC_BAD_GATEWAY                   "502 Bad Gateway"
#define SC_SERVICE_UNAVAILABLE           "503 Service Unavailable"
#define SC_GATEWAY_TIMEOUT               "504 Gateway Timeout"
#define SC_HTTP_VERSION_NOT_SUPPORTED    "505 HTTP Version Not Supported"
#define SC_NETWORK_AUTH_REQUIRED         "511 Network Authentication Required"

// HTTP cookie SameSite values
#define SAME_SITE_STRICT                 "Strict"
#define SAME_SITE_LAX                    "Lax"
#define SAME_SITE_NONE                   "None"

// HTTP Authentication types
#define AUTH_BASIC                       "Basic"
#define AUTH_BEARER                      "Bearer"
#define AUTH_DIGEST                      "Digest"
#define AUTH_MUTUAL                      "Mutual"
#define AUTH_OAUTH                       "OAuth"

// ═════════════════════════════════ Typedefs ═════════════════════════════════

/*
 * Cookie Examples:
 *
 *   o Session cookie
 *     Set-Cookie: sessionid=38afes7a8; HttpOnly; Path=/
 *
 *   o Permanent cookie (set Expires or Max-Age)
 *     Set-Cookie: id=a3fWa; Expires=Wed, 21 Oct 2015 07:28:00 GMT; Secure; HttpOnly
 *
 *   o Cookie prefixes (__Secure- and __Host-)
 *     Set-Cookie: __Secure-ID=123; Secure; Domain=example.com (Secure mandatory, Domain optional)
 *     Set-Cookie: __Host-ID=123; Secure; Path=/               (Secure mandatory, Path=/ mandatory)
 */
typedef struct HttpCookie {
	char *name;                   // Cookie name
	char *value;                  // Cookie value
	char *expires;                // HTTP-date timestamp of maximum cookie lifetime
	char *domain;                 // The hosts to which the cookie will be sent
	char *path;                   // URL path that must exist before sending Cookie header
	char *sameSite;               // [ Strict, Lax, None ] Mitigate CSRF attacks
	uint32_t maxAge;              // Number of seconds until the cookie expires
	bool isSecure;                // Only send Cookie when using SSL over HTTPS protocol
	bool isHttpOnly;              // Not accessible to JavaScript (mitigate XSS)
} HttpCookie;

static_assert(sizeof(HttpCookie) == 56, "Check your assumptions");

/*
 * Cache-Control Examples:
 *
 *   o Prevent caching
 *     Cache-Control: no-store
 *
 *   o Cache static assets
 *     Cache-Control: public, max-age=7257600
 */
typedef struct HttpCacheControl {
	uint32_t maxAge;              // Number of seconds until the cache expires
	uint32_t sharedMaxAge;        // Only applies to shared caches (e.g. proxies)
	bool mustRevalidate;          // Revalidate once a resource has become stale (e.g. max-age has expired)
	bool noCache;                 // Forces caches to submit the request to the origin server for validation before releasing a cached copy
	bool noStore;                 // The cache should not store anything about the client request or server response
	bool noTransform;             // No transformations or conversions should be made to the resource
	bool isPublic;                // The response may be cached by any cache
	bool isPrivate;               // The response is intended for a single user and must not be stored by a shared cache
	bool proxyRevalidate;         // Same as must-revalidate, but it only applies to shared caches (e.g. proxies)
} HttpCacheControl;

static_assert(sizeof(HttpCacheControl) == 16, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    de3696d3_addCookie
 * Description: Adds the specified HTTP cookie to the response header
 *
 * Parameters:
 *   cookie     The specified HTTP cookie to add
 * ----------------------------------------------------------------------------
 */
void de3696d3_addCookie(HttpCookie *cookie);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    de3696d3_closeResponseHeaders
 * Description: Closes the HTTP response header section
 * ----------------------------------------------------------------------------
 */
void de3696d3_closeResponseHeaders();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    de3696d3_setCacheControl
 * Description: Sets the cache control HTTP response header value
 *
 * Parameters:
 *   cacheControl   The cache control to set on the HTTP response header
 * ----------------------------------------------------------------------------
 */
void de3696d3_setCacheControl(HttpCacheControl *cacheControl);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    de3696d3_setContentType
 * Description: Sets the content type HTTP response header value
 *
 * Parameters:
 *   contentType    The content type to set on the HTTP response header
 * ----------------------------------------------------------------------------
 */
void de3696d3_setContentType(char *contentType);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    de3696d3_setHttpAuthenticate
 * Description: Specify the authentication method required to gain access to a resource
 *
 * Parameters:
 *   authType   The authenticate type
 *   realm      Description of the protected area
 * ----------------------------------------------------------------------------
 */
void de3696d3_setHttpAuthenticate(char *authType, char *realm);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    de3696d3_setRetryAfter
 * Description: Sets the retry after HTTP response header value. This header is
 *              used for the following HTTP status codes:
 *                o 301 Moved Permanently
 *                o 429 Too Many Requests
 *                o 503 Service Unavailable
 *
 * Parameters:
 *   numSeconds     The number of seconds for the client to wait before retry
 * ----------------------------------------------------------------------------
 */
void de3696d3_setRetryAfter(uint32_t numSeconds);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    de3696d3_setStatusCode
 * Description: Sets the status code HTTP response header value
 *
 * Parameters:
 *   statusCode     The status code to set on the HTTP response header
 * ----------------------------------------------------------------------------
 */
void de3696d3_setStatusCode(char *statusCode);

#endif /* ORG_DEVOPSBROKER_CGI_HTTPRESPONSE_H */
