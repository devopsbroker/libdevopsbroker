/*
 * queuebounded.c - DevOpsBroker C source file for the org.devopsbroker.adt.QueueBounded struct
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

#include "queuebounded.h"

#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

QueueBounded *b8da7268_createQueueBounded(uint32_t capacity) {
	QueueBounded *queueBounded = f668c4bd_mallocFlexibleStruct(sizeof(QueueBounded), sizeof(void*), capacity);

	queueBounded->head = 0;
	queueBounded->tail = 0;
	queueBounded->length = 0;
	queueBounded->capacity = capacity;

	return queueBounded;
}

void b8da7268_destroyQueueBounded(QueueBounded *queueBounded) {
	free(queueBounded);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void *b8da7268_dequeue(QueueBounded *queue) {
	void *value = NULL;

	if (queue->length > 0) {
		// Remove value from the queue
		value = queue->values[queue->head];
		queue->length--;

		// Increment queue head
		queue->head++;
		if (queue->head == queue->length) {
			queue->head = 0;
		}
	}

	return value;
}

bool b8da7268_enqueue(QueueBounded *queue, void *value) {
	if (queue->length == queue->capacity) {
		return false;
	}

	// Add value to the queue
	queue->values[queue->tail] = value;
	queue->length++;

	// Increment queue tail
	queue->tail++;
	if (queue->tail == queue->length) {
		queue->tail = 0;
	}

	return true;
}

bool b8da7268_isEmpty(QueueBounded *queue) {
	return (queue->length == 0);
}

bool b8da7268_isFull(QueueBounded *queue) {
	return (queue->length == queue->capacity);
}

void *b8da7268_peek(QueueBounded *queue) {
	return (queue->length > 0) ? queue->values[queue->head] : NULL;
}
