/*
 * async.c - DevOpsBroker C source file for Linux AIO functionality
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-40
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _GNU_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include <unistd.h>

#include <sys/syscall.h>

#include "async.h"

#include "../adt/stackarray.h"
#include "../io/file.h"
#include "../lang/error.h"
#include "../lang/integer.h"
#include "../lang/stringbuilder.h"
#include "../memory/memorypool.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define ASYNC_MAX_REQUEST_QUEUE_CAPACITY  2048
#define ASYNC_TIMEOUT_NSEC  6000000

// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct AIORequestPool {
	StackArray structStack;
	uint32_t   numAIORequestAlloc;
	uint32_t   numAIORequestFree;
	uint32_t   numAIORequestInUse;
	uint32_t   numAIORequestUsed;
} AIORequestPool;

static_assert(sizeof(AIORequestPool) == 32, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════

AIORequestPool aioRequestPool = { {NULL, 0, 0}, 0, 0, 0, 0 };

// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Acquire/Release Functions ~~~~~~~~~~~~~~~~~~~~~~~~

AIORequest *f1207515_acquireAIORequest() {
	AIORequest *aioRequest;

	// Lazy-initialize FileBufferPool
	if (aioRequestPool.structStack.values == NULL) {
		f106c0ab_initStackArray(&aioRequestPool.structStack);
	}

	if (aioRequestPool.structStack.length == 0) {
		// Acquire new FileBuffer from the memory pool
		aioRequest = f502a409_acquireMemory(sizeof(AIORequest));
		aioRequestPool.numAIORequestAlloc++;
	} else {
		aioRequest = f106c0ab_pop(&aioRequestPool.structStack);
		aioRequestPool.numAIORequestFree--;
	}

	// Keep track of metrics
	aioRequestPool.numAIORequestInUse++;
	aioRequestPool.numAIORequestUsed++;

	return aioRequest;
}

void f1207515_releaseAIORequest(AIORequest *aioRequest) {
	f106c0ab_push(&aioRequestPool.structStack, aioRequest);

	// Keep track of metrics
	aioRequestPool.numAIORequestInUse--;
	aioRequestPool.numAIORequestFree++;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

AIOContext *f1207515_createAIOContext(uint32_t maxOperations) {
	AIOContext *aioContext;
	aio_context_t aioContextId;
	long retValue;

	// Initialize aioContextId to 0 prior to calling io_setup()
	aioContextId = 0;

	// Define maximum operations as the minimum of parameter and ASYNC_MAX_REQUEST_QUEUE_CAPACITY
	maxOperations = f45efac2_min_uint32(maxOperations, ASYNC_MAX_REQUEST_QUEUE_CAPACITY);

	// Call io_setup(unsigned nr_events, aio_context_t *ctx_idp)
	retValue = syscall(__NR_io_setup, maxOperations, &aioContextId);

	if (retValue != 0) {
		errno = -retValue;
		return NULL;
	}

	aioContext = f668c4bd_malloc(sizeof(AIOContext));
	f668c4bd_meminit(aioContext, sizeof(AIOContext));

	aioContext->id = aioContextId;
	aioContext->requestQueue = b8da7268_createQueueBounded(maxOperations);
	aioContext->timeout.tv_sec = 0;
	aioContext->timeout.tv_nsec = ASYNC_TIMEOUT_NSEC;
	aioContext->maxOperations = maxOperations;

	return aioContext;
}

int f1207515_destroyAIOContext(AIOContext *aioContext) {
	long retValue;

	// Call io_destroy(aio_context_t ctx_id)
	retValue = syscall(__NR_io_destroy, aioContext->id);

	if (retValue < 0) {
		errno = -retValue;
		return SYSTEM_ERROR_CODE;
	}

	b8da7268_destroyQueueBounded(aioContext->requestQueue);
	f668c4bd_free(aioContext);

	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

int f1207515_cleanUpAIOContext(AIOContext *aioContext) {
	long retValue;

	// Call io_destroy(aio_context_t ctx_id)
	retValue = syscall(__NR_io_destroy, aioContext->id);

	if (retValue < 0) {
		errno = -retValue;
		return SYSTEM_ERROR_CODE;
	}

	b8da7268_destroyQueueBounded(aioContext->requestQueue);

	return 0;
}

int f1207515_initAIOContext(AIOContext *aioContext, uint32_t maxOperations) {
	aio_context_t aioContextId;
	long retValue;

	// Initialize aioContextId to 0 prior to calling io_setup()
	aioContextId = 0;

	// Define maximum operations as the minimum of parameter and ASYNC_MAX_REQUEST_QUEUE_CAPACITY
	maxOperations = f45efac2_min_uint32(maxOperations, ASYNC_MAX_REQUEST_QUEUE_CAPACITY);

	// Call io_setup(unsigned nr_events, aio_context_t *ctx_idp)
	retValue = syscall(__NR_io_setup, maxOperations, &aioContextId);

	if (retValue != 0) {
		errno = -retValue;
		return SYSTEM_ERROR_CODE;
	}

	f668c4bd_meminit(aioContext, sizeof(AIOContext));

	aioContext->id = aioContextId;
	aioContext->requestQueue = b8da7268_createQueueBounded(maxOperations);
	aioContext->timeout.tv_sec = 0;
	aioContext->timeout.tv_nsec = ASYNC_TIMEOUT_NSEC;
	aioContext->maxOperations = maxOperations;

	return 0;
}

void f1207515_cleanUpAIOFile(AIOFile *aioFile) {
	// Close the open file descriptor
	e2f74138_closeFile(aioFile->fd, aioFile->fileName);
}

void f1207515_initAIOFile(AIOContext *aioContext, AIOFile *aioFile, char *fileName) {
	// Initialize the AIOFile
	f668c4bd_meminit(aioFile, sizeof(AIOFile));

	// Set the AIOContext and fileName
	aioFile->aioContext = aioContext;
	aioFile->fileName = fileName;
}

void f1207515_cleanUpAIOTicket(AIOTicket *aioTicket) {
	if (aioTicket->numEvents == aioTicket->numRequests) {
		for (uint32_t i=0; i < aioTicket->numRequests; i++) {
			f1207515_releaseAIORequest(aioTicket->requestList[i]);
		}
	}
}

void f1207515_initAIOTicket(AIOTicket *aioTicket) {
	aioTicket->numRequests = 0;
	aioTicket->numEvents = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int f1207515_create(AIOFile *aioFile, FileAccessMode aMode, int flags, uint32_t mode) {
	if (aMode == FOPEN_READONLY) {
		StringBuilder errorMessage;
		c598a24c_initStringBuilder(&errorMessage);

		c598a24c_append_string(&errorMessage, "Cannot create AIO file '");
		c598a24c_append_string(&errorMessage, aioFile->fileName);
		c598a24c_append_string(&errorMessage, "': read-only File Access Mode specified");

		c7c88e52_printError_string(errorMessage.buffer);
		c598a24c_cleanUpStringBuilder(&errorMessage);

		return SYSTEM_ERROR_CODE;
	}

	aioFile->fd = open(aioFile->fileName, O_DIRECT|O_CREAT|aMode|flags, mode);

	return aioFile->fd;
}

int f1207515_open(AIOFile *aioFile, FileAccessMode aMode, int flags) {
	FileStatus fileStatus;

	aioFile->fd = open(aioFile->fileName, aMode | O_DIRECT | flags);
	e2f74138_getDescriptorStatus(aioFile->fd, &fileStatus);
	aioFile->fileSize = fileStatus.st_size;
	aioFile->offset = 0;

	return aioFile->fd;
}

AIORequest *f1207515_read(AIOFile *aioFile, void *buf, size_t bufSize) {
	AIORequest *aioReadRequest;
	AIOContext *aioContext;

	aioContext = aioFile->aioContext;

	if (b8da7268_isFull(aioContext->requestQueue)) {
		return NULL;
	}

	aioReadRequest = f1207515_acquireAIORequest();

	aioReadRequest->aio_data = 0;
	aioReadRequest->aio_fildes = aioFile->fd;
	aioReadRequest->aio_lio_opcode = AIO_READ;
	aioReadRequest->aio_reqprio = 0;
	aioReadRequest->aio_buf = (uint64_t) buf;
	aioReadRequest->aio_nbytes = bufSize;
	aioReadRequest->aio_offset = aioFile->offset;
	aioReadRequest->aio_flags = 0;
	aioReadRequest->aio_resfd = 0;
	aioFile->offset += bufSize;

	// Keep track of some metrics
	aioContext->numRequests++;
	aioContext->numReadRequests++;

	// Queue the request in the AIOContext
	b8da7268_enqueue(aioContext->requestQueue, aioReadRequest);

	return aioReadRequest;
}

AIORequest *f1207515_write(AIOFile *aioFile, void *buf, size_t count) {
	AIORequest *aioWriteRequest;
	AIOContext *aioContext;

	aioContext = aioFile->aioContext;

	if (b8da7268_isFull(aioContext->requestQueue)) {
		return NULL;
	}

	aioWriteRequest = f1207515_acquireAIORequest();

	aioWriteRequest->aio_data = 0;
	aioWriteRequest->aio_fildes = aioFile->fd;
	aioWriteRequest->aio_lio_opcode = AIO_WRITE;
	aioWriteRequest->aio_reqprio = 0;
	aioWriteRequest->aio_buf = (uint64_t) buf;
	aioWriteRequest->aio_nbytes = count;
	aioWriteRequest->aio_offset = aioFile->offset;
	aioWriteRequest->aio_flags = 0;
	aioWriteRequest->aio_resfd = 0;
	aioFile->offset += count;

	// Keep track of some metrics
	aioContext->numRequests++;
	aioContext->numWriteRequests++;

	// Queue the request in the AIOContext
	b8da7268_enqueue(aioContext->requestQueue, aioWriteRequest);

	return aioWriteRequest;
}

bool f1207515_submit(AIOFile *aioFile) {
	AIORequest *aioRequest;
	AIOContext *aioContext;
	AIOTicket *aioTicket;
	int numRequests = 0;
	long retValue;

	aioContext = aioFile->aioContext;
	aioTicket = &aioFile->aioTicket;

	// Nothing to submit if the request queue is empty
	if (b8da7268_isEmpty(aioContext->requestQueue)) {
		return false;
	}

	// Add up to eight AIORequest objects to the AIOTicket
	while (!b8da7268_isEmpty(aioContext->requestQueue) && numRequests < 8) {
		aioRequest = b8da7268_dequeue(aioContext->requestQueue);
		aioTicket->requestList[numRequests] = aioRequest;
		numRequests++;
	}

	aioTicket->numRequests = numRequests;

	retValue = syscall(__NR_io_submit, aioContext->id, numRequests, aioTicket->requestList);

	if (retValue < 0) {
		errno = -retValue;
		return false;
	}

	return true;
}

int32_t f1207515_getEvents(AIOFile *aioFile) {
	AIORequest *aioRequest;
	AIOContext *aioContext;
	AIOTicket *aioTicket;
	long retValue;

	aioContext = aioFile->aioContext;
	aioTicket = &aioFile->aioTicket;

	retValue = syscall(__NR_io_getevents, aioContext->id, 1, aioTicket->numRequests,
		    aioTicket->eventList, &aioContext->timeout);

	if (retValue < 0) {
		errno = -retValue;
		return SYSTEM_ERROR_CODE;
	}

	// Keep track of some metrics
	aioTicket->numEvents += retValue;

	for (uint32_t i=0; i < retValue; i++) {
		aioRequest = (AIORequest*) aioTicket->eventList[i].obj;

		if (aioRequest->aio_lio_opcode == AIO_READ) {
			aioContext->numBytesRead += aioTicket->eventList[i].res;
			aioTicket->numBytesRead += aioTicket->eventList[i].res;
		} else if (aioRequest->aio_lio_opcode == AIO_WRITE) {
			aioContext->numBytesWrite += aioTicket->eventList[i].res;
			aioTicket->numBytesWrite += aioTicket->eventList[i].res;
		}
	}

	return retValue;
}

void f1207515_printContext(AIOContext *aioContext) {
	printf("AIOContext ID: %lu\n", aioContext->id);
	printf("\tMax Operations:     %u\n", aioContext->maxOperations);
	printf("\tTotal Num Requests: %u\n", aioContext->numRequests);
	printf("\tNum Read Requests:  %u\n", aioContext->numReadRequests);
	printf("\tNum Bytes Read:     %ld bytes\n", aioContext->numBytesRead);
	printf("\tNum Write Requests: %u\n", aioContext->numWriteRequests);
	printf("\tNum Bytes Written:  %ld bytes\n", aioContext->numBytesWrite);
	printf("\n");
}

void f1207515_printTicket(AIOTicket *aioTicket) {
	AIORequest *requestPtr;

	printf("Number of AIORequests: %u\n", aioTicket->numRequests);

	for (uint32_t i=0; i < aioTicket->numRequests; i++) {
		requestPtr = aioTicket->requestList[i];

		printf("\nAIORequest #%u:\n", i);
		printf("\tpointer:        %p\n", requestPtr);
		printf("\taio_data:       %lu\n", (uint64_t) aioTicket->requestList[i]->aio_data);
		printf("\taio_lio_opcode: %u\n", (uint16_t) aioTicket->requestList[i]->aio_lio_opcode);
		printf("\taio_reqprio:    %d\n", (int16_t) aioTicket->requestList[i]->aio_reqprio);
		printf("\taio_fildes:     %u\n", (uint32_t) aioTicket->requestList[i]->aio_fildes);
		printf("\taio_buf:        %p\n", (void*) aioTicket->requestList[i]->aio_buf);
		printf("\taio_nbytes:     %lu\n", (uint64_t) aioTicket->requestList[i]->aio_nbytes);
		printf("\taio_offset:     %ld\n", (int64_t) aioTicket->requestList[i]->aio_offset);
		printf("\taio_flags:      %u\n", (uint32_t) aioTicket->requestList[i]->aio_flags);
	}

	printf("\nNumber of AIOEvents:   %u\n", aioTicket->numEvents);
	printf("\tNum Bytes Read:     %ld bytes\n", aioTicket->numBytesRead);
	printf("\tNum Bytes Written:  %ld bytes\n", aioTicket->numBytesWrite);

	for (uint32_t i=0; i < aioTicket->numEvents; i++) {
		printf("\nAIOEvent #%u:\n", i);
		printf("\tdata:           %lu\n", (uint64_t) aioTicket->eventList[i].data);
		printf("\tobj:            %p\n", (void*) aioTicket->eventList[i].obj);
		printf("\tres:            %ld\n", (int64_t) aioTicket->eventList[i].res);
		printf("\tres2:           %ld\n", (int64_t)aioTicket->eventList[i].res2);
	}

	printf("\n");
}
