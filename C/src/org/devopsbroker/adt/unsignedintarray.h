/*
 * unsignedintarray.h - DevOpsBroker C header file for the org.devopsbroker.adt.UnsignedIntArray struct
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
 * echo ORG_DEVOPSBROKER_ADT_UNSIGNEDINTARRAY | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_ADT_UNSIGNEDINTARRAY_H
#define ORG_DEVOPSBROKER_ADT_UNSIGNEDINTARRAY_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

#include "../lang/stringbuilder.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct UnsignedIntArray {
	uint32_t *values;
	uint32_t length;
	uint32_t size;
} UnsignedIntArray __attribute__ ((aligned (16)));

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(UnsignedIntArray) == 16, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(UnsignedIntArray) == 12, "Check your assumptions");
#endif

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a8638224_createUnsignedIntArray
 * Description: Creates a UnsignedIntArray struct instance
 *
 * Returns:     A UnsignedIntArray struct instance
 * ----------------------------------------------------------------------------
 */
UnsignedIntArray *a8638224_createUnsignedIntArray();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a8638224_destroyUnsignedIntArray
 * Description: Frees the memory allocated to the UnsignedIntArray struct pointer
 *
 * Parameters:
 *   listArray  A pointer to the UnsignedIntArray instance to destroy
 * ----------------------------------------------------------------------------
 */
void a8638224_destroyUnsignedIntArray(UnsignedIntArray *listArray);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a8638224_cleanUpUnsignedIntArray
 * Description: Frees dynamically allocated memory within the UnsignedIntArray instance
 *
 * Parameters:
 *   listArray  A pointer to the UnsignedIntArray instance to clean up
 * ----------------------------------------------------------------------------
 */
void a8638224_cleanUpUnsignedIntArray(UnsignedIntArray *listArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a8638224_initUnsignedIntArray
 * Description: Initializes an existing UnsignedIntArray struct
 *
 * Parameters:
 *   listArray  A pointer to the UnsignedIntArray instance to initalize
 * ----------------------------------------------------------------------------
 */
void a8638224_initUnsignedIntArray(UnsignedIntArray *listArray);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a8638224_add
 * Description: Adds an element to the UnsignedIntArray instance
 *
 * Parameters:
 *   listArray  The UnsignedIntArray instance
 *   element    The unsigned int element to add to the UnsignedIntArray
 * ----------------------------------------------------------------------------
 */
void a8638224_add(UnsignedIntArray *listArray, uint32_t element);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    a8638224_extract
 * Description: Extracts an UnsignedIntArray instance into a StringBuilder instance
 *
 * Parameters:
 *   listArray  The UnsignedIntArray instance to extract
 *   strBuilder The StringBuilder instance to populate
 * ----------------------------------------------------------------------------
 */
void a8638224_extract(UnsignedIntArray *listArray, StringBuilder *strBuilder);

#endif /* ORG_DEVOPSBROKER_ADT_UNSIGNEDINTARRAY_H */
