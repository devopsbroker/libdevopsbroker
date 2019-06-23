/*
 * hashmap.h - C header file for the org.devopsbroker.adt.HashMap struct
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
 * echo ORG_DEVOPSBROKER_ADT_HASHMAP | md5sum | cut -c 25-32
 * -----------------------------------------------------------------------------
 */

#ifndef ORG_DEVOPSBROKER_ADT_HASHMAP_H
#define ORG_DEVOPSBROKER_ADT_HASHMAP_H

// ═════════════════════════════════ Includes ═════════════════════════════════

#include <stdbool.h>
#include <stdint.h>

#include <assert.h>

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════

typedef struct MapEntry {
	void *key;
	void *value;
	struct MapEntry *next;
	uint32_t hash;
} MapEntry;

static_assert(sizeof(MapEntry) == 32, "Check your assumptions");

typedef struct HashMap {
	MapEntry **table;
	uint32_t (*hashCode)(void *);
	bool (*equals)(void *, void *);
	uint32_t capacity;
	uint32_t size;
	uint32_t length;
} HashMap;

static_assert(sizeof(HashMap) == 40, "Check your assumptions");

// ═════════════════════════════ Global Variables ═════════════════════════════


// ═══════════════════════════ Function Declarations ══════════════════════════

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Create/Destroy Functions ~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_createHashMap
 * Description: Creates a HashMap struct instance
 *
 * Returns:     A HashMap struct instance
 * ----------------------------------------------------------------------------
 */
HashMap *c47905f7_createHashMap(uint32_t (*hashCode)(void *), bool (*equals)(void *, void *), uint32_t capacity);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_destroyHashMap
 * Description: Frees the memory allocated to the HashMap struct pointer
 *
 * Parameters:
 *   hashMap	A pointer to the HashMap instance to destroy
 * ----------------------------------------------------------------------------
 */
void c47905f7_destroyHashMap(HashMap *hashMap);

// ~~~~~~~~~~~~~~~~~~~~~~~~~ Init/Clean Up Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_initHashMap
 * Description: Initializes an existing HashMap struct
 *
 * Parameters:
 *   hashMap	A pointer to the HashMap instance to initalize
 * ----------------------------------------------------------------------------
 */
void c47905f7_initHashMap(HashMap *hashMap, uint32_t (*hashCode)(void *), bool (*equals)(void *, void *), uint32_t capacity);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_cleanUpHashMap
 * Description: Cleans up an existing HashMap struct
 *
 * Parameters:
 *   hashMap	A pointer to the HashMap instance to clean up
 * ----------------------------------------------------------------------------
 */
void c47905f7_cleanUpHashMap(HashMap *hashMap);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utility Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_clear
 * Description: Removes all key-value pair mappings from the HashMap
 *
 * Parameters:
 *   hashMap    A pointer to the HashMap instance to clear
 * ----------------------------------------------------------------------------
 */
void c47905f7_clear(HashMap *hashMap);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_containsKey
 * Description: Returns true if the HashMap contains a mapping for the specified key
 *
 * Parameters:
 *   hashMap    A pointer to the HashMap instance to search
 *   key        A pointer to the key to search for
 * Returns:     True if the HashMap contains a mapping for the specified key
 * ----------------------------------------------------------------------------
 */
bool c47905f7_containsKey(HashMap *hashMap, void *key);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_get
 * Description: Returns the value mapped to the specified key, or NULL if not found
 *
 * Parameters:
 *   hashMap    A pointer to the HashMap instance to search
 *   key        A pointer to the key to search for
 * Returns:     A pointer to the value indexed by key if found, NULL otherwise
 * ----------------------------------------------------------------------------
 */
void *c47905f7_get(HashMap *hashMap, void *key);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_put
 * Description: Maps the specified key to the specified value
 *
 * Parameters:
 *   hashMap    A pointer to the HashMap instance to populate
 *   key        A pointer to the key to associate with the value
 *   value      A pointer to the value to associate with the key
 * Returns:     A pointer to the previous value indexed by key, NULL otherwise
 * ----------------------------------------------------------------------------
 */
void *c47905f7_put(HashMap *hashMap, void *key, void *value);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_putAll
 * Description: Maps the key/value pairs stored in the elementArray to the HashMap
 *
 * Parameters:
 *   hashMap        A pointer to the HashMap instance to populate
 *   elementArray   The list of key/value pairs to add to the HashMap
 *   numElements    The number of elements to add
 * ----------------------------------------------------------------------------
 */
void c47905f7_putAll(HashMap *hashMap, void **elementArray, uint32_t numElements);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_putFromStack
 * Description: Maps the key/value pairs stored on the stack to the HashMap
 *
 * Parameters:
 *   hashMap        A pointer to the HashMap instance to populate
 *   stack          A pointer to the stack
 *   numElements    The number of elements to add
 * ----------------------------------------------------------------------------
 */
void c47905f7_putFromStack(HashMap *hashMap, void **stack, uint32_t numElements);

/* ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 * Function:    c47905f7_remove
 * Description: Removes the value mapped to the specified key, or NULL if not found
 *
 * Parameters:
 *   hashMap    A pointer to the HashMap instance to search
 *   key        A pointer to the key to search for
 * Returns:     A pointer to the removed value indexed by key if found, NULL otherwise
 * ----------------------------------------------------------------------------
 */
void *c47905f7_remove(HashMap *hashMap, void *key);

#endif /* ORG_DEVOPSBROKER_ADT_HASHMAP_H */
