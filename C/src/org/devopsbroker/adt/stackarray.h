/*
 * stackarray.h - DevOpsBroker C header file for the org.devopsbroker.adt.StackArray struct
 *
 * Copyright (C) 2019 Edward Smith <edwardsmith@devopsbroker.org>
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
 * Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-25
 *
 * echo ORG_DEVOPSBROKER_ADT_STACKARRAY | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_ADT_STACKARRAY_H
#define ORG_DEVOPSBROKER_ADT_STACKARRAY_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdbool.h>
#include <stdint.h>

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct StackArray {
	void **values;
	uint32_t length;
	uint32_t size;
} StackArray;

#if __SIZEOF_POINTER__ == 8
static_assert(sizeof(StackArray) == 16, "Check your assumptions");
#elif  __SIZEOF_POINTER__ == 4
static_assert(sizeof(StackArray) == 12, "Check your assumptions");
#endif

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_createStackArray
 * Description: Creates a StackArray struct instance
 *
 * Returns: A StackArray struct instance
 * ----------------------------------------------------------------------------
 */
StackArray *f106c0ab_createStackArray();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_createStackArray_size
 * Description: Creates a StackArray struct instance with the specified size
 *
 * Parameters:
 *   size   The size of the StackArray instance to initalize
 * Returns: A StackArray struct instance
 * ----------------------------------------------------------------------------
 */
StackArray *f106c0ab_createStackArray_size(uint32_t size);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_destroyStackArray
 * Description: Frees the memory allocated to the StackArray struct pointer
 *
 * Parameters:
 *   stackArray     A pointer to the StackArray instance to destroy
 * ----------------------------------------------------------------------------
 */
void f106c0ab_destroyStackArray(StackArray *stackArray);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_cleanUpStackArray
 * Description: Frees dynamically allocated memory within the StackArray instance
 *
 * Parameters:
 *   stackArray     A pointer to the StackArray instance to clean up
 *   freeElement    A function pointer to the method that frees the underlying
 *                  elemenmt contained within the StackArray instance
 * ----------------------------------------------------------------------------
 */
void f106c0ab_cleanUpStackArray(StackArray *stackArray, void freeElement(void *ptr));

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_initStackArray
 * Description: Initializes an existing StackArray struct
 *
 * Parameters:
 *   stackArray     A pointer to the StackArray instance to initalize
 * ----------------------------------------------------------------------------
 */
void f106c0ab_initStackArray(StackArray *stackArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_initStackArray_size
 * Description: Initializes an existing StackArray struct with the specified size
 *
 * Parameters:
 *   stackArray	    A pointer to the StackArray instance to initalize
 *   size           The size of the StackArray instance to initalize
 * ----------------------------------------------------------------------------
 */
void f106c0ab_initStackArray_size(StackArray *stackArray, uint32_t size);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_clear
 * Description: Removes all elements from the stack
 *
 * Parameters:
 *   stackArray     A pointer to the StackArray instance
 * ----------------------------------------------------------------------------
 */
void f106c0ab_clear(StackArray *stackArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_peek
 * Description: Returns the top-most element on the stack, or NULL if empty
 *
 * Parameters:
 *   stackArray     A pointer to the StackArray instance
 * ----------------------------------------------------------------------------
 */
void *f106c0ab_peek(StackArray *stackArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_pop
 * Description: Removes and returns the top-most element on the stack, or NULL if empty
 *
 * Parameters:
 *   stackArray     A pointer to the StackArray instance
 * ----------------------------------------------------------------------------
 */
void *f106c0ab_pop(StackArray *stackArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    f106c0ab_push
 * Description: Places an element at the top of the stack
 *
 * Parameters:
 *   stackArray     A pointer to the StackArray instance
 *   item           The item to add to the top of the stack
 * ----------------------------------------------------------------------------
 */
void f106c0ab_push(StackArray *stackArray, void *item);

#endif /* ORG_DEVOPSBROKER_ADT_STACKARRAY_H */
