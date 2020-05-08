/*
 * crc32.h - DevOpsBroker C header file for the CRC-32 hash functionality
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-46
 *
 * echo ORG_DEVOPSBROKER_HASH_CRC32 | md5sum | cut -c 25-32
 *
 * The algorithm used to calcualte the CRC-32 is the Half-Byte version found
 * at https://create.stephan-brumme.com/crc32/. It is documented below for
 * local reference since I implemented it in x86-64 assembly.
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_HASH_CRC32_H
#define ORG_DEVOPSBROKER_HASH_CRC32_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b7e0468d_crc32
 * Description: Calculates the CRC-32 of the input data for length bytes
 *
 * Parameters:
 *   buffer     A pointer to the data buffer to calculate the CRC-32
 *   length     The length of the buffer to calculate
 *   crc32      The initial CRC-32 value to start from (allows chunk calculations)
 * Returns:     The calculated CRC-32 of the buffer
 * ----------------------------------------------------------------------------
 */
uint32_t b7e0468d_crc32(void *buffer, uint32_t length, uint32_t crc32);

/*
	From https://create.stephan-brumme.com/crc32/#half-byte

	uint32_t crc32_halfbyte(const void* data, size_t length, uint32_t previousCrc32 = 0) {
		uint32_t crc = ~previousCrc32;
		unsigned char* current = (unsigned char*) data;

		static uint32_t int lut[16] =
		{
		  0x00000000,0x1DB71064,0x3B6E20C8,0x26D930AC,0x76DC4190,0x6B6B51F4,0x4DB26158,0x5005713C,
		  0xEDB88320,0xF00F9344,0xD6D6A3E8,0xCB61B38C,0x9B64C2B0,0x86D3D2D4,0xA00AE278,0xBDBDF21C
		};

		while (length--) {
			crc = lut[(crc ^ *current) & 0x0F] ^ (crc >> 4);
			crc = lut[(crc ^ (*current >> 4)) & 0x0F] ^ (crc >> 4);
			current++;
		}

		return ~crc;
	}
*/

#endif /* ORG_DEVOPSBROKER_HASH_CRC32_H */
