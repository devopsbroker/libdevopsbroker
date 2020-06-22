/*
 * md5.h - DevOpsBroker C header file for the MD5 hash functionality
 *
 * Copyright (C) 2020 Edward Smith <edwardsmith@devopsbroker.org>
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-59
 *
 * echo ORG_DEVOPSBROKER_HASH_MD5 | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_HASH_MD5_H
#define ORG_DEVOPSBROKER_HASH_MD5_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1518caf_getHexString
 * Description: Generates a hexadecimal string representing the MD5 state
 *
 * Parameters:
 *   state      A pointer to the MD5 state array
 * Returns:     A hexadecimal string representing the MD5 state
 * ----------------------------------------------------------------------------
 */
char *f1518caf_getHexString(uint32_t *state);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1518caf_initMD5State
 * Description: Initializes the MD5 state to its default values
 *
 * Parameters:
 *   state      A pointer to the MD5 state array
 * ----------------------------------------------------------------------------
 */
void f1518caf_initMD5State(uint32_t *state);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1518caf_md5
 * Description: Calculates the MD5 of the input data for length bytes
 *
 * Parameters:
 *   state      A pointer to the MD5 state array
 *   buffer     A pointer to the data buffer to calculate the MD5
 *   length     The length of the buffer to calculate
 * ----------------------------------------------------------------------------
 */
void f1518caf_md5(uint32_t *state, void *buffer, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1518caf_md5Rounds
 * Description: Takes an existing MD5 digest state and applies and additional
 *              numRounds transformations to it
 *
 * Parameters:
 *   state      A pointer to the MD5 state array
 *   numRounds  The number of transformation rounds to apply to the MD5 state
 * ----------------------------------------------------------------------------
 */
void f1518caf_md5Rounds(uint32_t *state, uint32_t numRounds);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1518caf_md5WithSalt
 * Description: Applies a variable length salt (up to 64 bytes) to the buffer
 *              before calculating the MD5 digest
 *
 * Parameters:
 *   state      A pointer to the MD5 state array
 *   salt       A byte array of salt values (up to 64 bytes)
 *   saltLen    The length of the salt byte array
 *   buffer     A pointer to the data buffer to calculate the MD5
 *   length     The length of the buffer to calculate
 * ----------------------------------------------------------------------------
 */
void f1518caf_md5WithSalt(uint32_t *state, uint8_t *salt, uint32_t saltLen, void *buffer, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f1518caf_printMD5
 * Description: Prints the MD5 hash digest as per the RFC 1321 spec
 *
 * Parameters:
 *   digest     The MD5 hash digest to print
 * ----------------------------------------------------------------------------
 */
void f1518caf_printMD5(uint32_t *digest);

#endif /* ORG_DEVOPSBROKER_HASH_MD5_H */
