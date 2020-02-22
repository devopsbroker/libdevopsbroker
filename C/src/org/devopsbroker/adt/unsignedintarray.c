/*
 * unsignedintarray.c - DevOpsBroker C source file for the org.devopsbroker.adt.UnsignedIntArray struct
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
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "unsignedintarray.h"

#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════

#define A8638224_DEFAULT_SIZE 16

// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

UnsignedIntArray *a8638224_createUnsignedIntArray() {
	UnsignedIntArray *listArray = f668c4bd_malloc(sizeof(UnsignedIntArray));

	listArray->values = f668c4bd_mallocArray(sizeof(uint32_t), A8638224_DEFAULT_SIZE);
	listArray->size = A8638224_DEFAULT_SIZE;
	listArray->length = 0;

	return listArray;
}

void a8638224_destroyUnsignedIntArray(UnsignedIntArray *listArray) {
	free(listArray->values);
	free(listArray);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void a8638224_cleanUpUnsignedIntArray(UnsignedIntArray *listArray) {
	free(listArray->values);
}

void a8638224_initUnsignedIntArray(UnsignedIntArray *listArray) {
	listArray->values = f668c4bd_mallocArray(sizeof(uint32_t), A8638224_DEFAULT_SIZE);
	listArray->size = A8638224_DEFAULT_SIZE;
	listArray->length = 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void a8638224_extract(UnsignedIntArray *listArray, StringBuilder *strBuilder) {
	c598a24c_append_char(strBuilder, '[');

	if (listArray->length > 0) {
		c598a24c_append_uint(strBuilder, listArray->values[0]);

		for (uint32_t i=1; i < listArray->length; i++) {
			c598a24c_append_char(strBuilder, ',');
			c598a24c_append_uint(strBuilder, listArray->values[i]);
		}
	}

	c598a24c_append_char(strBuilder, ']');
}
