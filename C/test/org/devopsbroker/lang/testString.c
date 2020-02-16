/*
 * testString.c - DevOpsBroker C source file for testing org/devopsbroker/lang/string.h
 *
 * Copyright (C) 2019-2020 Edward Smith <edwardsmith@devopsbroker.org>
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

#include "org/devopsbroker/lang/string.h"
#include "org/devopsbroker/test/unittest.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static void testCopy();
static void testCopyToBuffer();
static void testEndsWith();
static void testGetLength();
static void testHashCode();

// ══════════════════════════════════ main() ══════════════════════════════════

int main(int argc, char *argv[]) {
	testCopy();
	testCopyToBuffer();
	testEndsWith();
	testGetLength();
	testHashCode();

	// Exit with success
	exit(EXIT_SUCCESS);
}

// ═════════════════════════ Function Implementations ═════════════════════════

static void testCopy() {
	char *foo = NULL;
	char dest[8];

	printTestName("testCopy");
	foo = f6215943_copy("foo", dest);
	positiveTestBool("  f6215943_copy(\"foo\", dest)\t\t\t\t", true, f6215943_isEqual("foo", foo));

	foo = f6215943_copy("bar", dest);
	positiveTestBool("  f6215943_copy(\"bar\", dest)\t\t\t\t", true, f6215943_isEqual("bar", foo));

	foo = f6215943_copy("XYZ", dest);
	positiveTestBool("  f6215943_copy(\"XYZ\", dest)\t\t\t\t", true, f6215943_isEqual("XYZ", foo));

	printf("\n");
}

static void testCopyToBuffer() {
	char dest[16];

	printTestName("testCopyToBuffer");
	f6215943_copyToBuffer("foo", dest, 4);
	positiveTestBool("  f6215943_copyToBuffer(\"foo\", dest, 4)\t\t\t", true, f6215943_isEqual("foo", dest));

	f6215943_copyToBuffer("foo", dest, 3);
	positiveTestBool("  f6215943_copyToBuffer(\"foo\", dest, 3)\t\t\t", true, f6215943_isEqual("fo", dest));

	f6215943_copyToBuffer("international", dest, 8);
	positiveTestBool("  f6215943_copyToBuffer(\"international\", dest, 8)\t", true, f6215943_isEqual("interna", dest));

	f6215943_copyToBuffer("international", dest, 14);
	positiveTestBool("  f6215943_copyToBuffer(\"international\", dest, 14)\t", true, f6215943_isEqual("international", dest));

	f6215943_copyToBuffer("international", dest, 9);
	positiveTestBool("  f6215943_copyToBuffer(\"international\", dest, 9)\t", true, f6215943_isEqual("internat", dest));

	printf("\n");
}

static void testEndsWith() {
	char pattern[5] = ".asm";
	char text[16];

	printTestName("testEndsWith");
	f6215943_copyToBuffer("foo.c", text, 6);
	positiveTestBool("  f6215943_endsWith(\".asm\", \"foo.c\")\t\t\t", false, f6215943_endsWith(pattern, "foo.c"));

	f6215943_copyToBuffer("foo.asm.linux", text, 14);
	positiveTestBool("  f6215943_endsWith(\".asm\", \"foo.asm.linux\")\t\t", false, f6215943_endsWith(pattern, "foo.asm.linux"));

	f6215943_copyToBuffer("foo.linux.asm", text, 14);
	positiveTestBool("  f6215943_endsWith(\".asm\", \"foo.linux.asm\")\t\t", true, f6215943_endsWith(pattern, "foo.linux.asm"));

	f6215943_copyToBuffer("foo.linux..asm", text, 15);
	positiveTestBool("  f6215943_endsWith(\".asm\", \"foo.linux..asm\")\t\t", true, f6215943_endsWith(pattern, "foo.linux..asm"));

	printf("\n");
}

static void testGetLength() {
	printTestName("testGetLength");
	positiveTestInt("  f6215943_getLength(NULL)\t\t\t\t", 0, f6215943_getLength(NULL));
	positiveTestInt("  f6215943_getLength(\"foo.c\")\t\t\t\t", 5, f6215943_getLength("foo.c"));
	positiveTestInt("  f6215943_getLength(\"foobar.asm\")\t\t\t", 10, f6215943_getLength("foobar.asm"));
	positiveTestInt("  f6215943_getLength(\"internationalization\")\t\t", 20, f6215943_getLength("internationalization"));

	printf("\n");
}

static void testHashCode() {
	printTestName("testHashCode");
	positiveTestInt("  f6215943_hashCode(\"foo\")\t\t\t\t", 6807, f6215943_hashCode("foo"));
	positiveTestInt("  f6215943_hashCode(\"bar\")\t\t\t\t", 7162, f6215943_hashCode("bar"));
	positiveTestInt("  f6215943_hashCode(\"XYZ\")\t\t\t\t", 5266, f6215943_hashCode("XYZ"));
	positiveTestInt("  f6215943_hashCode(\"123\")\t\t\t\t", 3555, f6215943_hashCode("123"));
	positiveTestInt("  f6215943_hashCode(\"international\")\t\t\t", 1450727140, f6215943_hashCode("international"));

	printf("\n");
}
