/*
 * iobuffer.h - DevOpsBroker C header file for DEFLATE buffer-related functionality
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
 * echo ORG_DEVOPSBROKER_COMPRESS_IOBUFFER | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_COMPRESS_IOBUFFER_H
#define ORG_DEVOPSBROKER_COMPRESS_IOBUFFER_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

#include "../io/filebuffer.h"
#include "../memory/slabpool.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct InputBuffer {
	FileBuffer *fileBuffer;
	uint8_t    *buffer;
	uint64_t    bits;
	uint64_t    bitPos;
	uint64_t    totalNumBits;
	uint32_t    offsetBitPos;
} InputBuffer;

static_assert(sizeof(InputBuffer) == 48, "Check your assumptions");

typedef struct OutputBuffer {
	Slab    *slab;
	uint8_t *currentBuf;
	uint8_t *previousBuf;
	uint32_t length;
	uint32_t size;
} OutputBuffer;

static_assert(sizeof(OutputBuffer) == 32, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_createInputBuffer
 * Description: Creates an InputBuffer struct instance
 *
 * Parameters:
 *   fileBuffer     The FileBuffer to use for the input data
 *   length         The length of the input data
 * Returns:     An InputBuffer struct instance
 * ----------------------------------------------------------------------------
 */
InputBuffer *c49f5b0d_createInputBuffer(FileBuffer *fileBuffer, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_destroyInputBuffer
 * Description: Frees the memory allocated to the InputBuffer struct pointer
 *
 * Parameters:
 *   inputBuffer    A pointer to the InputBuffer instance to destroy
 * ----------------------------------------------------------------------------
 */
void c49f5b0d_destroyInputBuffer(InputBuffer *inputBuffer);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_createOutputBuffer
 * Description: Creates an OutputBuffer struct instance
 *
 * Returns:     An OutputBuffer struct instance
 * ----------------------------------------------------------------------------
 */
OutputBuffer *c49f5b0d_createOutputBuffer();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_destroyOutputBuffer
 * Description: Frees the memory allocated to the OutputBuffer struct pointer
 *
 * Parameters:
 *   outputBuffer   A pointer to the OutputBuffer instance to destroy
 * ----------------------------------------------------------------------------
 */
void c49f5b0d_destroyOutputBuffer(OutputBuffer *outputBuffer);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_initInputBuffer
 * Description: Initializes an existing InputBuffer struct
 *
 * Parameters:
 *   inputBuffer    A pointer to the InputBuffer instance to initalize
 *   fileBuffer     The FileBuffer to use for the input data
 *   length         The length of the input data
 * ----------------------------------------------------------------------------
 */
void c49f5b0d_initInputBuffer(InputBuffer *inputBuffer, FileBuffer *fileBuffer, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_cleanUpOutputBuffer
 * Description: Frees dynamically allocated memory within the OutputBuffer instance
 *
 * Parameters:
 *   outputBuffer   A pointer to the OutputBuffer instance to clean up
 * ----------------------------------------------------------------------------
 */
void c49f5b0d_cleanUpOutputBuffer(OutputBuffer *outputBuffer);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_initOutputBuffer
 * Description: Initializes an existing OutputBuffer struct
 *
 * Parameters:
 *   outputBuffer   A pointer to the OutputBuffer instance to initalize
 * ----------------------------------------------------------------------------
 */
void c49f5b0d_initOutputBuffer(OutputBuffer *outputBuffer);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_crc32
 * Description: Calculates the CRC-32 of the OutputBuffer for length bytes
 *
 * Parameters:
 *   outputBuffer   A pointer to the OutputBuffer instance
 *   length         The length of the data to calculate
 * Returns:     The calculated CRC-32 of the output buffer
 * ----------------------------------------------------------------------------
 */
uint32_t c49f5b0d_crc32(OutputBuffer *outputBuffer, uint32_t length);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_advanceNumBits
 * Description: Advances the InputBuffer numBits
 *
 * Parameters:
 *   inputBuffer    A pointer to the InputBuffer instance
 *   numBits        The number of bits to advance the InputBuffer
 * Returns:     True if there were enough bits in the buffer, false otherwise
 * ----------------------------------------------------------------------------
 */
bool c49f5b0d_advanceNumBits(InputBuffer *inputBuffer, uint32_t numBits);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_getNextBits
 * Description: Sets the next bits from the InputBuffer
 *
 * Parameters:
 *   inputBuffer    A pointer to the InputBuffer instance
 * ----------------------------------------------------------------------------
 */
void c49f5b0d_getNextBits(InputBuffer *inputBuffer);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c49f5b0d_useNumBits
 * Description: Gets the next bits and advances the InputBuffer numBits
 *
 * Parameters:
 *   inputBuffer    A pointer to the InputBuffer instance
 *   numBits        The number of bits to advance the InputBuffer
 * Returns:     True if there were enough bits in the buffer, false otherwise
 * ----------------------------------------------------------------------------
 */
bool c49f5b0d_useNumBits(InputBuffer *inputBuffer, uint32_t numBits);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    ce97d170_write
 * Description: Writes an OutputBuffer instance to the specified file descriptor
 *
 * Parameters:
 *   outputBuffer   A pointer to the OutputBuffer instance
 *   fd             The file descriptor to write to
 *   pathName       The name of the file to write to (used for error handling)
 * ----------------------------------------------------------------------------
 */
void c49f5b0d_write(OutputBuffer *outputBuffer, int fd, char *pathName);

#endif /* ORG_DEVOPSBROKER_COMPRESS_IOBUFFER_H */
