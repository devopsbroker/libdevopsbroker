/*
 * testString.c - DevOpsBroker C source file for testing org/devopsbroker/lang/string.h
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

#include "org/devopsbroker/lang/string.h"
#include "org/devopsbroker/terminal/ansi.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define PASS "[" ANSI_BOLD ANSI_GREEN "pass" ANSI_RESET "]\n"
#define FAIL "[" ANSI_BOLD ANSI_RED "fail" ANSI_RESET "]\n"

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static void positiveTest(char *label, int expected, int actual);

static void testHashCode();

// ══════════════════════════════════ main() ══════════════════════════════════

int main(int argc, char *argv[]) {
	testHashCode();

	// Exit with success
	exit(EXIT_SUCCESS);
}

// ═════════════════════════ Function Implementations ═════════════════════════

void positiveTest(char *label, int expected, int actual) {
	printf("%s", label);

	if (expected == actual) {
		printf(PASS);
	} else {
		printf(FAIL);
	}
}

static void testHashCode() {
	puts("testHashCode:");
	positiveTest("  f6215943_hashCode(\"foo\")\t\t", 6807, f6215943_hashCode("foo"));
	positiveTest("  f6215943_hashCode(\"bar\")\t\t", 7162, f6215943_hashCode("bar"));
	positiveTest("  f6215943_hashCode(\"XYZ\")\t\t", 5266, f6215943_hashCode("XYZ"));
	positiveTest("  f6215943_hashCode(\"123\")\t\t", 3555, f6215943_hashCode("123"));
	positiveTest("  f6215943_hashCode(\"international\")\t", 1450727140, f6215943_hashCode("international"));

	printf("\n");
}
