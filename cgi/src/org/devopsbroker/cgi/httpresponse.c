/*
 * httpresponse.c - DevOpsBroker C source file for the org.devopsbroker.cgi.HttpResponse struct
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
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdio.h>

#include <org/devopsbroker/lang/stringbuilder.h>

#include "httpresponse.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void de3696d3_addCookie(HttpCookie *cookie) {
	StringBuilder strBuilder;
	c598a24c_initStringBuilder_uint32(&strBuilder, 128);

	c598a24c_append_string(&strBuilder, "Set-Cookie: ");

	// Set name and value
	c598a24c_append_string(&strBuilder, cookie->name);
	c598a24c_append_char(&strBuilder, '=');
	c598a24c_append_string(&strBuilder, cookie->value);

	// Set Expires or Max-Age
	if (cookie->maxAge > 0) {
		c598a24c_append_string(&strBuilder, "; Max-Age=");
		c598a24c_append_uint(&strBuilder, cookie->maxAge);
	} else if (cookie->expires != NULL) {
		c598a24c_append_string(&strBuilder, "; Expires=");
		c598a24c_append_string(&strBuilder, cookie->expires);
	}

	// Set Domain
	if (cookie->domain != NULL) {
		c598a24c_append_string(&strBuilder, "; Domain=");
		c598a24c_append_string(&strBuilder, cookie->domain);
	}

	// Set Path
	if (cookie->path != NULL) {
		c598a24c_append_string(&strBuilder, "; Path=");
		c598a24c_append_string(&strBuilder, cookie->path);
	}

	// Set SameSite
	if (cookie->sameSite != NULL) {
		c598a24c_append_string(&strBuilder, "; SameSite=");
		c598a24c_append_string(&strBuilder, cookie->sameSite);
	}

	// Set Secure
	if (cookie->isSecure) {
		c598a24c_append_string(&strBuilder, "; Secure");
	}

	// Set HttpOnly
	if (cookie->isHttpOnly) {
		c598a24c_append_string(&strBuilder, "; HttpOnly");
	}

	// Print Set-Cookie header
	puts(strBuilder.buffer);

	c598a24c_cleanUpStringBuilder(&strBuilder);
}

void de3696d3_closeResponseHeaders() {
	printf("\n");
}

void de3696d3_setCacheControl(HttpCacheControl *cacheControl) {
	StringBuilder strBuilder;
	c598a24c_initStringBuilder(&strBuilder);

	c598a24c_append_string(&strBuilder, "Cache-Control: ");

	// Figure out which response to send
	if (cacheControl->mustRevalidate) {
		c598a24c_append_string(&strBuilder, "must-revalidate");
	} else if (cacheControl->noCache) {
		c598a24c_append_string(&strBuilder, "no-cache");
	} else if (cacheControl->noStore) {
		c598a24c_append_string(&strBuilder, "no-store");
	} else if (cacheControl->noTransform) {
		c598a24c_append_string(&strBuilder, "no-transform");
	} else if (cacheControl->isPublic) {
		c598a24c_append_string(&strBuilder, "public");
	} else if (cacheControl->isPrivate) {
		c598a24c_append_string(&strBuilder, "private");
	} else if (cacheControl->proxyRevalidate) {
		c598a24c_append_string(&strBuilder, "proxy-revalidate");
	}

	// Set the maxAge or sharedMaxAge if necessary
	if (cacheControl->maxAge > 0) {
		c598a24c_append_string(&strBuilder, ", max-age=");
		c598a24c_append_uint(&strBuilder, cacheControl->maxAge);
	} else if (cacheControl->sharedMaxAge > 0) {
		c598a24c_append_string(&strBuilder, ", s-maxage=");
		c598a24c_append_uint(&strBuilder, cacheControl->sharedMaxAge);
	}

	// Print Cache-Control header
	puts(strBuilder.buffer);

	c598a24c_cleanUpStringBuilder(&strBuilder);
}

void de3696d3_setContentType(char *contentType) {
	printf("Content-type: %s\n", contentType);
}

void de3696d3_setHttpAuthenticate(char *authType, char *realm) {
	StringBuilder strBuilder;
	c598a24c_initStringBuilder_uint32(&strBuilder, 128);

	c598a24c_append_string(&strBuilder, "WWW-Authenticate: ");

	// Set authentication type
	c598a24c_append_string(&strBuilder, authType);

	// Set realm
	if (realm != NULL) {
		c598a24c_append_string(&strBuilder, " realm=\"");
		c598a24c_append_string(&strBuilder, realm);
		c598a24c_append_char(&strBuilder, '"');
	}

	// Print WWW-Authenticate header
	puts(strBuilder.buffer);

	c598a24c_cleanUpStringBuilder(&strBuilder);
}


void de3696d3_setRetryAfter(uint32_t numSeconds) {
	printf("Retry-After: %u\n", numSeconds);
}

void de3696d3_setStatusCode(char *statusCode) {
	printf("Status: %s\n", statusCode);
}
