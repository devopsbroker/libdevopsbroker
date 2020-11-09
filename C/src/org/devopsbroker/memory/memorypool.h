/*
 * memorypool.h - DevOpsBroker C header file for the org.devopsbroker.memory.MemoryPool struct
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
 * echo ORG_DEVOPSBROKER_MEMORY_MEMORYPOOL | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_MEMORY_MEMORYPOOL_H
#define ORG_DEVOPSBROKER_MEMORY_MEMORYPOOL_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

#include "../adt/stackarray.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct MemoryPool {
	StackArray pageStack;
	uint32_t   numPagesAlloc;
	uint32_t   numPageBytesUsed;
	uint32_t   numPageBytesFree;
	uint32_t   numBlocksAlloc;
} MemoryPool;

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(MemoryPool) == 32, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(MemoryPool) == 28, "Check your assumptions");
#endif

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b86b2c8d_destroyMemoryPool
 * Description: Frees the memory allocated to the internal MemoryPool
 *
 * Parameters:
 *   debug      True to print internal statistics, false otherwise
 * ----------------------------------------------------------------------------
 */
void b86b2c8d_destroyMemoryPool(bool debug);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f502a409_acquireMemory
 * Description: Acquires a block of memory of the specified size from the
 *              internal MemoryPool
 *
 * Parameters:
 *   size       The size of the memory block to allocate
 * Returns:     The memory if available, or NULL otherwise
 * ----------------------------------------------------------------------------
 */
void *f502a409_acquireMemory(uint32_t size);

#endif /* ORG_DEVOPSBROKER_MEMORY_MEMORYPOOL_H */
