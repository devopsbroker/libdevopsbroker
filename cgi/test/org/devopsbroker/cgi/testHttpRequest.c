/*
 * testHttpRequest.c - DevOpsBroker C source file for testing org/devopsbroker/cgi/httprequest.h
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
 *
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
#include <stdbool.h>

#include "org/devopsbroker/cgi/httprequest.h"
#include "org/devopsbroker/io/stream.h"
#include "org/devopsbroker/lang/string.h"
#include "org/devopsbroker/terminal/ansi.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define PASS "[" ANSI_BOLD ANSI_GREEN "pass" ANSI_RESET "]\n"
#define FAIL "[" ANSI_BOLD ANSI_RED "fail" ANSI_RESET "]\n"

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════

HttpRequest request;

// ════════════════════════════ Function Prototypes ═══════════════════════════

static void positiveTestChar(char *label, char *expected, char *actual);
static void positiveTestInt(char *label, int expected, int actual);

static void setupGetTesting(HttpRequest *request);
static void tearDownGetTesting(HttpRequest *request);

static void setupPostTesting(HttpRequest *request);
static void tearDownPostTesting(HttpRequest *request);

static void testUrlDecode();
static void testParameters(HttpRequest *request);

// ══════════════════════════════════ main() ══════════════════════════════════

int main(int argc, char *argv[]) {
	setupGetTesting(&request);
	testParameters(&request);
	tearDownGetTesting(&request);

	setupPostTesting(&request);
	testParameters(&request);
	tearDownPostTesting(&request);

	testUrlDecode();

	// Exit with success
	exit(EXIT_SUCCESS);
}

// ═════════════════════════ Function Implementations ═════════════════════════

static void positiveTestChar(char *label, char *expected, char *actual) {
	printf("%s", label);

	if (f6215943_isEqual(expected, actual)) {
		printf(PASS);
	} else {
		printf(FAIL);
	}
}

static void positiveTestInt(char *label, int expected, int actual) {
	printf("%s", label);

	if (expected == actual) {
		printf(PASS);
	} else {
		printf(FAIL);
	}
}

static void setupGetTesting(HttpRequest *request) {
	puts("setupGetTesting:");
	setenv(HTTP_REQUEST_METHOD, HTTP_REQUEST_METHOD_GET, true);
	setenv(HTTP_QUERY_STRING, "foo=bar&bar=baz&baz=XYZ", true);
	a2465172_initHttpRequest(request, 3);

	positiveTestInt("  HashMap capacity = 8\t\t\t\t", 8, request->parameters.capacity);
	positiveTestInt("  HashMap size = 3\t\t\t\t", 3, request->parameters.size);
	positiveTestInt("  HashMap length = 11\t\t\t\t", 11, request->parameters.length);

	printf("\n");
}

static void tearDownGetTesting(HttpRequest *request) {
	puts("tearDownGetTesting:");
	a2465172_cleanUpHttpRequest(request);

	positiveTestInt("  HashMap capacity = 8\t\t\t\t", 8, request->parameters.capacity);
	positiveTestInt("  HashMap size = 0\t\t\t\t", 0, request->parameters.size);
	positiveTestInt("  HashMap length = 11\t\t\t\t", 11, request->parameters.length);

	printf("\n");
}

static void setupPostTesting(HttpRequest *request) {
	puts("setupPostTesting:");
	setenv(HTTP_REQUEST_METHOD, HTTP_REQUEST_METHOD_POST, true);
	setenv(HTTP_CONTENT_LENGTH, "23", true);
	a2465172_initHttpRequest(request, 3);

	// Populate STDIN with the HTTP POST data
	a8d15ebe_ungetString("foo=bar&bar=baz&baz=XYZ", stdin);
	a2465172_mapPostData(request);

	positiveTestInt("  HashMap capacity = 8\t\t\t\t", 8, request->parameters.capacity);
	positiveTestInt("  HashMap size = 3\t\t\t\t", 3, request->parameters.size);
	positiveTestInt("  HashMap length = 11\t\t\t\t", 11, request->parameters.length);

	printf("\n");
}

static void tearDownPostTesting(HttpRequest *request) {
	puts("tearDownPostTesting:");
	a2465172_cleanUpHttpRequest(request);

	positiveTestInt("  HashMap capacity = 8\t\t\t\t", 8, request->parameters.capacity);
	positiveTestInt("  HashMap size = 0\t\t\t\t", 0, request->parameters.size);
	positiveTestInt("  HashMap length = 11\t\t\t\t", 11, request->parameters.length);

	printf("\n");
}

static void testUrlDecode() {
	char queryString1[] = "foo=bar&bar=baz&baz=XYZ";
	char *urlDecodedStr = NULL;

	urlDecodedStr = f668c4bd_malloc(24);

	puts("testUrlDecode:");
	a2465172_urldecode(queryString1, urlDecodedStr);
	positiveTestChar("  a2465172_urldecode(\"foo=bar&bar=baz&baz=XYZ\")\t\t", queryString1, urlDecodedStr);

	char queryString2[] = "foo=bar%21";
	a2465172_urldecode(queryString2, urlDecodedStr);
	positiveTestChar("  a2465172_urldecode(\"foo=bar%21\")\t\t\t", urlDecodedStr, "foo=bar!");

	char queryString3[] = "foo=ba%21";
	a2465172_urldecode(queryString3, urlDecodedStr);
	positiveTestChar("  a2465172_urldecode(\"foo=ba%21\")\t\t\t", urlDecodedStr, "foo=ba!");

	char queryString4[] = "foo=ba%2F";
	a2465172_urldecode(queryString4, urlDecodedStr);
	positiveTestChar("  a2465172_urldecode(\"foo=ba%2F\")\t\t\t", urlDecodedStr, "foo=ba/");

	char queryString5[] = "foo=bar+baz";
	a2465172_urldecode(queryString5, urlDecodedStr);
	positiveTestChar("  a2465172_urldecode(\"foo=bar+baz\")\t\t\t", urlDecodedStr, "foo=bar baz");

	printf("\n");
}

static void testParameters(HttpRequest *request) {
	char *value = NULL;

	puts("testParameters:");
	value = a2465172_getString(request, "foo", 16);
	positiveTestChar("  a2465172_getString(request, \"foo\", 16)\t", "bar", value);

	value = a2465172_getString(request, "bar", 16);
	positiveTestChar("  a2465172_getString(request, \"bar\", 16)\t", "baz", value);

	value = a2465172_getString(request, "baz", 16);
	positiveTestChar("  a2465172_getString(request, \"baz\", 16)\t", "XYZ", value);

	printf("\n");
}
