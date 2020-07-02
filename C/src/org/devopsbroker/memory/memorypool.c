/*
 * memorypool.c - DevOpsBroker C source file for the org.devopsbroker.memory.MemoryPool struct
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
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-61
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "memorypool.h"
#include "pagepool.h"

#include "../lang/error.h"
#include "../lang/memory.h"
#include "../lang/stringbuilder.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════

MemoryPool memoryPool = { {NULL, 0, 0}, 0, 0, 0, 0 };

// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

void b86b2c8d_destroyMemoryPool(bool debug) {
	if (debug) {
		puts("MemoryPool Statistics:");
		printf("\tNumber of Pages Allocated:  %u\n", memoryPool.numPagesAlloc);
		printf("\tNumber of Page Bytes Used:  %u\n", memoryPool.numPageBytesUsed);
		printf("\tNumber of Page Bytes Free:  %u\n", memoryPool.numPageBytesFree);
		printf("\tNumber of Blocks Allocated: %u\n", memoryPool.numBlocksAlloc);
		printf("\n");
	}

	// Clean up the page stack and slab list
	f106c0ab_cleanUpStackArray(&memoryPool.pageStack, f502a409_releasePage);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void *f502a409_acquireMemory(uint32_t size) {
	void *block;

	// Initialize the page stack and slab list if no slabs have been allocated yet
	if (memoryPool.pageStack.values == NULL) {
		f106c0ab_initStackArray(&memoryPool.pageStack);
	}

	// Check to ensure the requested size is less than or equal to the page size
	if (size > MEMORY_PAGE_SIZE) {
		StringBuilder errorMessage;

		c598a24c_initStringBuilder(&errorMessage);
		c598a24c_append_string(&errorMessage, "f502a409_acquireMemory(): Invalid requested size of ");
		c598a24c_append_uint(&errorMessage, size);

		c7c88e52_printError_string(errorMessage.buffer);
		c598a24c_cleanUpStringBuilder(&errorMessage);
		return NULL;
	}

	// Populate the PagePool with another slab if no pages are free
	if (memoryPool.numPageBytesFree < size) {
		block = f502a409_acquirePage();
		f106c0ab_push(&memoryPool.pageStack, block);

		memoryPool.numPagesAlloc++;
		memoryPool.numPageBytesUsed = 0;
		memoryPool.numPageBytesFree = MEMORY_PAGE_SIZE;
	} else {
		block = f106c0ab_peek(&memoryPool.pageStack);
	}

	// Calculate starting position of memory block
	block += memoryPool.numPageBytesUsed;

	// Keep track of metrics
	memoryPool.numPageBytesUsed += size;
	memoryPool.numPageBytesFree -= size;
	memoryPool.numBlocksAlloc++;

	return block;
}
