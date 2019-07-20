/*
 * testInteger.c - DevOpsBroker C source file for testing org/devopsbroker/lang/integer.h
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

#include "org/devopsbroker/lang/integer.h"
#include "org/devopsbroker/test/unittest.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static void testGetStringSize_int32();
static void testGetStringSize_uint32();
static void testParseInt();

// ══════════════════════════════════ main() ══════════════════════════════════

int main(int argc, char *argv[]) {
	testGetStringSize_int32();
	testGetStringSize_uint32();
	testParseInt();

	// Exit with success
	exit(EXIT_SUCCESS);
}

// ═════════════════════════ Function Implementations ═════════════════════════

static void testGetStringSize_int32() {
	printTestName("testGetStringSize_int32");
	positiveTestInt("  f45efac2_getStringSize_int32(1)\t\t", 2, f45efac2_getStringSize_int32(1));
	positiveTestInt("  f45efac2_getStringSize_int32(10)\t\t", 3, f45efac2_getStringSize_int32(10));
	positiveTestInt("  f45efac2_getStringSize_int32(100)\t\t", 4, f45efac2_getStringSize_int32(100));
	positiveTestInt("  f45efac2_getStringSize_int32(1000)\t\t", 5, f45efac2_getStringSize_int32(1000));
	positiveTestInt("  f45efac2_getStringSize_int32(10000)\t\t", 6, f45efac2_getStringSize_int32(10000));
	positiveTestInt("  f45efac2_getStringSize_int32(100000)\t\t", 7, f45efac2_getStringSize_int32(100000));
	positiveTestInt("  f45efac2_getStringSize_int32(1000000)\t\t", 8, f45efac2_getStringSize_int32(1000000));
	positiveTestInt("  f45efac2_getStringSize_int32(10000000)\t", 9, f45efac2_getStringSize_int32(10000000));
	positiveTestInt("  f45efac2_getStringSize_int32(100000000)\t", 10, f45efac2_getStringSize_int32(100000000));
	positiveTestInt("  f45efac2_getStringSize_int32(1000000000)\t", 11, f45efac2_getStringSize_int32(1000000000));
	positiveTestInt("  f45efac2_getStringSize_int32(-1)\t\t", 3, f45efac2_getStringSize_int32(-1));
	positiveTestInt("  f45efac2_getStringSize_int32(-10)\t\t", 4, f45efac2_getStringSize_int32(-10));
	positiveTestInt("  f45efac2_getStringSize_int32(-100)\t\t", 5, f45efac2_getStringSize_int32(-100));
	positiveTestInt("  f45efac2_getStringSize_int32(-1000)\t\t", 6, f45efac2_getStringSize_int32(-1000));
	positiveTestInt("  f45efac2_getStringSize_int32(-10000)\t\t", 7, f45efac2_getStringSize_int32(-10000));
	positiveTestInt("  f45efac2_getStringSize_int32(-100000)\t\t", 8, f45efac2_getStringSize_int32(-100000));
	positiveTestInt("  f45efac2_getStringSize_int32(-1000000)\t", 9, f45efac2_getStringSize_int32(-1000000));
	positiveTestInt("  f45efac2_getStringSize_int32(-10000000)\t", 10, f45efac2_getStringSize_int32(-10000000));
	positiveTestInt("  f45efac2_getStringSize_int32(-100000000)\t", 11, f45efac2_getStringSize_int32(-100000000));
	positiveTestInt("  f45efac2_getStringSize_int32(-1000000000)\t", 12, f45efac2_getStringSize_int32(-1000000000));
	printf("\n");
}

static void testGetStringSize_uint32() {
	printTestName("testGetStringSize_uint32");
	positiveTestInt("  f45efac2_getStringSize_uint32(1)\t\t", 2, f45efac2_getStringSize_uint32(1));
	positiveTestInt("  f45efac2_getStringSize_uint32(10)\t\t", 3, f45efac2_getStringSize_uint32(10));
	positiveTestInt("  f45efac2_getStringSize_uint32(100)\t\t", 4, f45efac2_getStringSize_uint32(100));
	positiveTestInt("  f45efac2_getStringSize_uint32(1000)\t\t", 5, f45efac2_getStringSize_uint32(1000));
	positiveTestInt("  f45efac2_getStringSize_uint32(10000)\t\t", 6, f45efac2_getStringSize_uint32(10000));
	positiveTestInt("  f45efac2_getStringSize_uint32(100000)\t\t", 7, f45efac2_getStringSize_uint32(100000));
	positiveTestInt("  f45efac2_getStringSize_uint32(1000000)\t", 8, f45efac2_getStringSize_uint32(1000000));
	positiveTestInt("  f45efac2_getStringSize_uint32(10000000)\t", 9, f45efac2_getStringSize_uint32(10000000));
	positiveTestInt("  f45efac2_getStringSize_uint32(100000000)\t", 10, f45efac2_getStringSize_uint32(100000000));
	positiveTestInt("  f45efac2_getStringSize_uint32(1000000000)\t", 11, f45efac2_getStringSize_uint32(1000000000));
	printf("\n");
}


static void testParseInt() {
	int value;

	printTestName("f45efac2_parseInt");
	f45efac2_parseInt("1", &value);
	positiveTestInt("  f45efac2_parseInt(1)\t\t\t", 1, value);
	f45efac2_parseInt("10", &value);
	positiveTestInt("  f45efac2_parseInt(10)\t\t\t", 10, value);
	f45efac2_parseInt("100", &value);
	positiveTestInt("  f45efac2_parseInt(100)\t\t", 100, value);
	f45efac2_parseInt("1000", &value);
	positiveTestInt("  f45efac2_parseInt(1000)\t\t", 1000, value);
	f45efac2_parseInt("10000", &value);
	positiveTestInt("  f45efac2_parseInt(10000)\t\t", 10000, value);
	f45efac2_parseInt("100000", &value);
	positiveTestInt("  f45efac2_parseInt(100000)\t\t", 100000, value);
	f45efac2_parseInt("1000000", &value);
	positiveTestInt("  f45efac2_parseInt(1000000)\t\t", 1000000, value);
	f45efac2_parseInt("10000000", &value);
	positiveTestInt("  f45efac2_parseInt(10000000)\t\t", 10000000, value);
	f45efac2_parseInt("100000000", &value);
	positiveTestInt("  f45efac2_parseInt(100000000)\t\t", 100000000, value);
	f45efac2_parseInt("1000000000", &value);
	positiveTestInt("  f45efac2_parseInt(1000000000)\t\t", 1000000000, value);
	f45efac2_parseInt("-1", &value);
	positiveTestInt("  f45efac2_parseInt(-1)\t\t\t", -1, value);
	f45efac2_parseInt("-10", &value);
	positiveTestInt("  f45efac2_parseInt(-10)\t\t", -10, value);
	f45efac2_parseInt("-100", &value);
	positiveTestInt("  f45efac2_parseInt(-100)\t\t", -100, value);
	f45efac2_parseInt("-1000", &value);
	positiveTestInt("  f45efac2_parseInt(-1000)\t\t", -1000, value);
	f45efac2_parseInt("-10000", &value);
	positiveTestInt("  f45efac2_parseInt(-10000)\t\t", -10000, value);
	f45efac2_parseInt("-100000", &value);
	positiveTestInt("  f45efac2_parseInt(-100000)\t\t", -100000, value);
	f45efac2_parseInt("-1000000", &value);
	positiveTestInt("  f45efac2_parseInt(-1000000)\t\t", -1000000, value);
	f45efac2_parseInt("-10000000", &value);
	positiveTestInt("  f45efac2_parseInt(-10000000)\t\t", -10000000, value);
	f45efac2_parseInt("-100000000", &value);
	positiveTestInt("  f45efac2_parseInt(-100000000)\t\t", -100000000, value);
	f45efac2_parseInt("-1000000000", &value);
	positiveTestInt("  f45efac2_parseInt(-1000000000)\t", -1000000000, value);

	printf("\n");
}
