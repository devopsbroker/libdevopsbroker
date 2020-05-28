/*
 * iobuffer.c - DevOpsBroker C source file for DEFLATE buffer-related functionality
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
 * The inflate code comes from hwzip-1.2.zip which can be found at the great
 * blog post on the DEFLATE algorithm at https://www.hanshq.net/zip.html
 *
 * I have made some customizations to the code to get it to work with the
 * libdevopsbroker environment, as well as some custom optimizations. I will
 * continue to make adjustments to the code as time permits.
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _GNU_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "iobuffer.h"

#include "../hash/crc32.h"
#include "../io/file.h"
#include "../io/filebuffer.h"
#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define OFFSET_BIT_END_POS   MEMORY_PAGE_SIZE << 3
#define OFFSET_BIT_MIN       57

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

InputBuffer *c49f5b0d_createInputBuffer(FileBuffer *fileBuffer, uint32_t length) {
	InputBuffer *inputBuffer = f668c4bd_malloc(sizeof(InputBuffer));

	inputBuffer->fileBuffer = fileBuffer;
	inputBuffer->buffer = fileBuffer->buffer;
	inputBuffer->bits = 0;
	inputBuffer->bitPos = 0;
	inputBuffer->totalNumBits = length << 3;
	inputBuffer->offsetBitPos = fileBuffer->dataOffset << 3;

	return inputBuffer;
}

void c49f5b0d_destroyInputBuffer(InputBuffer *inputBuffer) {
	f668c4bd_free(inputBuffer);
}

OutputBuffer *c49f5b0d_createOutputBuffer() {
	OutputBuffer *outputBuffer = f668c4bd_malloc(sizeof(OutputBuffer));

	outputBuffer->slab = b426145b_acquireSlab();
	outputBuffer->currentBuf = outputBuffer->slab->buffer;
	outputBuffer->previousBuf = NULL;
	outputBuffer->length = 0;
	outputBuffer->size = 0;

	return outputBuffer;
}

void c49f5b0d_destroyOutputBuffer(OutputBuffer *outputBuffer) {
	f668c4bd_free(outputBuffer);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void c49f5b0d_initInputBuffer(InputBuffer *inputBuffer, FileBuffer *fileBuffer, uint32_t length) {
	inputBuffer->fileBuffer = fileBuffer;
	inputBuffer->buffer = fileBuffer->buffer;
	inputBuffer->bits = 0;
	inputBuffer->bitPos = 0;
	inputBuffer->totalNumBits = length << 3;
	inputBuffer->offsetBitPos = fileBuffer->dataOffset << 3;
}

void c49f5b0d_cleanUpOutputBuffer(OutputBuffer *outputBuffer) {
	Slab *slabPtr, *nextSlabPtr;

	slabPtr = outputBuffer->slab;

	while (slabPtr != NULL) {
		nextSlabPtr = outputBuffer->slab->next;
		b426145b_releaseSlab(slabPtr);
		slabPtr = nextSlabPtr;
	}
}

void c49f5b0d_initOutputBuffer(OutputBuffer *outputBuffer) {
	outputBuffer->slab = b426145b_acquireSlab();
	outputBuffer->currentBuf = outputBuffer->slab->buffer;
	outputBuffer->previousBuf = NULL;
	outputBuffer->length = 0;
	outputBuffer->size = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

uint32_t c49f5b0d_crc32(OutputBuffer *outputBuffer, uint32_t length) {
	Slab *slabPtr;
	uint32_t bufferLength;
	uint32_t crc32 = 0;

	slabPtr = outputBuffer->slab;

	while (length > 0 && slabPtr != NULL) {
		bufferLength = (SLABPOOL_SLAB_SIZE > length) ? length : SLABPOOL_SLAB_SIZE;

		crc32 = b7e0468d_crc32(slabPtr->buffer, bufferLength, crc32);

		length -= bufferLength;
		slabPtr = slabPtr->next;
	}

	return crc32;
}

bool c49f5b0d_advanceNumBits(InputBuffer *inputBuffer, uint32_t numBits) {
	uint64_t nextBitPos;
	uint32_t nextOffsetBitPos;

	// 1. Check if we are past the end of the bit stream
	nextBitPos = inputBuffer->bitPos + numBits;

	if (nextBitPos > inputBuffer->totalNumBits) {
		return false;
	}

	inputBuffer->bitPos = nextBitPos;

	// 2. Check if we are past the end of the offset bit position
	nextOffsetBitPos = inputBuffer->offsetBitPos + numBits;

	if (nextOffsetBitPos > OFFSET_BIT_END_POS) {
		// Cannot advance if we reached the end of the FileBuffer
		if (inputBuffer->fileBuffer->next == NULL) {
			return false;
		}

		// Adjust the FileBuffer to the next one
		inputBuffer->fileBuffer = inputBuffer->fileBuffer->next;
		inputBuffer->buffer = inputBuffer->fileBuffer->buffer;

		// Set the offset bit position
		nextOffsetBitPos -= OFFSET_BIT_END_POS;
	}

	inputBuffer->offsetBitPos = nextOffsetBitPos;

	return true;
}

void c49f5b0d_getNextBits(InputBuffer *inputBuffer) {
	const uint8_t *next;
	uint64_t bits;
	uint32_t minOffsetBitPos;

	next = inputBuffer->buffer + (inputBuffer->offsetBitPos / 8);
	minOffsetBitPos = inputBuffer->offsetBitPos + OFFSET_BIT_MIN;

	// Perform a normal bit read
	if (minOffsetBitPos <= OFFSET_BIT_END_POS || inputBuffer->fileBuffer->next == NULL) {
		bits = ((uint64_t)next[0] << 0)  |
		       ((uint64_t)next[1] << 8)  |
			   ((uint64_t)next[2] << 16) |
			   ((uint64_t)next[3] << 24) |
			   ((uint64_t)next[4] << 32) |
			   ((uint64_t)next[5] << 40) |
			   ((uint64_t)next[6] << 48) |
			   ((uint64_t)next[7] << 56);
	} else {
		uint32_t endCurrentBuf;
		uint32_t i, j;

		endCurrentBuf = MEMORY_PAGE_SIZE - (inputBuffer->offsetBitPos / 8);

		bits = 0;
		for (i = 0; i < endCurrentBuf; i++) {
			bits |= (uint64_t)next[i] << (i * 8);
		}

		next = inputBuffer->fileBuffer->next->buffer;

		for (j=0; i < 8; i++, j++) {
			bits |= (uint64_t)next[j] << (i * 8);
		}
	}

	inputBuffer->bits = bits >> (inputBuffer->offsetBitPos % 8);
}

bool c49f5b0d_useNumBits(InputBuffer *inputBuffer, uint32_t numBits) {
	const uint8_t *next;
	uint64_t bits;
	uint32_t minOffsetBitPos;
	uint64_t nextBitPos;
	uint32_t nextOffsetBitPos;

	// 1. Check if we are past the end of the bit stream
	nextBitPos = inputBuffer->bitPos + numBits;

	if (nextBitPos > inputBuffer->totalNumBits) {
		return false;
	}

	inputBuffer->bitPos = nextBitPos;

	next = inputBuffer->buffer + (inputBuffer->offsetBitPos / 8);
	minOffsetBitPos = inputBuffer->offsetBitPos + OFFSET_BIT_MIN;

	// 2. Perform a normal bit read
	if (minOffsetBitPos <= OFFSET_BIT_END_POS || inputBuffer->fileBuffer->next == NULL) {
		bits = ((uint64_t)next[0] << 0)  |
			   ((uint64_t)next[1] << 8)  |
			   ((uint64_t)next[2] << 16) |
			   ((uint64_t)next[3] << 24) |
			   ((uint64_t)next[4] << 32) |
			   ((uint64_t)next[5] << 40) |
			   ((uint64_t)next[6] << 48) |
			   ((uint64_t)next[7] << 56);

	// 3. Perform a cross-page bit read
	} else {
		uint32_t endCurrentBuf;
		uint32_t i, j;

		endCurrentBuf = MEMORY_PAGE_SIZE - (inputBuffer->offsetBitPos / 8);

		bits = 0;
		for (i = 0; i < endCurrentBuf; i++) {
			bits |= (uint64_t)next[i] << (i * 8);
		}

		next = inputBuffer->fileBuffer->next->buffer;

		for (j=0; i < 8; i++, j++) {
			bits |= (uint64_t)next[j] << (i * 8);
		}
	}

	// 4. Check if we are past the end of the offset bit position
	nextOffsetBitPos = inputBuffer->offsetBitPos + numBits;

	if (nextOffsetBitPos > OFFSET_BIT_END_POS) {
		// Cannot advance if we reached the end of the FileBuffer
		if (inputBuffer->fileBuffer->next == NULL) {
			return false;
		}

		// Adjust the FileBuffer to the next one
		inputBuffer->fileBuffer = inputBuffer->fileBuffer->next;
		inputBuffer->buffer = inputBuffer->fileBuffer->buffer;

		// Set the offset bit position
		nextOffsetBitPos -= OFFSET_BIT_END_POS;
	}

	inputBuffer->bits = bits >> (inputBuffer->offsetBitPos % 8);
	inputBuffer->offsetBitPos = nextOffsetBitPos;

	return true;
}

void c49f5b0d_write(OutputBuffer *outputBuffer, int fd, char *pathName) {
	// Write out OutputBuffer
	e2f74138_writeFile(fd, outputBuffer->currentBuf, outputBuffer->length, pathName);
}
