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
#include "../lang/memory.h"
#include "../lang/stringbuilder.h"
#include "../memory/pagepool.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define FILEBUFFERLIST_DEFAULT_SIZE 8

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

FileBuffer *ce97d170_createFileBuffer(void *buffer) {
	FileBuffer *fileBuffer = f668c4bd_malloc(sizeof(FileBuffer));

	f668c4bd_meminit(fileBuffer, sizeof(FileBuffer));
	fileBuffer->buffer = buffer;

	return fileBuffer;
}

void ce97d170_destroyFileBuffer(FileBuffer *fileBuffer) {
	f668c4bd_free(fileBuffer);
}

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
			f668c4bd_free(fileBuffer);
		}
	} else {
		for (uint32_t i=0; i < bufferList->length; i++) {
			fileBuffer = bufferList->values[i];
			freeBuffer(fileBuffer->buffer);
			f668c4bd_free(fileBuffer);
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

FileBuffer *ce97d170_readFileBuffer(AIOFile *aioFile, uint32_t length) {
	AIOTicket aioTicket;
	FileBuffer *fileBuffer;
	void *bufferPtr;

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

	f1207515_initAIOTicket(&aioTicket);

	// 1. Read file data
	bufferPtr = f502a409_acquirePage();
	f1207515_read(aioFile, bufferPtr, MEMORY_PAGE_SIZE);

	// 2. Submit the AIORequests
	f1207515_submit(aioFile->aioContext, &aioTicket);

	// 3. Retrieve the AIOEvents
	f1207515_getEvents(aioFile->aioContext, &aioTicket);

	// 4. Print the ticket
	// f1207515_printTicket(&aioTicket);

	// 5. Create FileBuffer object from the AIORequest
	fileBuffer = ce97d170_createFileBuffer(bufferPtr);
	fileBuffer->numBytes = (int64_t) aioTicket.eventList[0].res;
	fileBuffer->fileOffset = aioFile->offset;

	// 6. Clean up the AIOTicket
	f1207515_cleanUpAIOTicket(&aioTicket);

	return fileBuffer;
}

void ce97d170_readFileBufferList(AIOFile *aioFile, FileBufferList *bufferList, uint32_t length) {
	AIOTicket aioTicket;
	FileBuffer *fileBuffer;
	uint32_t numBlocks;
	void *bufferPtr;
	int64_t offset;

	f1207515_initAIOTicket(&aioTicket);

	numBlocks = (length + 4095) >> 12;

	// 1. Read file data
	offset = aioFile->offset;
	for (uint32_t i=0; i < numBlocks; i++) {
		bufferPtr = f502a409_acquirePage();
		f1207515_read(aioFile, bufferPtr, MEMORY_PAGE_SIZE);
		aioFile->offset += MEMORY_PAGE_SIZE;
	}
	aioFile->offset = offset;

	while (aioFile->aioContext->requestQueue->length > 0) {
		// 2. Submit the AIORequests
		f1207515_submit(aioFile->aioContext, &aioTicket);

		// 3. Retrieve the AIOEvents
		f1207515_getEvents(aioFile->aioContext, &aioTicket);

		// 4. Print the ticket
		// f1207515_printTicket(&aioTicket);

		// 5. Create FileBuffer objects from the AIORequests
		for (uint32_t i=0; i < aioTicket.numEvents; i++) {
			bufferPtr = (void*) aioTicket.requestList[i]->aio_buf;

			fileBuffer = ce97d170_createFileBuffer(bufferPtr);
			fileBuffer->numBytes = (int64_t) aioTicket.eventList[i].res;
			fileBuffer->fileOffset = (int64_t) aioTicket.requestList[i]->aio_offset;

			ce97d170_addBuffer(bufferList, fileBuffer);
		}
	}

	// 6. Clean up the AIOTicket
	f1207515_cleanUpAIOTicket(&aioTicket);
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
