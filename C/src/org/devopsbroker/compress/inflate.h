/*
 * inflate.h - DevOpsBroker C header file for the org.devopsbroker.compress.Inflate struct
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
 * echo ORG_DEVOPSBROKER_COMPRESS_INFLATE | md5sum | cut -c 25-32
 *
 * A Deflate stream consists of a series of blocks. Each block is preceded by a
 * 3-bit header:
 *
 *  First bit: Last-block-in-stream marker:
 *      1: This is the last block in the stream
 *      0: There are more blocks to process after this one
 *
 *  Second and third bits: Encoding method used for this block type:
 *      00: A stored (a.k.a. raw or literal) section, between 0 and 65,535 bytes in length
 *      01: A static Huffman compressed block, using a pre-agreed Huffman tree defined in the RFC
 *      10: A compressed block complete with the Huffman table supplied
 *      11: Reserved—don't use
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_COMPRESS_INFLATE_H
#define ORG_DEVOPSBROKER_COMPRESS_INFLATE_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

#include "iobuffer.h"
#include "../memory/slabpool.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define INFLATE_INPUT_LENGTH  8192
#define INFLATE_OUTPUT_LENGTH  32768

// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef enum InflationStatus {
	INFLATE_SUCCESS,              // Inflation was a success
	INFLATE_NEED_INPUT,           // Need more input
	INFLATE_OUTPUT_FULL,          // Output buffer is full
	INFLATE_INPUT_ERROR           // Input data error
} InflationStatus;

typedef struct Inflate {
	InputBuffer       inputBuffer;
	OutputBuffer      outputBuffer;
	uint32_t          inputBlockStart;
	InflationStatus   status;
} Inflate;

static_assert(sizeof(Inflate) == 88, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d592eb82_createInflate
 * Description: Creates an initialized Inflate struct
 *
 * Parameters:
 *   fileBuffer     The FileBuffer instance
 *   compressSize   The compressed size of the input file
 * Returns:         An initialized Inflate struct
 * ----------------------------------------------------------------------------
 */
Inflate *d592eb82_createInflate(FileBuffer *fileBuffer, uint32_t compressSize);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d592eb82_destroyInflate
 * Description: Frees the memory allocated to the Inflate struct pointer
 *
 * Parameters:
 *   inflate    A pointer to the Inflate instance to destroy
 * ----------------------------------------------------------------------------
 */
void d592eb82_destroyInflate(Inflate *inflate);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d592eb82_cleanUpInflate
 * Description: Frees dynamically allocated memory within the Inflate instance
 *
 * Parameters:
 *   inflate    A pointer to the Inflate instance to clean up
 * ----------------------------------------------------------------------------
 */
void d592eb82_cleanUpInflate(Inflate *inflate);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d592eb82_initInflate
 * Description: Initializes an Inflate struct
 *
 * Parameters:
 *   inflate        A pointer to the Inflate instance to initalize
 *   fileBuffer     The FileBuffer instance
 *   compressSize   The compressed size of the input file
 * ----------------------------------------------------------------------------
 */
void d592eb82_initInflate(Inflate *inflate, FileBuffer *fileBuffer, uint32_t compressSize);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    d592eb82_inflate
 * Description: Inflates a previously DEFLATE bitstream
 *
 * Parameters:
 *   inflate	A pointer to the Inflate instance
 * ----------------------------------------------------------------------------
 */
bool d592eb82_inflate(Inflate *inflate);

#endif /* ORG_DEVOPSBROKER_COMPRESS_INFLATE_H */
