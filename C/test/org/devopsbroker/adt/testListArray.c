/*
 * testListArray.c - DevOpsBroker C source file for testing org/devopsbroker/adt/listarray.h
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

#include "org/devopsbroker/adt/listarray.h"
#include "org/devopsbroker/adt/stackarray.h"
#include "org/devopsbroker/test/unittest.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════

ListArray listArray;

// ════════════════════════════ Function Prototypes ═══════════════════════════

static void setupTesting(ListArray *listArray);
static void tearDownTesting(ListArray *listArray);

static void testAdd(ListArray *listArray);
static void testAddAll(ListArray *listArray);
static void testAddFromStack(ListArray *listArray);
static void testEnsureCapacity(ListArray *listArray);
static void testLast(ListArray *listArray);

// ══════════════════════════════════ main() ══════════════════════════════════

int main(int argc, char *argv[]) {
	setupTesting(&listArray);

	testAdd(&listArray);
	testAddAll(&listArray);
	testAddFromStack(&listArray);
	testEnsureCapacity(&listArray);
	testLast(&listArray);

	tearDownTesting(&listArray);

	// Exit with success
	exit(EXIT_SUCCESS);
}

// ═════════════════════════ Function Implementations ═════════════════════════

static void setupTesting(ListArray *listArray) {
	printTestName("testListArray Setup");
	b196167f_initListArray(listArray);

	positiveTestInt("  ListArray size = 8\t\t\t\t", 8, listArray->size);
	positiveTestInt("  ListArray length = 0\t\t\t\t", 0, listArray->length);

	printf("\n");
}

static void tearDownTesting(ListArray *listArray) {
	printTestName("b196167f_clear");
	b196167f_clear(listArray);
	positiveTestInt("  ListArray size = 32\t\t\t\t", 32, listArray->size);
	positiveTestInt("  ListArray length = 0\t\t\t\t", 0, listArray->length);

	b196167f_cleanUpListArray(listArray);

	printf("\n");
}

static void testAdd(ListArray *listArray) {
	printTestName("b196167f_add");
	puts("  b196167f_add(listArray, \"foo\")..."); b196167f_add(listArray, "foo");
	puts("  b196167f_add(listArray, \"bar\")..."); b196167f_add(listArray, "bar");
	puts("  b196167f_add(listArray, \"baz\")..."); b196167f_add(listArray, "baz");
	puts("  b196167f_add(listArray, \"XYZ\")..."); b196167f_add(listArray, "XYZ");
	puts("  b196167f_add(listArray, \"123\")..."); b196167f_add(listArray, "123");
	puts("  b196167f_add(listArray, \"234\")..."); b196167f_add(listArray, "234");
	puts("  b196167f_add(listArray, \"345\")..."); b196167f_add(listArray, "345");
	puts("  b196167f_add(listArray, \"456\")..."); b196167f_add(listArray, "456");
	printf("\n");
	positiveTestInt("  ListArray size = 8\t\t\t\t", 8, listArray->size);
	positiveTestInt("  ListArray length = 8\t\t\t\t", 8, listArray->length);
	printf("\n");
	puts("  b196167f_add(listArray, \"567\")..."); b196167f_add(listArray, "567");
	printf("\n");
	positiveTestInt("  ListArray size = 16\t\t\t\t", 16, listArray->size);
	positiveTestInt("  ListArray length = 9\t\t\t\t", 9, listArray->length);
	printf("\n");
	positiveTestVoid("  b196167f_get(listArray, 0) = foo\t\t", "foo", b196167f_get(listArray, 0));
	positiveTestVoid("  b196167f_get(listArray, 1) = bar\t\t", "bar", b196167f_get(listArray, 1));
	positiveTestVoid("  b196167f_get(listArray, 2) = baz\t\t", "baz", b196167f_get(listArray, 2));
	positiveTestVoid("  b196167f_get(listArray, 3) = XYZ\t\t", "XYZ", b196167f_get(listArray, 3));
	positiveTestVoid("  b196167f_get(listArray, 4) = 123\t\t", "123", b196167f_get(listArray, 4));
	positiveTestVoid("  b196167f_get(listArray, 5) = 234\t\t", "234", b196167f_get(listArray, 5));
	positiveTestVoid("  b196167f_get(listArray, 6) = 345\t\t", "345", b196167f_get(listArray, 6));
	positiveTestVoid("  b196167f_get(listArray, 7) = 456\t\t", "456", b196167f_get(listArray, 7));
	positiveTestVoid("  b196167f_get(listArray, 8) = 567\t\t", "567", b196167f_get(listArray, 8));

	printf("\n");
}

static void testAddAll(ListArray *listArray) {
	char *elementArray[4] = { "mov", "add", "mul", "shl" };

	printTestName("b196167f_addAll");
	puts("  b196167f_addAll(listArray, elementArray, 4)..."); b196167f_addAll(listArray, elementArray, 4);
	printf("\n");
	positiveTestInt("  ListArray size = 16\t\t\t\t", 16, listArray->size);
	positiveTestInt("  ListArray length = 13\t\t\t\t", 13, listArray->length);
	printf("\n");
	positiveTestVoid("  b196167f_get(listArray, 9) = mov\t\t", "mov", b196167f_get(listArray, 9));
	positiveTestVoid("  b196167f_get(listArray, 10) = add\t\t", "add", b196167f_get(listArray, 10));
	positiveTestVoid("  b196167f_get(listArray, 11) = mul\t\t", "mul", b196167f_get(listArray, 11));
	positiveTestVoid("  b196167f_get(listArray, 12) = shl\t\t", "shl", b196167f_get(listArray, 12));

	printf("\n");
}

static void testAddFromStack(ListArray *listArray) {
	StackArray stackArray;
	void *topStackPtr;

	f106c0ab_initStackArray(&stackArray);
	f106c0ab_push(&stackArray, "shr");
	f106c0ab_push(&stackArray, "cmp");
	f106c0ab_push(&stackArray, "test");
	f106c0ab_push(&stackArray, "jmp");

	topStackPtr = &stackArray.values[3];

	printTestName("b196167f_addFromStack");
	puts("  b196167f_addFromStack(listArray, topStackPtr, 4)..."); b196167f_addFromStack(listArray, topStackPtr, 4);
	printf("\n");
	positiveTestInt("  ListArray size = 25\t\t\t\t", 25, listArray->size);
	positiveTestInt("  ListArray length = 17\t\t\t\t", 17, listArray->length);
	printf("\n");
	positiveTestVoid("  b196167f_get(listArray, 13) = jmp\t\t", "jmp", b196167f_get(listArray, 13));
	positiveTestVoid("  b196167f_get(listArray, 14) = test\t\t", "test", b196167f_get(listArray, 14));
	positiveTestVoid("  b196167f_get(listArray, 15) = cmp\t\t", "cmp", b196167f_get(listArray, 15));
	positiveTestVoid("  b196167f_get(listArray, 16) = shr\t\t", "shr", b196167f_get(listArray, 16));

	printf("\n");
}

static void testEnsureCapacity(ListArray *listArray) {
	printTestName("b196167f_ensureCapacity");
	puts("  b196167f_ensureCapacity(listArray, 12)..."); b196167f_ensureCapacity(listArray, 12);
	printf("\n");
	positiveTestInt("  ListArray size = 25\t\t\t\t", 25, listArray->size);
	positiveTestInt("  ListArray length = 17\t\t\t\t", 17, listArray->length);
	printf("\n");
	puts("  b196167f_ensureCapacity(listArray, 26)..."); b196167f_ensureCapacity(listArray, 26);
	printf("\n");
	positiveTestInt("  ListArray size = 32\t\t\t\t", 32, listArray->size);
	positiveTestInt("  ListArray length = 17\t\t\t\t", 17, listArray->length);

	printf("\n");
}

static void testLast(ListArray *listArray) {
	printTestName("b196167f_last");
	positiveTestVoid("  b196167f_last(listArray) = shr\t\t", "shr", b196167f_last(listArray));

	printf("\n");
}
