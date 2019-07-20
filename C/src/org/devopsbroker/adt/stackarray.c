/*
 * stackarray.c - DevOpsBroker C source file for the org.devopsbroker.adt.StackArray struct
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
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "stackarray.h"

#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


#define F106C0AB_DEFAULT_SIZE 8


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

StackArray *f106c0ab_createStackArray() {
	StackArray *stackArray = f668c4bd_malloc(sizeof(StackArray));

	stackArray->values = f668c4bd_malloc(sizeof(void*) * F106C0AB_DEFAULT_SIZE);
	stackArray->size = F106C0AB_DEFAULT_SIZE;
	stackArray->length = 0;

	return stackArray;
}

StackArray *f106c0ab_createStackArray_size(uint32_t size) {
	StackArray *stackArray = f668c4bd_malloc(sizeof(StackArray));

	// Enforce minimum size value of eight
	if (size < F106C0AB_DEFAULT_SIZE) {
		size = F106C0AB_DEFAULT_SIZE;
	}

	stackArray->values = f668c4bd_malloc(sizeof(void*) * size);
	stackArray->size = size;
	stackArray->length = 0;

	return stackArray;
}

void f106c0ab_destroyStackArray(StackArray *stackArray) {
	f668c4bd_free(stackArray->values);
	f668c4bd_free(stackArray);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void f106c0ab_cleanUpStackArray(StackArray *stackArray) {
	f668c4bd_free(stackArray->values);
}

void f106c0ab_initStackArray(StackArray *stackArray) {
	stackArray->values = f668c4bd_malloc(sizeof(void*) * F106C0AB_DEFAULT_SIZE);
	stackArray->size = F106C0AB_DEFAULT_SIZE;
	stackArray->length = 0;
}

void f106c0ab_initStackArray_size(StackArray *stackArray, uint32_t size) {
	// Enforce minimum size value of eight
	if (size < F106C0AB_DEFAULT_SIZE) {
		size = F106C0AB_DEFAULT_SIZE;
	}

	stackArray->values = f668c4bd_malloc(sizeof(void*) * size);
	stackArray->size = size;
	stackArray->length = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void f106c0ab_clear(StackArray *stackArray) {
	stackArray->length = 0;
}

void *f106c0ab_peek(StackArray *stackArray) {
	return (stackArray->length == 0) ? NULL : stackArray->values[stackArray->length - 1];
}

void *f106c0ab_pop(StackArray *stackArray) {
	if (stackArray->length == 0) {
		return NULL;
	}

	return stackArray->values[--stackArray->length];
}

void f106c0ab_push(StackArray *stackArray, void *item) {
	if (stackArray->length == stackArray->size) {
		stackArray->size <<= 1;
		stackArray->values = f668c4bd_resizeArray(stackArray->values, stackArray->length, sizeof(void*), stackArray->size);
	}

	stackArray->values[stackArray->length++] = item;
}
