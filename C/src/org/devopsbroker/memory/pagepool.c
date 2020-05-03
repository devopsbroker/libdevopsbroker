/*
 * pagepool.c - DevOpsBroker C source file for the org.devopsbroker.memory.PagePool struct
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

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "pagepool.h"

#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define PAGEPOOL_SLAB_SIZE  32768

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════

PagePool pagePool = { {NULL, 0, 0}, {NULL, 0, 0}, 0, 0, 0, 0 };

// ════════════════════════════ Function Prototypes ═══════════════════════════

static void populatePagePool();

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

void f502a409_destroyPagePool(bool debug) {
	void *slabPtr;

	if (debug) {
		puts("PagePool Statistics:");
		printf("\tNumber of Slabs Allocated: %u\n", pagePool.numSlabsAlloc);
		printf("\tNumber of Pages Allocated: %u\n", pagePool.numPagesAlloc);
		printf("\tNumber of Pages Free:      %u\n", pagePool.numPagesFree);
		printf("\tNumber of Pages Used:      %u\n", pagePool.numPagesUsed);
	}

	// Free slabs from the slab list
	for (uint32_t i=0; i < pagePool.slabList.length; i++) {
		slabPtr = b196167f_get(&pagePool.slabList, i);
		f668c4bd_free(slabPtr);
	}

	// Clean up the page stack and slab list
	f106c0ab_cleanUpStackArray(&pagePool.pageStack);
	b196167f_cleanUpListArray(&pagePool.slabList);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void *f502a409_acquirePage() {
	// Initialize the page stack and slab list if no slabs have been allocated yet
	if (pagePool.numSlabsAlloc == 0) {
		f106c0ab_initStackArray(&pagePool.pageStack);
		b196167f_initListArray(&pagePool.slabList);
	}

	// Populate the PagePool with another slab if no pages are free
	if (pagePool.numPagesFree == 0) {
		populatePagePool();
	}

	// Keep track of how many pages are free versus used
	pagePool.numPagesFree--;
	pagePool.numPagesUsed++;

	return f106c0ab_pop(&pagePool.pageStack);
}

void f502a409_releasePage(void *pagePtr) {
	// Keep track of how many pages are free versus used
	pagePool.numPagesFree++;
	pagePool.numPagesUsed--;

	f106c0ab_push(&pagePool.pageStack, pagePtr);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static void populatePagePool() {
	// Allocate a 32KB slab aligned on 4KB page boundary
	void *slabPtr = f668c4bd_alignedAlloc(PAGEPOOL_PAGE_SIZE, PAGEPOOL_SLAB_SIZE);

	// Add slab to the slab list
	b196167f_add(&pagePool.slabList, slabPtr);
	pagePool.numSlabsAlloc++;

	// Add pages to the page stack
	for (int i=0; i < 8; i++) {
		f106c0ab_push(&pagePool.pageStack, slabPtr);
		slabPtr += PAGEPOOL_PAGE_SIZE;
	}

	pagePool.numPagesAlloc += 8;
	pagePool.numPagesFree += 8;
}
