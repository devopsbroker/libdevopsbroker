/*
 * unittest.h - DevOpsBroker C header file for unit testing functionality
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
 * echo ORG_DEVOPSBROKER_TEST_UNITTEST | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_TEST_UNITTEST_H
#define ORG_DEVOPSBROKER_TEST_UNITTEST_H

// ═════════════════════════════════ Includes ═════════════════════════════════


// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#include <stdbool.h>

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Unit Testing Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    positiveTestBool
 * Description: Compares expected vs. actual boolean values
 *
 * Parameters:
 *   label      The label to print for the given test
 *   expected   The expected boolean value
 *   actual     The actual boolean value
 * ----------------------------------------------------------------------------
 */
void positiveTestBool(char *label, bool expected, bool actual);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    positiveTestInt
 * Description: Compares expected vs. actual int values
 *
 * Parameters:
 *   label      The label to print for the given test
 *   expected   The expected integer value
 *   actual     The actual integer value
 * ----------------------------------------------------------------------------
 */
void positiveTestInt(char *label, int expected, int actual);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    positiveTestVoid
 * Description: Compares expected vs. actual void pointer values
 *
 * Parameters:
 *   label      The label to print for the given test
 *   expected   The expected void pointer value
 *   actual     The actual void pointer value
 * ----------------------------------------------------------------------------
 */
void positiveTestVoid(char *label, void *expected, void *actual);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    printTestName
 * Description: Prints the test name
 *
 * Parameters:
 *   testName   The name of the test
 * ----------------------------------------------------------------------------
 */
void printTestName(char *testName);

#endif /* ORG_DEVOPSBROKER_TEST_UNITTEST_H */
