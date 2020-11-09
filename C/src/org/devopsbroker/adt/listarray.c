/*
 * listarray.c - DevOpsBroker C source file for providing array-based dynamic list functionality
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
 *
 * -----------------------------------------------------------------------------
 * Developed on Ubuntu 16.04.5 LTS running kernel.osrelease = 4.15.0-34
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include "listarray.h"

#include "../lang/array.h"
#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define LISTARRAY_DEFAULT_SIZE  8

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

ListArray *b196167f_createListArray() {
	ListArray *listArray = f668c4bd_malloc(sizeof(ListArray));

	listArray->values = f668c4bd_malloc(sizeof(void*) * LISTARRAY_DEFAULT_SIZE);
	listArray->size = LISTARRAY_DEFAULT_SIZE;
	listArray->length = 0;

	return listArray;
}

ListArray *b196167f_createListArrayWithSize(const uint32_t size) {
	ListArray *listArray = f668c4bd_malloc(sizeof(ListArray));

	listArray->values = f668c4bd_malloc(sizeof(void*) * size);
	listArray->size = size;
	listArray->length = 0;

	return listArray;
}

void b196167f_destroyListArray(ListArray *listArray, void freeElement(void *ptr)) {
	if (listArray->values != NULL) {
		// Free all the elements first, if necessary
		if (freeElement != NULL) {
			for (uint32_t i=0; i < listArray->length; i++) {
				freeElement(listArray->values[i]);
			}
		}

		f668c4bd_free(listArray->values);
	}

	f668c4bd_free(listArray);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void b196167f_cleanUpListArray(ListArray *listArray, void freeElement(void *ptr)) {
	if (listArray->values != NULL) {
		// Free all the elements first, if necessary
		if (freeElement != NULL) {
			for (uint32_t i=0; i < listArray->length; i++) {
				freeElement(listArray->values[i]);
			}
		}

		f668c4bd_free(listArray->values);
	}
}

void b196167f_initListArray(ListArray *listArray) {
	listArray->values = f668c4bd_malloc(sizeof(void*) * LISTARRAY_DEFAULT_SIZE);
	listArray->size = LISTARRAY_DEFAULT_SIZE;
	listArray->length = 0;
}

void b196167f_initListArrayWithSize(ListArray *listArray, const uint32_t size) {
	listArray->values = f668c4bd_malloc(sizeof(void*) * size);
	listArray->size = size;
	listArray->length = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef __arm__
void b196167f_add(ListArray *listArray, void *element) {
	if (listArray->length == listArray->size) {
		// Ensure the new capacity is a multiple of 8
		uint32_t capacity = (((listArray->length + 7) >> 3) << 3) << 1;

		listArray->values = f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(void*), capacity);
		listArray->size = capacity;

	}

	listArray->values[listArray->length++] = element;
}
#endif

#ifdef __arm__
void b196167f_addAll(ListArray *listArray, void *elementArray[], uint32_t numElements) {
	uint32_t newLength = listArray->length + numElements;

	if (newLength >= listArray->size) {
		// Ensure the new capacity is a multiple of 8
		uint32_t capacity = (((newLength + 7) >> 3) << 3) << 1;

		listArray->values = f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(void*), capacity);
		listArray->size = capacity;
	}

	for (uint32_t i = listArray->length; i < newLength; i++) {
		listArray->values[i] = elementArray[0];
		elementArray++;
	}

	listArray->length = newLength;
}
#endif

#ifdef __arm__
void b196167f_addFromStack(ListArray *listArray, void *stack[], uint32_t numElements) {
	uint32_t newLength = listArray->length + numElements;

	if (newLength >= listArray->size) {
		// Ensure the new capacity is a multiple of 8
		uint32_t capacity = (((newLength + 7) >> 3) << 3) << 1;

		listArray->values = f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(void*), capacity);
		listArray->size = capacity;
	}

	for (uint32_t i = listArray->length; i < newLength; i++) {
		listArray->values[i] = stack[0];
		stack--;
	}

	listArray->length = newLength;
}
#endif

void b196167f_clear(ListArray *listArray, void freeElement(void *ptr)) {
	if (listArray->values != NULL && freeElement != NULL) {
		// Free all the elements first, if necessary
		for (uint32_t i=0; i < listArray->length; i++) {
			freeElement(listArray->values[i]);
		}
	}

	listArray->length = 0;
}

void b196167f_ensureCapacity(ListArray *listArray, uint32_t capacity) {
	if (listArray->size < capacity) {
		// Ensure the new capacity is a multiple of 8
		capacity = ((capacity + 7) >> 3) << 3;

		listArray->values = f668c4bd_resizeArray(listArray->values, listArray->length, sizeof(void*), capacity);
		listArray->size = capacity;
	}
}

void *b196167f_get(ListArray *listArray, uint32_t index) {
	if (listArray->length > index) {
		return listArray->values[index];
	}

	return NULL;
}

void *b196167f_last(ListArray *listArray) {
	if (listArray->length > 0) {
		return listArray->values[listArray->length - 1];
	}

	return NULL;
}

void b196167f_reset(ListArray *listArray) {
	for (uint32_t i=0; i < listArray->length; i++) {
		listArray->values[i] = NULL;
	}

	listArray->length = 0;
}

void b196167f_sort(ListArray *listArray, int compare(void *a, void *b)) {
	if (listArray->length > 0) {
		b33b0483_sortPtrArray(listArray->values, 0, listArray->length-1, compare);
	}
}
