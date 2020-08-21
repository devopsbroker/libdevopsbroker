/*
 * pagepool.h - DevOpsBroker C header file for the org.devopsbroker.memory.PagePool struct
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
 * echo ORG_DEVOPSBROKER_MEMORY_PAGEPOOL | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_MEMORY_PAGEPOOL_H
#define ORG_DEVOPSBROKER_MEMORY_PAGEPOOL_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>
#include <stdbool.h>

#include <assert.h>

#include "../adt/listarray.h"
#include "../adt/stackarray.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct PagePool {
	StackArray pageStack;
	ListArray  slabList;
	uint32_t   numPagesAlloc;
	uint32_t   numPagesFree;
	uint32_t   numPagesInUse;
	uint32_t   numPagesUsed;
} PagePool;

static_assert(sizeof(PagePool) == 48, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f502a409_destroyPagePool
 * Description: Frees the memory allocated to the internal PagePool
 *
 * Parameters:
 *   debug      True to print internal statistics, false otherwise
 * ----------------------------------------------------------------------------
 */
void f502a409_destroyPagePool(bool debug);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f502a409_acquirePage
 * Description: Acquires a 4096-byte memory page from the internal PagePool
 *
 * Returns:     The page if available, NULL otherwise
 * ----------------------------------------------------------------------------
 */
void *f502a409_acquirePage();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f502a409_releasePage
 * Description: Releases a 4096-byte memory page back into the internal PagePool
 *
 * Parameters:
 *   pagePtr    The memory page to return to the PagePool
 * ----------------------------------------------------------------------------
 */
void f502a409_releasePage(void *pagePtr);

#endif /* ORG_DEVOPSBROKER_MEMORY_PAGEPOOL_H */
