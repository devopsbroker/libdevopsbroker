/*
 * slabpool.c - DevOpsBroker C source file for the org.devopsbroker.memory.SlabPool struct
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
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>
#include <stdio.h>

#include "slabpool.h"

#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════

SlabPool slabPool = { {NULL, 0, 0}, 0, 0, 0, 0 };

// ════════════════════════════ Function Prototypes ═══════════════════════════

static void populateSlabPool();

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

void b426145b_destroySlabPool(bool debug) {
	if (debug) {
		puts("SlabPool Statistics:");
		printf("\tNumber of Slabs Allocated: %u\n", slabPool.numSlabsAlloc);
		printf("\tNumber of Slabs Free:      %u\n", slabPool.numSlabsFree);
		printf("\tNumber of Slabs In Use     %u\n", slabPool.numSlabsInUse);
		printf("\tNumber of Slabs Used:      %u\n", slabPool.numSlabsUsed);
		printf("\n");
	}

	// Clean up the page stack and slab list
	f106c0ab_cleanUpStackArray(&slabPool.slabStack, f668c4bd_free);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Slab *b426145b_acquireSlab() {
	Slab *slabPtr;

	// Initialize the slab stack if no slabs have been allocated yet
	if (slabPool.numSlabsAlloc == 0) {
		f106c0ab_initStackArray(&slabPool.slabStack);
	}

	// Populate the SlabPool with another slab if no pages are free
	if (slabPool.numSlabsFree == 0) {
		populateSlabPool();
	}

	// Keep track of SlabPool statistics
	slabPool.numSlabsFree--;
	slabPool.numSlabsInUse++;
	slabPool.numSlabsUsed++;

	// Pop the Slab off the stack and return
	slabPtr = f106c0ab_pop(&slabPool.slabStack);
	slabPtr->next = NULL;

	return slabPtr;
}

void b426145b_releaseSlab(void *slabPtr) {
	// Keep track of SlabPool statistics
	slabPool.numSlabsFree++;
	slabPool.numSlabsInUse--;

	f106c0ab_push(&slabPool.slabStack, slabPtr);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static void populateSlabPool() {
	// Allocate a 32KB slab aligned on 4KB page boundary
	Slab *slab = f668c4bd_malloc(sizeof(Slab));
	slab->buffer = f668c4bd_alignedAlloc(MEMORY_PAGE_SIZE, SLABPOOL_SLAB_SIZE);

	// Add slab to the slab stack and keep SlabPool statistics
	f106c0ab_push(&slabPool.slabStack, slab);
	slabPool.numSlabsAlloc++;
	slabPool.numSlabsFree++;
}
