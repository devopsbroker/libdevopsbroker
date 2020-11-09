/*
 * slabpool.h - DevOpsBroker C header file for the org.devopsbroker.memory.SlabPool struct
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
 * echo ORG_DEVOPSBROKER_MEMORY_SLABPOOL | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_MEMORY_SLABPOOL_H
#define ORG_DEVOPSBROKER_MEMORY_SLABPOOL_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

#include "../adt/stackarray.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define SLABPOOL_SLAB_SIZE  32768

// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct SlabPool {
	StackArray slabStack;
	uint32_t   numSlabsAlloc;
	uint32_t   numSlabsFree;
	uint32_t   numSlabsInUse;
	uint32_t   numSlabsUsed;
} SlabPool;

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(SlabPool) == 32, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(SlabPool) == 28, "Check your assumptions");
#endif

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b426145b_destroySlabPool
 * Description: Frees the memory allocated to the internal SlabPool
 *
 * Parameters:
 *   debug      True to print internal statistics, false otherwise
 * ----------------------------------------------------------------------------
 */
void b426145b_destroySlabPool(bool debug);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b426145b_acquireSlab
 * Description: Acquires a 32KB memory slab from the internal SlabPool
 *
 * Returns:     The slab if available, NULL otherwise
 * ----------------------------------------------------------------------------
 */
void *b426145b_acquireSlab();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b426145b_releaseSlab
 * Description: Releases a 32KB memory slab back into the internal SlabPool
 *
 * Parameters:
 *   slabPtr    The memory slab to return to the SlabPool
 * ----------------------------------------------------------------------------
 */
void b426145b_releaseSlab(void *slabPtr);

#endif /* ORG_DEVOPSBROKER_MEMORY_SLABPOOL_H */
