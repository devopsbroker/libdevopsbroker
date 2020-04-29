/*
 * testQueueBounded.c - DevOpsBroker C source file for testing org/devopsbroker/adt/queuebounded.h
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
 *
 * -----------------------------------------------------------------------------
 * Developed on Ubuntu 18.04.4 LTS running kernel.osrelease = 5.3.0-46
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "org/devopsbroker/adt/queuebounded.h"
#include "org/devopsbroker/lang/memory.h"
#include "org/devopsbroker/test/unittest.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static QueueBounded *setupTesting();
static void tearDownTesting(QueueBounded *queue);

static void testEnqueue(QueueBounded *queue);
static void testDequeue(QueueBounded *queue);
static void testIsEmpty(QueueBounded *queue);
static void testReset(QueueBounded *queue);
static void testIsFull(QueueBounded *queue);
static void testPeek(QueueBounded *queue);
static void testToArray(QueueBounded *queue);


// ══════════════════════════════════ main() ══════════════════════════════════

int main(int argc, char *argv[]) {
	QueueBounded *queue = setupTesting();

	testEnqueue(queue);
	testDequeue(queue);
	testIsEmpty(queue);
	testReset(queue);
	testIsFull(queue);
	testPeek(queue);
	testToArray(queue);

	tearDownTesting(queue);

	// Exit with success
	exit(EXIT_SUCCESS);
}

// ═════════════════════════ Function Implementations ═════════════════════════

static QueueBounded *setupTesting() {
	QueueBounded *queue;

	printTestName("testQueueBounded Setup");
	queue = b8da7268_createQueueBounded(32);

	positiveTestInt("  QueueBounded head = 0\t\t\t", 0, queue->head);
	positiveTestInt("  QueueBounded tail = 0\t\t\t", 0, queue->tail);
	positiveTestInt("  QueueBounded length = 0\t\t", 0, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);

	printf("\n");

	return queue;
}

static void tearDownTesting(QueueBounded *queue) {
	printTestName("b8da7268_destroyQueueBounded");
	b8da7268_destroyQueueBounded(queue);

	printf("\n");
}

static void testEnqueue(QueueBounded *queue) {
	printTestName("b8da7268_enqueue");
	puts("  b8da7268_enqueue(queue, \"foo\")..."); b8da7268_enqueue(queue, "foo");
	puts("  b8da7268_enqueue(queue, \"bar\")..."); b8da7268_enqueue(queue, "bar");
	puts("  b8da7268_enqueue(queue, \"baz\")..."); b8da7268_enqueue(queue, "baz");
	puts("  b8da7268_enqueue(queue, \"XYZ\")..."); b8da7268_enqueue(queue, "XYZ");
	puts("  b8da7268_enqueue(queue, \"123\")..."); b8da7268_enqueue(queue, "123");
	puts("  b8da7268_enqueue(queue, \"234\")..."); b8da7268_enqueue(queue, "234");
	puts("  b8da7268_enqueue(queue, \"345\")..."); b8da7268_enqueue(queue, "345");
	puts("  b8da7268_enqueue(queue, \"456\")..."); b8da7268_enqueue(queue, "456");
	printf("\n");
	positiveTestInt("  QueueBounded head = 0\t\t\t", 0, queue->head);
	positiveTestInt("  QueueBounded tail = 8\t\t\t", 8, queue->tail);
	positiveTestInt("  QueueBounded length = 8\t\t", 8, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);

	printf("\n");
}

static void testDequeue(QueueBounded *queue) {
	printTestName("b8da7268_dequeue");
	positiveTestVoid("  b8da7268_dequeue(queue) = foo\t\t", "foo", b8da7268_dequeue(queue));
	positiveTestVoid("  b8da7268_dequeue(queue) = bar\t\t", "bar", b8da7268_dequeue(queue));
	positiveTestVoid("  b8da7268_dequeue(queue) = baz\t\t", "baz", b8da7268_dequeue(queue));
	positiveTestVoid("  b8da7268_dequeue(queue) = XYZ\t\t", "XYZ", b8da7268_dequeue(queue));
	positiveTestVoid("  b8da7268_dequeue(queue) = 123\t\t", "123", b8da7268_dequeue(queue));
	positiveTestVoid("  b8da7268_dequeue(queue) = 234\t\t", "234", b8da7268_dequeue(queue));
	positiveTestVoid("  b8da7268_dequeue(queue) = 345\t\t", "345", b8da7268_dequeue(queue));
	positiveTestVoid("  b8da7268_dequeue(queue) = 456\t\t", "456", b8da7268_dequeue(queue));
	printf("\n");
	positiveTestInt("  QueueBounded head = 8\t\t\t", 8, queue->head);
	positiveTestInt("  QueueBounded tail = 8\t\t\t", 8, queue->tail);
	positiveTestInt("  QueueBounded length = 0\t\t", 0, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);

	printf("\n");
}

static void testIsEmpty(QueueBounded *queue) {
	printTestName("b8da7268_isEmpty");
	positiveTestBool("  b8da7268_isEmpty(queue) = true\t", true, b8da7268_isEmpty(queue));
	puts("  b8da7268_enqueue(queue, \"foo\")..."); b8da7268_enqueue(queue, "foo");
	positiveTestBool("  b8da7268_isEmpty(queue) = false\t", false, b8da7268_isEmpty(queue));
	puts("  b8da7268_dequeue(queue)..."); b8da7268_dequeue(queue);
	printf("\n");
	positiveTestInt("  QueueBounded head = 9\t\t\t", 9, queue->head);
	positiveTestInt("  QueueBounded tail = 9\t\t\t", 9, queue->tail);
	positiveTestInt("  QueueBounded length = 0\t\t", 0, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);

	printf("\n");
}

static void testReset(QueueBounded *queue) {
	printTestName("b8da7268_reset");
	puts("  b8da7268_reset(queue)..."); b8da7268_reset(queue);
	printf("\n");
	positiveTestInt("  QueueBounded head = 0\t\t\t", 0, queue->head);
	positiveTestInt("  QueueBounded tail = 0\t\t\t", 0, queue->tail);
	positiveTestInt("  QueueBounded length = 0\t\t", 0, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);

	printf("\n");
}

static void testIsFull(QueueBounded *queue) {
	printTestName("b8da7268_isFull");
	positiveTestBool("  b8da7268_isFull(queue) = false\t", false, b8da7268_isFull(queue));
	puts("  b8da7268_enqueue(queue, \"foo\")..."); b8da7268_enqueue(queue, "foo");
	puts("  b8da7268_enqueue(queue, \"bar\")..."); b8da7268_enqueue(queue, "bar");
	puts("  b8da7268_enqueue(queue, \"baz\")..."); b8da7268_enqueue(queue, "baz");
	puts("  b8da7268_enqueue(queue, \"XYZ\")..."); b8da7268_enqueue(queue, "XYZ");
	puts("  b8da7268_enqueue(queue, \"123\")..."); b8da7268_enqueue(queue, "123");
	puts("  b8da7268_enqueue(queue, \"234\")..."); b8da7268_enqueue(queue, "234");
	puts("  b8da7268_enqueue(queue, \"345\")..."); b8da7268_enqueue(queue, "345");
	puts("  b8da7268_enqueue(queue, \"456\")..."); b8da7268_enqueue(queue, "456");
	puts("  b8da7268_enqueue(queue, \"foo\")..."); b8da7268_enqueue(queue, "foo");
	puts("  b8da7268_enqueue(queue, \"bar\")..."); b8da7268_enqueue(queue, "bar");
	puts("  b8da7268_enqueue(queue, \"baz\")..."); b8da7268_enqueue(queue, "baz");
	puts("  b8da7268_enqueue(queue, \"XYZ\")..."); b8da7268_enqueue(queue, "XYZ");
	puts("  b8da7268_enqueue(queue, \"123\")..."); b8da7268_enqueue(queue, "123");
	puts("  b8da7268_enqueue(queue, \"234\")..."); b8da7268_enqueue(queue, "234");
	puts("  b8da7268_enqueue(queue, \"345\")..."); b8da7268_enqueue(queue, "345");
	puts("  b8da7268_enqueue(queue, \"456\")..."); b8da7268_enqueue(queue, "456");
	puts("  b8da7268_enqueue(queue, \"foo\")..."); b8da7268_enqueue(queue, "foo");
	puts("  b8da7268_enqueue(queue, \"bar\")..."); b8da7268_enqueue(queue, "bar");
	puts("  b8da7268_enqueue(queue, \"baz\")..."); b8da7268_enqueue(queue, "baz");
	puts("  b8da7268_enqueue(queue, \"XYZ\")..."); b8da7268_enqueue(queue, "XYZ");
	puts("  b8da7268_enqueue(queue, \"123\")..."); b8da7268_enqueue(queue, "123");
	puts("  b8da7268_enqueue(queue, \"234\")..."); b8da7268_enqueue(queue, "234");
	puts("  b8da7268_enqueue(queue, \"345\")..."); b8da7268_enqueue(queue, "345");
	puts("  b8da7268_enqueue(queue, \"456\")..."); b8da7268_enqueue(queue, "456");
	puts("  b8da7268_enqueue(queue, \"foo\")..."); b8da7268_enqueue(queue, "foo");
	puts("  b8da7268_enqueue(queue, \"bar\")..."); b8da7268_enqueue(queue, "bar");
	puts("  b8da7268_enqueue(queue, \"baz\")..."); b8da7268_enqueue(queue, "baz");
	puts("  b8da7268_enqueue(queue, \"XYZ\")..."); b8da7268_enqueue(queue, "XYZ");
	puts("  b8da7268_enqueue(queue, \"123\")..."); b8da7268_enqueue(queue, "123");
	puts("  b8da7268_enqueue(queue, \"234\")..."); b8da7268_enqueue(queue, "234");
	puts("  b8da7268_enqueue(queue, \"345\")..."); b8da7268_enqueue(queue, "345");
	puts("  b8da7268_enqueue(queue, \"456\")..."); b8da7268_enqueue(queue, "456");
	positiveTestBool("  b8da7268_isFull(queue) = true\t\t", true, b8da7268_isFull(queue));
	printf("\n");
	positiveTestInt("  QueueBounded head = 0\t\t\t", 0, queue->head);
	positiveTestInt("  QueueBounded tail = 0\t\t\t", 0, queue->tail);
	positiveTestInt("  QueueBounded length = 32\t\t", 32, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);

	printf("\n");
}

static void testPeek(QueueBounded *queue) {
	printTestName("b8da7268_peek");
	positiveTestVoid("  b8da7268_peek(queue) = foo\t\t", "foo", b8da7268_peek(queue));
	printf("\n");
	positiveTestInt("  QueueBounded head = 0\t\t\t", 0, queue->head);
	positiveTestInt("  QueueBounded tail = 0\t\t\t", 0, queue->tail);
	positiveTestInt("  QueueBounded length = 32\t\t", 32, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);

	printf("\n");
}

static void testToArray(QueueBounded *queue) {
	void **array;

	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);
	b8da7268_dequeue(queue);

	array = b8da7268_toArray(queue);

	printTestName("b8da7268_toArray");
	positiveTestVoid("  array[0] = 123\t\t\t", "123", array[0]);
	positiveTestVoid("  array[1] = 234\t\t\t", "234", array[1]);
	positiveTestVoid("  array[2] = 345\t\t\t", "345", array[2]);
	positiveTestVoid("  array[3] = 456\t\t\t", "456", array[3]);
	printf("\n");
	positiveTestInt("  QueueBounded head = 28\t\t", 28, queue->head);
	positiveTestInt("  QueueBounded tail = 0\t\t\t", 0, queue->tail);
	positiveTestInt("  QueueBounded length = 4\t\t", 4, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);
	printf("\n");

	f668c4bd_free(array);

	puts("  b8da7268_enqueue(queue, \"foo\")..."); b8da7268_enqueue(queue, "foo");
	puts("  b8da7268_enqueue(queue, \"bar\")..."); b8da7268_enqueue(queue, "bar");
	puts("  b8da7268_enqueue(queue, \"baz\")..."); b8da7268_enqueue(queue, "baz");
	puts("  b8da7268_enqueue(queue, \"XYZ\")..."); b8da7268_enqueue(queue, "XYZ");
	printf("\n");

	array = b8da7268_toArray(queue);

	positiveTestVoid("  array[0] = 123\t\t\t", "123", array[0]);
	positiveTestVoid("  array[1] = 234\t\t\t", "234", array[1]);
	positiveTestVoid("  array[2] = 345\t\t\t", "345", array[2]);
	positiveTestVoid("  array[3] = 456\t\t\t", "456", array[3]);
	positiveTestVoid("  array[4] = foo\t\t\t", "foo", array[4]);
	positiveTestVoid("  array[5] = bar\t\t\t", "bar", array[5]);
	positiveTestVoid("  array[6] = baz\t\t\t", "baz", array[6]);
	positiveTestVoid("  array[7] = XYZ\t\t\t", "XYZ", array[7]);
	printf("\n");
	positiveTestInt("  QueueBounded head = 28\t\t", 28, queue->head);
	positiveTestInt("  QueueBounded tail = 4\t\t\t", 4, queue->tail);
	positiveTestInt("  QueueBounded length = 8\t\t", 8, queue->length);
	positiveTestInt("  QueueBounded capacity = 32\t\t", 32, queue->capacity);

	f668c4bd_free(array);

	printf("\n");
}
