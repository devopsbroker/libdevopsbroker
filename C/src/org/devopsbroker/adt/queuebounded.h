/*
 * queuebounded.h - DevOpsBroker C header file for the org.devopsbroker.adt.QueueBounded struct
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
 * echo ORG_DEVOPSBROKER_ADT_QUEUEBOUNDED | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_ADT_QUEUEBOUNDED_H
#define ORG_DEVOPSBROKER_ADT_QUEUEBOUNDED_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdbool.h>
#include <stdint.h>

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

/*
 * Flexible Array Member values does not have a static size
 */
typedef struct QueueBounded {
	uint32_t head;
	uint32_t tail;
	uint32_t length;
	uint32_t capacity;
	void *values[];
} QueueBounded;

static_assert(sizeof(QueueBounded) == 16, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_createQueueBounded
 * Description: Creates a QueueBounded struct instance
 *
 * Parameters:
 *   capacity   The fixed capacity of the bounded queue
 * Returns:     A QueueBounded struct instance with the given fixed capacity
 * ----------------------------------------------------------------------------
 */
QueueBounded *b8da7268_createQueueBounded(uint32_t capacity);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_destroyQueueBounded
 * Description: Frees the memory allocated to the QueueBounded struct pointer
 *
 * Parameters:
 *   queue      A pointer to the QueueBounded instance to destroy
 * ----------------------------------------------------------------------------
 */
void b8da7268_destroyQueueBounded(QueueBounded *queue);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_dequeue
 * Description: Retrieves and removes the head of the queue, or returns NULL
 *              if the queue is empty
 *
 * Parameters:
 *   queue      A pointer to the QueueBounded instance
 * Returns:     The head of the queue value, or NULL if the queue is empty
 * ----------------------------------------------------------------------------
 */
void *b8da7268_dequeue(QueueBounded *queue);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_enqueue
 * Description: Appends the value to the tail of the queue, if the queue is
 *              not already full. If the queue is full, false is returned to
 *              indicate the value was not added to the queue
 *
 * Parameters:
 *   queue      A pointer to the QueueBounded instance
 *   value      The value to add to the end of the queue
 * Returns:     True if the value was added to the queue, false otherwise
 * ----------------------------------------------------------------------------
 */
bool b8da7268_enqueue(QueueBounded *queue, void *value);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_isEmpty
 * Description: Returns true if the queue is empty, false otherwise
 *
 * Parameters:
 *   queue      A pointer to the QueueBounded instance
 * Returns:     True if the queue is empty, false otherwise
 * ----------------------------------------------------------------------------
 */
bool b8da7268_isEmpty(QueueBounded *queue);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_isFull
 * Description: Returns true if the queue is full, false otherwise
 *
 * Parameters:
 *   queue      A pointer to the QueueBounded instance
 * Returns:     True if the queue is full, false otherwise
 * ----------------------------------------------------------------------------
 */
bool b8da7268_isFull(QueueBounded *queue);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_peek
 * Description: Retrieves, but does not remove, the head of the queue. Or
 *              returns NULL if the queue is empty
 *
 * Parameters:
 *   queue      A pointer to the QueueBounded instance
 * Returns:     The head of the queue value, or NULL if the queue is empty
 * ----------------------------------------------------------------------------
 */
void *b8da7268_peek(QueueBounded *queue);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_reset
 * Description: Resets the queue to zero length
 *
 * Parameters:
 *   queue      A pointer to the QueueBounded instance
 * ----------------------------------------------------------------------------
 */
void b8da7268_reset(QueueBounded *queue);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    b8da7268_toArray
 * Description: Extracts a dynamic array of void* pointers from the queue
 *
 * Parameters:
 *   queue      A pointer to the QueueBounded instance
 * Returns:     The head of the queue value, or NULL if the queue is empty
 * ----------------------------------------------------------------------------
 */
void **b8da7268_toArray(QueueBounded *queue);

#endif /* ORG_DEVOPSBROKER_ADT_QUEUEBOUNDED_H */
