/*
 * testHashMap.c - DevOpsBroker C source file for testing org/devopsbroker/adt/hashmap.h
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

#include "org/devopsbroker/adt/hashmap.h"
#include "org/devopsbroker/lang/string.h"
#include "org/devopsbroker/terminal/ansi.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define PASS "[" ANSI_BOLD ANSI_GREEN "pass" ANSI_RESET "]\n"
#define FAIL "[" ANSI_BOLD ANSI_RED "fail" ANSI_RESET "]\n"

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════

HashMap hashMap;

// ════════════════════════════ Function Prototypes ═══════════════════════════

static void positiveTestInt(char *label, int expected, int actual);
static void positiveTestVoid(char *label, void *expected, void *actual);

static void setupTesting(HashMap *hashMap);
static void tearDownTesting(HashMap *hashMap);

static void testPut(HashMap *hashMap);
static void testContainsKey(HashMap *hashMap);
static void testGet(HashMap *hashMap);
static void testRemove(HashMap *hashMap);
static void testReplace(HashMap *hashMap);

// ══════════════════════════════════ main() ══════════════════════════════════

int main(int argc, char *argv[]) {
	setupTesting(&hashMap);

	testPut(&hashMap);
	testContainsKey(&hashMap);
	testGet(&hashMap);
	testRemove(&hashMap);
	testReplace(&hashMap);

	tearDownTesting(&hashMap);

	// Exit with success
	exit(EXIT_SUCCESS);
}

// ═════════════════════════ Function Implementations ═════════════════════════

static void positiveTestBool(char *label, bool expected, bool actual) {
	printf("%s", label);

	if (expected == actual) {
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

static void positiveTestVoid(char *label, void *expected, void *actual) {
	printf("%s", label);

	if (expected == actual) {
		printf(PASS);
	} else {
		printf(FAIL);
	}
}

static void setupTesting(HashMap *hashMap) {
	puts("testHashMap Setup:");
	c47905f7_initHashMap(hashMap, &f6215943_hashCode, &f6215943_isEqual, 8);

	positiveTestInt("  HashMap capacity = 8\t\t\t\t", 8, hashMap->capacity);
	positiveTestInt("  HashMap size = 0\t\t\t\t", 0, hashMap->size);
	positiveTestInt("  HashMap length = 11\t\t\t\t", 11, hashMap->length);

	printf("\n");
}

static void tearDownTesting(HashMap *hashMap) {
	puts("c47905f7_clear:");
	c47905f7_clear(hashMap);
	positiveTestInt("  HashMap capacity = 16\t\t\t\t", 16, hashMap->capacity);
	positiveTestInt("  HashMap size = 0\t\t\t\t", 0, hashMap->size);
	positiveTestInt("  HashMap length = 23\t\t\t\t", 23, hashMap->length);

	c47905f7_cleanUpHashMap(hashMap);

	printf("\n");
}

static void testPut(HashMap *hashMap) {
	puts("c47905f7_put:");
	positiveTestVoid("  c47905f7_put(hashMap, \"foo\", \"bar\")\t\t", NULL, c47905f7_put(hashMap, &"foo", &"bar"));
	positiveTestVoid("  c47905f7_put(hashMap, \"bar\", \"baz\")\t\t", NULL, c47905f7_put(hashMap, &"bar", &"baz"));
	positiveTestVoid("  c47905f7_put(hashMap, \"baz\", \"XYZ\")\t\t", NULL, c47905f7_put(hashMap, &"baz", &"XYZ"));
	positiveTestVoid("  c47905f7_put(hashMap, \"XYZ\", \"123\")\t\t", NULL, c47905f7_put(hashMap, &"XYZ", &"123"));
	positiveTestVoid("  c47905f7_put(hashMap, \"123\", \"234\")\t\t", NULL, c47905f7_put(hashMap, &"123", &"234"));
	positiveTestVoid("  c47905f7_put(hashMap, \"234\", \"345\")\t\t", NULL, c47905f7_put(hashMap, &"234", &"345"));
	positiveTestVoid("  c47905f7_put(hashMap, \"345\", \"456\")\t\t", NULL, c47905f7_put(hashMap, &"345", &"456"));
	positiveTestVoid("  c47905f7_put(hashMap, \"456\", \"567\")\t\t", NULL, c47905f7_put(hashMap, &"456", &"567"));
	printf("\n");
	positiveTestInt("  HashMap capacity = 8\t\t\t\t", 8, hashMap->capacity);
	positiveTestInt("  HashMap size = 8\t\t\t\t", 8, hashMap->size);
	positiveTestInt("  HashMap length = 11\t\t\t\t", 11, hashMap->length);
	printf("\n");
	positiveTestVoid("  c47905f7_put(hashMap, \"567\", \"678\")\t\t", NULL, c47905f7_put(hashMap, &"567", &"678"));
	printf("\n");
	positiveTestInt("  HashMap capacity = 16\t\t\t\t", 16, hashMap->capacity);
	positiveTestInt("  HashMap size = 9\t\t\t\t", 9, hashMap->size);
	positiveTestInt("  HashMap length = 23\t\t\t\t", 23, hashMap->length);

	printf("\n");
}

static void testContainsKey(HashMap *hashMap) {
	puts("c47905f7_containsKey:");
	positiveTestBool("  c47905f7_containsKey(hashMap, \"foo\")\t\t", true, c47905f7_containsKey(hashMap, &"foo"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"bar\")\t\t", true, c47905f7_containsKey(hashMap, &"bar"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"baz\")\t\t", true, c47905f7_containsKey(hashMap, &"baz"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"XYZ\")\t\t", true, c47905f7_containsKey(hashMap, &"XYZ"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"123\")\t\t", true, c47905f7_containsKey(hashMap, &"123"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"234\")\t\t", true, c47905f7_containsKey(hashMap, &"234"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"345\")\t\t", true, c47905f7_containsKey(hashMap, &"345"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"456\")\t\t", true, c47905f7_containsKey(hashMap, &"456"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"567\")\t\t", true, c47905f7_containsKey(hashMap, &"567"));
	positiveTestBool("  c47905f7_containsKey(hashMap, \"678\")\t\t", false, c47905f7_containsKey(hashMap, &"678"));

	printf("\n");
}

static void testGet(HashMap *hashMap) {
	puts("c47905f7_get:");
	positiveTestBool("  c47905f7_get(hashMap, \"foo\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"foo"), "bar"));
	positiveTestBool("  c47905f7_get(hashMap, \"bar\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"bar"), "baz"));
	positiveTestBool("  c47905f7_get(hashMap, \"baz\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"baz"), "XYZ"));
	positiveTestBool("  c47905f7_get(hashMap, \"XYZ\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"XYZ"), "123"));
	positiveTestBool("  c47905f7_get(hashMap, \"123\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"123"), "234"));
	positiveTestBool("  c47905f7_get(hashMap, \"234\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"234"), "345"));
	positiveTestBool("  c47905f7_get(hashMap, \"345\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"345"), "456"));
	positiveTestBool("  c47905f7_get(hashMap, \"456\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"456"), "567"));
	positiveTestBool("  c47905f7_get(hashMap, \"567\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"567"), "678"));
	positiveTestBool("  c47905f7_get(hashMap, \"678\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"678"), NULL));

	printf("\n");
}

static void testRemove(HashMap *hashMap) {
	puts("c47905f7_remove:");
	positiveTestBool("  c47905f7_remove(hashMap, \"foo\")\t\t", true, f6215943_isEqual(c47905f7_remove(hashMap, &"foo"), "bar"));
	positiveTestBool("  c47905f7_remove(hashMap, \"678\")\t\t", true, f6215943_isEqual(c47905f7_remove(hashMap, &"678"), NULL));
	printf("\n");
	positiveTestInt("  HashMap capacity = 16\t\t\t\t", 16, hashMap->capacity);
	positiveTestInt("  HashMap size = 8\t\t\t\t", 8, hashMap->size);
	positiveTestInt("  HashMap length = 23\t\t\t\t", 23, hashMap->length);

	printf("\n");
}

static void testReplace(HashMap *hashMap) {
	puts("c47905f7_put to replace:");
	positiveTestBool("  c47905f7_get(hashMap, \"bar\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"bar"), "baz"));
	positiveTestBool("  c47905f7_put(hashMap, \"bar\", \"foo\")\t\t", true, f6215943_isEqual(c47905f7_put(hashMap, &"bar", &"foo"), "baz"));
	positiveTestBool("  c47905f7_get(hashMap, \"bar\")\t\t\t", true, f6215943_isEqual(c47905f7_get(hashMap, &"bar"), "foo"));
	printf("\n");
	positiveTestInt("  HashMap capacity = 16\t\t\t\t", 16, hashMap->capacity);
	positiveTestInt("  HashMap size = 8\t\t\t\t", 8, hashMap->size);
	positiveTestInt("  HashMap length = 23\t\t\t\t", 23, hashMap->length);

	printf("\n");
}
