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

#include "../lang/error.h"
#include "../lang/integer.h"
#include "../lang/stringbuilder.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

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

AIOTicket *f1207515_createAIOTicket() {
	AIOTicket *aioTicket;

	aioTicket = f668c4bd_malloc(sizeof(AIOTicket));
	f668c4bd_meminit(aioTicket, sizeof(AIOTicket));

	return aioTicket;
}

void f1207515_destroyAIOTicket(AIOTicket *aioTicket) {
	// Free all the AIORequest objects
	for (uint32_t i=0; i < aioTicket->numRequests; i++) {
		f668c4bd_free(aioTicket->requestList[i]);
	}

	f668c4bd_free(aioTicket);
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

void f1207515_initAIOFile(AIOFile *aioFile, AIOContext *aioContext, char *fileName) {
	f668c4bd_meminit(aioFile, sizeof(AIOFile));

	aioFile->aioContext = aioContext;
	aioFile->fileName = fileName;
}

void f1207515_cleanUpAIOTicket(AIOTicket *aioTicket) {
	// Free all the AIORequest objects
	for (uint32_t i=0; i < aioTicket->numRequests; i++) {
		f668c4bd_free(aioTicket->requestList[i]);
	}
}

void f1207515_initAIOTicket(AIOTicket *aioTicket) {
	f668c4bd_meminit(aioTicket, sizeof(AIOTicket));
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
	aioFile->fd = open(aioFile->fileName, aMode | O_DIRECT | flags);

	return aioFile->fd;
}

bool f1207515_read(AIOFile *aioFile, void *buf, size_t bufSize) {
	AIORequest *aioReadRequest;

	if (b8da7268_isFull(aioFile->aioContext->requestQueue)) {
		return false;
	}

	aioReadRequest = f668c4bd_malloc(sizeof(AIORequest));
	f668c4bd_meminit(aioReadRequest, sizeof(AIORequest));

	aioReadRequest->aio_fildes = aioFile->fd;
	aioReadRequest->aio_lio_opcode = AIO_READ;
	aioReadRequest->aio_buf = (uint64_t) buf;
	aioReadRequest->aio_nbytes = bufSize;
	aioReadRequest->aio_offset = aioFile->offset;

	// Keep track of some metrics
	aioFile->aioContext->numRequests++;
	aioFile->aioContext->numReadRequests++;

	return b8da7268_enqueue(aioFile->aioContext->requestQueue, aioReadRequest);
}

bool f1207515_write(AIOFile *aioFile, void *buf, size_t count) {
	AIORequest *aioWriteRequest;

	if (b8da7268_isFull(aioFile->aioContext->requestQueue)) {
		return false;
	}

	aioWriteRequest = f668c4bd_malloc(sizeof(AIORequest));
	f668c4bd_meminit(aioWriteRequest, sizeof(AIORequest));

	aioWriteRequest->aio_fildes = aioFile->fd;
	aioWriteRequest->aio_lio_opcode = AIO_WRITE;
	aioWriteRequest->aio_buf = (uint64_t) buf;
	aioWriteRequest->aio_nbytes = count;
	aioWriteRequest->aio_offset = aioFile->offset;

	// Keep track of some metrics
	aioFile->aioContext->numRequests++;
	aioFile->aioContext->numWriteRequests++;

	return b8da7268_enqueue(aioFile->aioContext->requestQueue, aioWriteRequest);
}

bool f1207515_submit(AIOContext *aioContext, AIOTicket *aioTicket) {
	AIORequest *aioRequest;
	int numRequests = 0;
	long retValue;

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

	aioTicket->id = aioContext->id;
	aioTicket->numRequests = numRequests;

	retValue = syscall(__NR_io_submit, aioContext->id, numRequests, aioTicket->requestList);

	if (retValue < 0) {
		errno = -retValue;
		return false;
	}

	return true;
}

bool f1207515_getEvents(AIOContext *aioContext, AIOTicket *aioTicket) {
	AIORequest *aioRequest;
	uint32_t numEvents = 0;
	long retValue;

	while (numEvents < aioTicket->numRequests) {
		retValue = syscall(__NR_io_getevents, aioTicket->id, 1,
			aioTicket->numRequests - numEvents, aioTicket->eventList,
			&aioContext->timeout);

		if (retValue < 0) {
			errno = -retValue;
			return false;
		}

		numEvents += retValue;
	}

	// Keep track of some metrics
	aioTicket->numEvents = numEvents;

	for (uint32_t i=0; i < numEvents; i++) {
		aioRequest = (AIORequest*) aioTicket->eventList[i].obj;

		if (aioRequest->aio_lio_opcode == AIO_READ) {
			aioContext->numBytesRead += aioTicket->eventList[i].res;
			aioTicket->numBytesRead += aioTicket->eventList[i].res;
		} else if (aioRequest->aio_lio_opcode == AIO_WRITE) {
			aioContext->numBytesWrite += aioTicket->eventList[i].res;
			aioTicket->numBytesWrite += aioTicket->eventList[i].res;
		}
	}

	return true;
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
