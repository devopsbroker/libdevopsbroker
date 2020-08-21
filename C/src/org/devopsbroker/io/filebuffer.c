/*
 * filebuffer.c - DevOpsBroker C source file for the org.devopsbroker.io.FileBuffer struct
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
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _GNU_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "filebuffer.h"

#include "../hash/crc32.h"
#include "../lang/error.h"
#include "../lang/integer.h"
#include "../lang/memory.h"
#include "../lang/stringbuilder.h"
#include "../memory/memorypool.h"
#include "../memory/pagepool.h"
#include "../memory/slabpool.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define FILEBUFFERLIST_DEFAULT_SIZE 8

// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct FileBufferPool {
	StackArray structStack;
	uint32_t   numFileBufferAlloc;
	uint32_t   numFileBufferFree;
	uint32_t   numFileBufferInUse;
	uint32_t   numFileBufferUsed;
} FileBufferPool;

static_assert(sizeof(FileBufferPool) == 32, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════

FileBufferPool fileBufferPool = { {NULL, 0, 0}, 0, 0, 0, 0 };

// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Acquire/Release Functions ~~~~~~~~~~~~~~~~~~~~~~~~

FileBuffer *ce97d170_acquireFileBuffer(void *internalBuf) {
	FileBuffer *fileBuffer;

	// Lazy-initialize FileBufferPool
	if (fileBufferPool.structStack.values == NULL) {
		f106c0ab_initStackArray(&fileBufferPool.structStack);
	}

	if (fileBufferPool.structStack.length == 0) {
		// Acquire new FileBuffer from the memory pool
		fileBuffer = f502a409_acquireMemory(sizeof(FileBuffer));
		fileBufferPool.numFileBufferAlloc++;
	} else {
		fileBuffer = f106c0ab_pop(&fileBufferPool.structStack);
		fileBufferPool.numFileBufferFree--;
	}

	// Initialize FileBuffer attributes
	fileBuffer->buffer = internalBuf;
	fileBuffer->next = NULL;
	fileBuffer->fileOffset = 0;
	fileBuffer->dataOffset = 0;
	fileBuffer->numBytes = 0;

	// Keep track of metrics
	fileBufferPool.numFileBufferInUse++;
	fileBufferPool.numFileBufferUsed++;

	return fileBuffer;
}

void ce97d170_releaseFileBuffer(FileBuffer *fileBuffer) {
	f106c0ab_push(&fileBufferPool.structStack, fileBuffer);

	// Keep track of metrics
	fileBufferPool.numFileBufferInUse--;
	fileBufferPool.numFileBufferFree++;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

FileBufferList *ce97d170_createFileBufferList() {
	FileBufferList *bufferList = f668c4bd_malloc(sizeof(FileBufferList));

	bufferList->values = f668c4bd_mallocArray(sizeof(FileBuffer*), FILEBUFFERLIST_DEFAULT_SIZE);
	bufferList->numBytes = 0;
	bufferList->fileOffset = 0;
	bufferList->length = 0;
	bufferList->size = FILEBUFFERLIST_DEFAULT_SIZE;

	return bufferList;
}

void ce97d170_destroyFileBufferList(FileBufferList *bufferList, void freeBuffer(void *buffer)) {
	FileBuffer *fileBuffer;

	// Free all of the FileBuffer objects first
	if (freeBuffer == NULL) {
		for (uint32_t i=0; i < bufferList->length; i++) {
			fileBuffer = bufferList->values[i];
			f668c4bd_free(fileBuffer);
		}
	} else {
		for (uint32_t i=0; i < bufferList->length; i++) {
			fileBuffer = bufferList->values[i];
			freeBuffer(fileBuffer->buffer);
			f668c4bd_free(fileBuffer);
		}
	}

	f668c4bd_free(bufferList->values);
	f668c4bd_free(bufferList);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void ce97d170_initFileBuffer(FileBuffer *fileBuffer, void *buffer) {
	f668c4bd_meminit(fileBuffer, sizeof(FileBuffer));
	fileBuffer->buffer = buffer;
}

void ce97d170_cleanUpFileBufferList(FileBufferList *bufferList, void freeBuffer(void *buffer)) {
	FileBuffer *fileBuffer;

	// Free all of the FileBuffer objects first
	if (freeBuffer == NULL) {
		for (uint32_t i=0; i < bufferList->length; i++) {
			fileBuffer = bufferList->values[i];
			ce97d170_releaseFileBuffer(fileBuffer);
		}
	} else {
		for (uint32_t i=0; i < bufferList->length; i++) {
			fileBuffer = bufferList->values[i];
			freeBuffer(fileBuffer->buffer);
			ce97d170_releaseFileBuffer(fileBuffer);
		}
	}

	f668c4bd_free(bufferList->values);
}

void ce97d170_initFileBufferList(FileBufferList *bufferList) {
	bufferList->values = f668c4bd_mallocArray(sizeof(FileBuffer*), FILEBUFFERLIST_DEFAULT_SIZE);
	bufferList->numBytes = 0;
	bufferList->fileOffset = 0;
	bufferList->length = 0;
	bufferList->size = FILEBUFFERLIST_DEFAULT_SIZE;
}

void ce97d170_resetFileBufferList(FileBufferList *bufferList, void freeBuffer(void *buffer)) {
	FileBuffer *fileBuffer;

	// Free all of the FileBuffer objects first
	if (freeBuffer == NULL) {
		for (uint32_t i=0; i < bufferList->length; i++) {
			fileBuffer = bufferList->values[i];
			ce97d170_releaseFileBuffer(fileBuffer);
		}
	} else {
		for (uint32_t i=0; i < bufferList->length; i++) {
			fileBuffer = bufferList->values[i];
			freeBuffer(fileBuffer->buffer);
			ce97d170_releaseFileBuffer(fileBuffer);
		}
	}

	bufferList->numBytes = 0;
	bufferList->fileOffset = 0;
	bufferList->length = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void ce97d170_addBuffer(FileBufferList *bufferList, FileBuffer *fileBuffer) {
	FileBuffer *prevFileBuffer;

	// Keep track of statistics for FileBufferList
	bufferList->numBytes += fileBuffer->numBytes;

	if (bufferList->length == 0) {
		bufferList->fileOffset = fileBuffer->fileOffset;
	} else {
		prevFileBuffer = bufferList->values[bufferList->length - 1];
		prevFileBuffer->next = fileBuffer;
	}

	if (bufferList->length == bufferList->size) {
		bufferList->size <<= 1;
		bufferList->values = f668c4bd_resizeArray(bufferList->values, bufferList->length, sizeof(FileBuffer*), bufferList->size);
	}

	bufferList->values[bufferList->length++] = fileBuffer;
}

FileBuffer *ce97d170_containsData(FileBufferList *bufferList, int64_t offset, uint32_t length) {
	FileBuffer *fileBuffer;
	int64_t bufferListEnd;
	int64_t dataEnd;

	if (bufferList->length > 0 && bufferList->fileOffset <= offset) {
		bufferListEnd = bufferList->fileOffset + bufferList->numBytes;
		dataEnd = offset + length;

		if (bufferListEnd >= dataEnd) {
			fileBuffer = bufferList->values[0];

			for (uint32_t i=1; i < bufferList->length; i++) {
				if (bufferList->values[i]->fileOffset <= offset) {
					fileBuffer = bufferList->values[i];
				} else {
					break;
				}
			}

			fileBuffer->dataOffset = offset - fileBuffer->fileOffset;

			return fileBuffer;
		}
	}

	return NULL;
}

uint32_t ce97d170_crc32(FileBuffer *fileBuffer, uint32_t length) {
	uint32_t bufferLength;
	void *bufferPtr;
	uint32_t crc32 = 0;

	while (length > 0) {
		bufferPtr = fileBuffer->buffer + fileBuffer->dataOffset;
		bufferLength = fileBuffer->numBytes - fileBuffer->dataOffset;
		bufferLength = (bufferLength > length) ? length : bufferLength;

		crc32 = b7e0468d_crc32(bufferPtr, bufferLength, crc32);

		length -= bufferLength;
		fileBuffer = fileBuffer->next;
	}

	return crc32;
}

FileBuffer *ce97d170_getBuffer(FileBufferList *bufferList, uint32_t index) {
	if (bufferList->length > index) {
		return bufferList->values[index];
	}

	return NULL;
}

FileBuffer *ce97d170_readFileBuffer(AIOFile *aioFile, uint64_t length) {
	FileBuffer *fileBuffer;
	AIOTicket *aioTicket;
	void *bufferPtr;

	aioTicket = &aioFile->aioTicket;

	if (length > MEMORY_PAGE_SIZE) {
		StringBuilder errorMessage;
		c598a24c_initStringBuilder(&errorMessage);

		c598a24c_append_string(&errorMessage, "Cannot read FileBuffer: length ");
		c598a24c_append_uint(&errorMessage, length);
		c598a24c_append_string(&errorMessage, " greater than PagePool page size of 4096");

		c7c88e52_printError_string(errorMessage.buffer);
		c598a24c_cleanUpStringBuilder(&errorMessage);

		return NULL;
	}

	// 1. Create FileBuffer
	bufferPtr = f502a409_acquirePage();
	fileBuffer = ce97d170_acquireFileBuffer(bufferPtr);
	fileBuffer->fileOffset = aioFile->offset;

	// 2. Initialize AIOTicket
	f1207515_initAIOTicket(aioTicket);

	// 3. Read file data
	f1207515_read(aioFile, bufferPtr, MEMORY_PAGE_SIZE);

	// 4. Submit the AIORequests
	f1207515_submit(aioFile);

	// 5. Retrieve the AIOEvents
	f1207515_getEvents(aioFile);

	// 6. Print the ticket
	// f1207515_printTicket(&aioTicket);

	// 7. Retrieve number of bytes read
	fileBuffer->numBytes = (int64_t) aioTicket->eventList[0].res;

	// 8. Clean up the AIOTicket
	f1207515_cleanUpAIOTicket(aioTicket);

	return fileBuffer;
}

void ce97d170_readFileBufferList(AIOFile *aioFile, FileBufferList *bufferList, int64_t length) {
	FileBuffer *fileBuffer;
	AIOTicket *aioTicket;
	uint32_t numBlocks;
	int32_t numEvents;
	void *bufferPtr;

	aioTicket = &aioFile->aioTicket;

	// Reset the FileBufferList if contains data
	if (bufferList->length > 0) {
		ce97d170_resetFileBufferList(bufferList, f502a409_releasePage);
	}

	if (aioFile->numRequestsRemaining > 0) {
		// Get the remaining events from the submitted requests
		numEvents = f1207515_getEvents(aioFile);

	} else {
		// Create and submit new AIORequests
		f1207515_initAIOTicket(aioTicket);

		// Calculate the number of blocks to read
		length = f45efac2_min_uint32(length, ASYNC_AIOTICKET_MAXSIZE);
		numBlocks = (length + 4095) >> 12;

		// Read file data
		for (uint32_t i=0; i < numBlocks; i++) {
			bufferPtr = f502a409_acquirePage();
			f1207515_read(aioFile, bufferPtr, MEMORY_PAGE_SIZE);
		}

		// Submit the AIORequests
		aioFile->numRequestsRemaining = numBlocks;
		f1207515_submit(aioFile);

		// Retrieve the AIOEvents
		numEvents = f1207515_getEvents(aioFile);
	}

	// Print the ticket
	aioFile->numRequestsRemaining -= numEvents;
//	f1207515_printTicket(aioTicket);

	// Create FileBuffer objects from the AIORequests
	for (int32_t i=0; i < numEvents; i++) {
		AIORequest *aioRequest;

		aioRequest = (AIORequest*)aioTicket->eventList[i].obj;
		bufferPtr = (void*) aioRequest->aio_buf;

		fileBuffer = ce97d170_acquireFileBuffer(bufferPtr);
		fileBuffer->numBytes = (int64_t) aioTicket->eventList[i].res;
		fileBuffer->fileOffset = (int64_t) aioRequest->aio_offset;

		ce97d170_addBuffer(bufferList, fileBuffer);
	}

	// Clean up the AIOTicket
	f1207515_cleanUpAIOTicket(aioTicket);
}

void ce97d170_write(FileBuffer *fileBuffer, int fd, uint32_t length, char *pathName) {
	uint32_t bufferLength;
	void *bufferPtr;

	while (length > 0) {
		bufferPtr = fileBuffer->buffer + fileBuffer->dataOffset;
		bufferLength = fileBuffer->numBytes - fileBuffer->dataOffset;
		bufferLength = (bufferLength > length) ? length : bufferLength;

		e2f74138_writeFile(fd, bufferPtr, bufferLength, pathName);

		length -= bufferLength;
		fileBuffer = fileBuffer->next;
	}
}
