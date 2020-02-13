/*
 * array.c - DevOpsBroker C source file for array-related functionality
 *
 * Copyright (C) 2019-2020 Edward Smith <edwardsmith@devopsbroker.org>
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
 * Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-17
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "array.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void b33b0483_sortPtrArray(void **array, int l, int h, int compare(void *a, void *b)) {
	void *highPtr, *currentPtr, *tempPtr;
	int pivot;

	// Create an auxiliary stack
	int stack[h - l + 1];
	int top = 0;

	stack[top++] = l;
	stack[top++] = h;

	while (top > 0) {
		h = stack[--top];
		l = stack[--top];

		// Partition the array around the pivot point
		highPtr = array[h];
		pivot = (l - 1);

		for (int j = l; j < h; j++) {
			currentPtr = array[j];

			if (compare(currentPtr, highPtr) <= 0) {
				pivot++;

				// Swap pivot pointer with current pointer
				tempPtr = array[pivot];
				array[pivot] = currentPtr;
				array[j] = tempPtr;
			}
		}

		// Swap pivot pointer with high pointer
		pivot++;
		tempPtr = array[pivot];
		array[pivot] = highPtr;
		array[h] = tempPtr;

		// Push left side of pivot to stack, if present
		if ((pivot - 1) > l) {
			stack[top++] = l;
			stack[top++] = pivot - 1;
		}

		// Push right side of pivot to stack, if present
		if ((pivot + 1) < h) {
			stack[top++] = pivot + 1;
			stack[top++] = h;
		}
	}
}
