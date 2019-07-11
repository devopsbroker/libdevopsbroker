/*
 * hashmap.c - C source file for the org.devopsbroker.adt.HashMap struct
 *
 * Copyright (C) 2019 AUTHOR_NAME <email@address.com>
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
 * Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-21
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdlib.h>

#include "hashmap.h"

#include "../lang/memory.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════

static uint32_t resize(HashMap *hashMap, const uint32_t hashCode);

// ═════════════════════════ Function Implementations ═════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

HashMap *c47905f7_createHashMap(uint32_t (*hashCode)(void *), bool (*equals)(void *, void *), uint32_t capacity) {
	HashMap *hashMap = malloc(sizeof(HashMap));

	// Calculate table length
	hashMap->capacity = capacity;
	hashMap->length = capacity * 1.375f;

	if ((hashMap->length & 1) == 0) {
		hashMap->length++;
	}

	hashMap->table = f668c4bd_malloc_size_size(sizeof(MapEntry *), hashMap->length);

	// Set hashCode and equals methods for the key
	hashMap->hashCode = hashCode;
	hashMap->equals = equals;

	hashMap->size = 0;

	return hashMap;
}

void c47905f7_destroyHashMap(HashMap *hashMap) {
	c47905f7_clear(hashMap);

	free(hashMap->table);

	free(hashMap);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

void c47905f7_initHashMap(HashMap *hashMap, uint32_t (*hashCode)(void *), bool (*equals)(void *, void *), uint32_t capacity) {
	// Normalize capacity to be a multiple of eight
	capacity = ((capacity + 7) >> 3) << 3;

	// Calculate table length
	hashMap->capacity = capacity;
	hashMap->length = capacity * 1.375f;

	if ((hashMap->length & 1) == 0) {
		hashMap->length++;
	}

	// Create initialized memory block for table
	hashMap->table = f668c4bd_malloc_size_size(sizeof(MapEntry *), hashMap->length);
	f668c4bd_meminit(hashMap->table, sizeof(MapEntry *) * hashMap->length);

	// Set hashCode and equals methods for the key
	hashMap->hashCode = hashCode;
	hashMap->equals = equals;

	hashMap->size = 0;
}

void c47905f7_cleanUpHashMap(HashMap *hashMap) {
	c47905f7_clear(hashMap);

	free(hashMap->table);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void c47905f7_clear(HashMap *hashMap) {
	MapEntry *entry, *next;

	for (uint32_t i = 0; i < hashMap->length; i++) {
		for (entry = hashMap->table[i]; entry != NULL; entry = next) {
			next = entry->next;
			free(entry);
		}

		hashMap->table[i] = NULL;
	}

	hashMap->size = 0;
}

bool c47905f7_containsKey(HashMap *hashMap, void *key) {
	uint32_t index = hashMap->hashCode(key) % hashMap->length;

	for (MapEntry *e = hashMap->table[index]; e != NULL; e = e->next) {
		if (hashMap->equals(e->key, key)) {
			return true;
		}
	}

	return false;
}

void *c47905f7_get(HashMap *hashMap, void *key) {
	uint32_t index = hashMap->hashCode(key) % hashMap->length;

	for (MapEntry *e = hashMap->table[index]; e != NULL; e = e->next) {
		if (hashMap->equals(e->key, key)) {
			return e->value;
		}
	}

	return NULL;
}

void *c47905f7_put(HashMap *hashMap, void *key, void *value) {
	uint32_t hashCode = hashMap->hashCode(key);
	uint32_t i = hashCode % hashMap->length;

	for (MapEntry *e = hashMap->table[i]; e != NULL; e = e->next) {
		if (hashMap->equals(e->key, key)) {
			void *v = e->value;
			e->value = value;
			return v;
		}
	}

	if (hashMap->size++ == hashMap->capacity) {
		i = resize(hashMap, hashCode);
	}

	// Add new MapEntry to  the HashMap
	MapEntry *entry = f668c4bd_malloc(sizeof(MapEntry));

	entry->key = key;
	entry->value = value;
	entry->hash = hashCode;
	entry->next = hashMap->table[i];

	hashMap->table[i] = entry;

	return NULL;
}

void c47905f7_putAll(HashMap *hashMap, void **elementArray, uint32_t numElements) {
	for (int i = 0; i < numElements; i += 2) {
		c47905f7_put(hashMap, elementArray[i], elementArray[i+1]);
	}
}

void c47905f7_putFromStack(HashMap *hashMap, void **stack, uint32_t numElements) {
	for (int i = 0; i < numElements; i += 2) {
		c47905f7_put(hashMap, stack[i+1], stack[i]);
	}
}

void *c47905f7_remove(HashMap *hashMap, void *key) {
	uint32_t i = hashMap->hashCode(key) % hashMap->length;
	MapEntry *prev = NULL;

	for (MapEntry *e = hashMap->table[i]; e != NULL; e = e->next) {
		if (hashMap->equals(e->key, key)) {
			if (prev == NULL) {
				hashMap->table[i] = e->next;
			} else {
				prev->next = e->next;
			}

			void *v = e->value;
			hashMap->size--;
			free(e);
			return v;
		}

		prev = e;
	}

	return NULL;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

static uint32_t resize(HashMap *hashMap, const uint32_t hashCode) {
	MapEntry **oldTable = hashMap->table;
	uint32_t oldLength = hashMap->length;
	hashMap->capacity <<= 1;

	// Calculate new table length
	hashMap->length = hashMap->capacity * 1.375f;

	if ((hashMap->length & 1) == 0) {
		hashMap->length++;
	}

	hashMap->table = f668c4bd_malloc_size_size(sizeof(MapEntry *), hashMap->length);
	f668c4bd_meminit(hashMap->table, sizeof(MapEntry *) * hashMap->length);

	MapEntry *entry, *next;
	uint32_t index;

	for (int i = 0; i < oldLength; i++) {
		for (entry = oldTable[i]; entry != NULL; entry = next) {
			next = entry->next;
			index = entry->hash % hashMap->length;
			entry->next = hashMap->table[index];
			hashMap->table[index] = entry;
		}
	}

	free(oldTable);

	return hashCode % hashMap->length;
}
