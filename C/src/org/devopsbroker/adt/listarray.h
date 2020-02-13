/*
 * listarray.h - DevOpsBroker C header file for providing array-based dynamic list functionality
 *
 * Copyright (C) 2018-2019 Edward Smith <edwardsmith@devopsbroker.org>
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
 * Developed on Ubuntu 16.04.5 LTS running kernel.osrelease = 4.15.0-34
 *
 * echo ORG_DEVOPSBROKER_ADT_LISTARRAY | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_ADT_LISTARRAY_H
#define ORG_DEVOPSBROKER_ADT_LISTARRAY_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdint.h>

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct ListArray {
	void **values;
	uint32_t length;
	uint32_t size;
} ListArray;

static_assert(sizeof(ListArray) == 16, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_createListArray
 * Description: Creates a ListArray struct with the default list size
 *
 * Returns:     A ListArray struct with the default list size
 * ----------------------------------------------------------------------------
 */
ListArray *b196167f_createListArray();

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_createListArrayWithSize
 * Description: Creates a ListArray struct with the specified list size
 *
 * Parameters:
 *   size       The size of the list to allocate
 * Returns:     A ListArray struct with the specified list size
 * ---------------------------------------------------------------------
 */
ListArray *b196167f_createListArrayWithSize(const uint32_t size);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_destroyListArray
 * Description: Frees the memory allocated to the ListArray struct pointer
 *
 * Parameters:
 *   listArray     A pointer to the ListArray instance to destroy
 * ----------------------------------------------------------------------------
 */
void b196167f_destroyListArray(ListArray *listArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_destroyAllElements
 * Description: Frees memory allocated to all elements contained within the ListArray
 *
 * Parameters:
 *   listArray     A pointer to the ListArray instance
 * ----------------------------------------------------------------------------
 */
void b196167f_destroyAllElements(ListArray *listArray);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_cleanUpListArray
 * Description: Frees dynamically allocated memory within the ListArray instance
 *
 * Parameters:
 *   listArray      A pointer to the ListArray instance to clean up
 * ----------------------------------------------------------------------------
 */
void b196167f_cleanUpListArray(ListArray *listArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_initListArray
 * Description: Initializes a ListArray struct with the default list size
 *
 * Parameters:
 *   listArray      A pointer to the ListArray instance to initalize
 * ----------------------------------------------------------------------------
 */
void b196167f_initListArray(ListArray *listArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_initListArrayWithSize
 * Description: Initializes a ListArray struct with the specified list size
 *
 * Parameters:
 *   listArray  A pointer to the ListArray instance to initalize
 *   size       The size of the list to allocate
 * ----------------------------------------------------------------------------
 */
void b196167f_initListArrayWithSize(ListArray *listArray, const uint32_t size);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_add
 * Description: Adds an element to the ListArray instance
 *
 * Parameters:
 *   listArray  The ListArray instance
 *   element    The element to add to the ListArray
 * ----------------------------------------------------------------------------
 */
void b196167f_add(ListArray *listArray, void *element);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_addAll
 * Description: Appends all of the elements to the end of the ListArray instance
 *
 * Parameters:
 *   listArray      The ListArray instance
 *   elementArray   The element to add to the ListArray
 *   numElements    The number of elements to add
 * ----------------------------------------------------------------------------
 */
void b196167f_addAll(ListArray *listArray, void *elementArray, uint32_t numElements);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_addFromStack
 * Description: Adds the values stored on the stack to the ListArray
 *
 * Parameters:
 *   listArray      A pointer to the ListArray instance to populate
 *   stack          A pointer to the stack
 *   numElements    The number of elements to add
 * ----------------------------------------------------------------------------
 */
void b196167f_addFromStack(ListArray *listArray, void *stack, uint32_t numElements);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_clear
 * Description: Removes all elements from the list
 *
 * Parameters:
 *   listArray      A pointer to the ListArray instance
 * ----------------------------------------------------------------------------
 */
void b196167f_clear(ListArray *listArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_get
 * Description: Retrieves the elements at the specified index
 *
 * Parameters:
 *   listArray      A pointer to the ListArray instance
 *   index          The index of the element to retrieve
 * ----------------------------------------------------------------------------
 */
void *b196167f_get(ListArray *listArray, uint32_t index);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_last
 * Description: Retrieves the last element in the list
 *
 * Parameters:
 *   listArray      A pointer to the ListArray instance
 * ----------------------------------------------------------------------------
 */
void *b196167f_last(ListArray *listArray);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b196167f_sort
 * Description: Sorts the ListArray using the compare() function
 *
 * Parameters:
 *   listArray  A pointer to the ListArray instance
 *   compare    The compare() function to use during sorting
 * ----------------------------------------------------------------------------
 */
void b196167f_sort(ListArray *listArray, int compare(void *a, void *b));

#endif /* ORG_DEVOPSBROKER_ADT_LISTARRAY_H */
