/*
 * testStackArray.c - DevOpsBroker C source file for testing org/devopsbroker/adt/stackarray.h
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
 * Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-25
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "org/devopsbroker/adt/stackarray.h"
#include "org/devopsbroker/lang/string.h"
#include "org/devopsbroker/terminal/ansi.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define PASS "[" ANSI_BOLD ANSI_GREEN "pass" ANSI_RESET "]\n"
#define FAIL "[" ANSI_BOLD ANSI_RED "fail" ANSI_RESET "]\n"

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════

StackArray stackArray;

// ════════════════════════════ Function Prototypes ═══════════════════════════

static void positiveTestInt(char *label, int expected, int actual);
static void positiveTestVoid(char *label, void *expected, void *actual);

static void setupTesting(StackArray *stackArray);
static void tearDownTesting(StackArray *stackArray);

static void testPush(StackArray *stackArray);
static void testPeek(StackArray *stackArray);
static void testPop(StackArray *stackArray);

// ══════════════════════════════════ main() ══════════════════════════════════

int main(int argc, char *argv[]) {
	setupTesting(&stackArray);

	testPush(&stackArray);
	testPeek(&stackArray);
	testPop(&stackArray);

	tearDownTesting(&stackArray);

	// Exit with success
	exit(EXIT_SUCCESS);
}

// ═════════════════════════ Function Implementations ═════════════════════════

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

static void setupTesting(StackArray *stackArray) {
	puts("testStackArray Setup:");
	f106c0ab_initStackArray(stackArray);

	positiveTestInt("  StackArray size = 8\t\t\t\t", 8, stackArray->size);
	positiveTestInt("  StackArray length = 0\t\t\t\t", 0, stackArray->length);

	printf("\n");
}

static void tearDownTesting(StackArray *stackArray) {
	puts("f106c0ab_clear:");
	f106c0ab_clear(stackArray);
	positiveTestInt("  StackArray size = 16\t\t\t\t", 16, stackArray->size);
	positiveTestInt("  StackArray length = 0\t\t\t\t", 0, stackArray->length);

	f106c0ab_cleanUpStackArray(stackArray);

	printf("\n");
}

static void testPush(StackArray *stackArray) {
	puts("f106c0ab_push:");
	puts("f106c0ab_push(stackArray, \"foo\")..."); f106c0ab_push(stackArray, "foo");
	puts("f106c0ab_push(stackArray, \"bar\")..."); f106c0ab_push(stackArray, "bar");
	puts("f106c0ab_push(stackArray, \"baz\")..."); f106c0ab_push(stackArray, "baz");
	puts("f106c0ab_push(stackArray, \"XYZ\")..."); f106c0ab_push(stackArray, "XYZ");
	puts("f106c0ab_push(stackArray, \"123\")..."); f106c0ab_push(stackArray, "123");
	puts("f106c0ab_push(stackArray, \"234\")..."); f106c0ab_push(stackArray, "234");
	puts("f106c0ab_push(stackArray, \"345\")..."); f106c0ab_push(stackArray, "345");
	puts("f106c0ab_push(stackArray, \"456\")..."); f106c0ab_push(stackArray, "456");
	printf("\n");
	positiveTestInt("  StackArray size = 8\t\t\t\t", 8, stackArray->size);
	positiveTestInt("  StackArray length = 8\t\t\t\t", 8, stackArray->length);
	printf("\n");
	puts("f106c0ab_push(stackArray, \"567\")..."); f106c0ab_push(stackArray, "567");
	printf("\n");
	positiveTestInt("  StackArray size = 16\t\t\t\t", 16, stackArray->size);
	positiveTestInt("  StackArray length = 9\t\t\t\t", 9, stackArray->length);

	printf("\n");
}

static void testPeek(StackArray *stackArray) {
	puts("f106c0ab_peek:");
	positiveTestVoid("  f106c0ab_peek(stackArray) = 567\t\t", "567", f106c0ab_peek(stackArray));
	printf("\n");
	positiveTestInt("  StackArray size = 16\t\t\t\t", 16, stackArray->size);
	positiveTestInt("  StackArray length = 9\t\t\t\t", 9, stackArray->length);

	printf("\n");
}

static void testPop(StackArray *stackArray) {
	puts("f106c0ab_pop:");
	positiveTestVoid("  f106c0ab_pop(stackArray) = 567\t\t", "567", f106c0ab_pop(stackArray));
	positiveTestVoid("  f106c0ab_pop(stackArray) = 456\t\t", "456", f106c0ab_pop(stackArray));
	positiveTestVoid("  f106c0ab_pop(stackArray) = 345\t\t", "345", f106c0ab_pop(stackArray));
	positiveTestVoid("  f106c0ab_pop(stackArray) = 234\t\t", "234", f106c0ab_pop(stackArray));
	positiveTestVoid("  f106c0ab_pop(stackArray) = 123\t\t", "123", f106c0ab_pop(stackArray));
	positiveTestVoid("  f106c0ab_pop(stackArray) = XYZ\t\t", "XYZ", f106c0ab_pop(stackArray));
	positiveTestVoid("  f106c0ab_pop(stackArray) = baz\t\t", "baz", f106c0ab_pop(stackArray));
	positiveTestVoid("  f106c0ab_pop(stackArray) = bar\t\t", "bar", f106c0ab_pop(stackArray));
	positiveTestVoid("  f106c0ab_pop(stackArray) = foo\t\t", "foo", f106c0ab_pop(stackArray));
	printf("\n");
	positiveTestInt("  StackArray size = 16\t\t\t\t", 16, stackArray->size);
	positiveTestInt("  StackArray length = 0\t\t\t\t", 0, stackArray->length);

	printf("\n");
}
