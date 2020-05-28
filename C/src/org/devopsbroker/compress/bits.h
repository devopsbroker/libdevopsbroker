/*
 * bits.h - DevOpsBroker C header file for bit-manipulation functionality
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-51
 *
 * echo ORG_DEVOPSBROKER_COMPRESS_BITS | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_COMPRESS_BITS_H
#define ORG_DEVOPSBROKER_COMPRESS_BITS_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    e474415b_reverse16
 * Description: Reverses the 16-bit value and sets the (16 - numBits) most
 *              significant bits to zero
 *
 * Parameters:
 *   value      The 16-bit value to reverse
 *   numBits    The number of most significant bits to set to zero
 * Returns:     The reversed 16-bit value
 * ----------------------------------------------------------------------------
 */
uint16_t e474415b_reverse16(uint16_t value, uint32_t numBits);

#endif /* ORG_DEVOPSBROKER_COMPRESS_BITS_H */
