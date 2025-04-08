#include "DPHT.h"
#include "PHT.h"
#include "pair.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_INITIAL_TABLES 16   // Default number of tables
#define DEFAULT_PHT_CAPACITY 4      // Initial capacity for each PHT table
#define LOAD_FACTOR_THRESHOLD 5.0   // Average keys per table before rehashing

/** Hash function for the DPHT.
 *
 * This function computes a hash value for the given key using the
 * djb2 algorithm. The hash value is used to determine the index of
 * the PHT table in which the key-value pair will be stored.
 *
 * \param key Pointer to the key string.
 * \returns The computed hash value.
 */
static size_t dpht_hash(const char* key) {
    size_t hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    return hash;
}

DPHT* dpht_create(int initialTables) {
    // Set default initial tables if the input is invalid
    if (initialTables < 1) {
        initialTables = DEFAULT_INITIAL_TABLES;
    }

    DPHT* dpht = malloc(sizeof(DPHT));
    if (!dpht) {
        return NULL; // Memory allocation failure
    }

    dpht->capacity = initialTables;
    dpht->size = 0;
    dpht->tables = malloc(sizeof(PHT*) * dpht->capacity);
    if (!dpht->tables) {
        free(dpht);
        return NULL; // Memory allocation failure
    }

    // Initialize each PHT table in the DPHT
    for (int i = 0; i < dpht->capacity; i++) {
        dpht->tables[i] = pht_create(DEFAULT_PHT_CAPACITY);
        if (!dpht->tables[i]) {
            dpht_free(dpht);
            return NULL;
        }
    }
    return dpht;
}

/** Rehashes the DPHT by doubling its number of PHT buckets.
 *
 * This function reallocates the internal array of PHT pointers in the DPHT,
 * and redistributes all key-value pairs into the new PHTs using the updated capacity.
 * Each key is rehashed into a new bucket based on the updated hash modulus.
 *
 * \param dpht Pointer to the DPHT to be rehashed.
 *             The original tables and entries will be freed after rehashing.
 */
static void dpht_rehash(DPHT* dpht) {
    int oldCapacity = dpht->capacity;
    int newCapacity = oldCapacity * 2;

    // Allocate new tables array
    PHT** newTables = malloc(sizeof(PHT*) * newCapacity);
    if (!newTables)
        return; // Leave DPHT unchanged on allocation failure

    // Initialize each new PHT
    for (int i = 0; i < newCapacity; i++) {
        newTables[i] = pht_create(DEFAULT_PHT_CAPACITY);
        if (!newTables[i]) { // Memory allocation failure
            for (int j = 0; j < i; j++) {
                pht_delete(newTables[j]);
            }
            free(newTables);
            return;
        }
    }

    // Rehash all entries from the old tables into the new tables
    for (int i = 0; i < oldCapacity; i++) {
        PHT* oldTable = dpht->tables[i];

        // Iterate through each entry in the old table
        for (int j = 0; j < oldTable->size; j++) {
            pair_t* entry = oldTable->entries[j];

            if (entry) {
                // Duplicate the pair so the new tables own their copy
                pair_t* dupEntry = pair_create(entry->key, entry->value);
                if (!dupEntry) {
                    continue; // Memory allocation failed, skip this entry
                }
                // Calculate the new hash value and insert into the new table
                size_t hashValue = dpht_hash(entry->key);
                int newIndex = hashValue % newCapacity;
                int status = pht_insert(newTables[newIndex], dupEntry);

                if (!status) {
                    pair_free(dupEntry); // Insertion failed, free the duplicate
                }
            }
        }
        pht_delete(oldTable); // Free the old table
    }
    // Replace the old tables with the new ones
    free(dpht->tables);
    dpht->tables = newTables;
    dpht->capacity = newCapacity;
}

int dpht_insert(DPHT* dpht, char* key, char* value) {
    // Validate input parameters
    if (!dpht || !key || !value) {
        return 0;
    }

    // Compute the hash value and map it to the appropriate table index
    size_t hashValue = dpht_hash(key);
    int index = hashValue % dpht->capacity;

    // If the key already exists, update the value
    PHT* table = dpht->tables[index];
    if (pht_lookup(table, key)) {
        return pht_update(table, key, value);
    }

    // If the key does not exist, create a new pair and insert it
    pair_t* newPair = pair_create(key, value);
    if (!newPair) {
        return 0; // Memory allocation failure
    }

    int status = pht_insert(table, newPair);
    if (status) { // Successful insertion
        dpht->size++;

        // Check the load factor and rehash if necessary
        float currentLoad = (float)dpht->size / dpht->capacity;
        if (currentLoad > LOAD_FACTOR_THRESHOLD) {
            dpht_rehash(dpht);
        }
    }
    return status;
}

char* dpht_search(DPHT* dpht, char* key) {
    // Validate input parameters
    if (!dpht || !key) {
        return NULL;
    }

    // Compute the hash value and map it to the appropriate table index
    size_t hashValue = dpht_hash(key);
    int index = hashValue % dpht->capacity;

    // Delegate the search to the appropriate PHT
    return pht_search(dpht->tables[index], key);
}

int dpht_update(DPHT* dpht, char* key, char* new_value) {
    // Validate input parameters
    if (!dpht || !key || !new_value) {
        return 0;
    }

    // Locate the PHT bucket for the given key
    size_t hashValue = dpht_hash(key);
    int index = hashValue % dpht->capacity;
    PHT* table = dpht->tables[index];

    // Special case: optimize for size-1 table with no MPH
    if (table->size == 1 && table->mph == NULL) {
        if (strcmp(table->entries[0]->key, key) == 0)
            return pair_update_value(table->entries[0], new_value);
        return 0;
    }

    // Delegate the update to the appropriate PHT
    return pht_update(table, key, new_value);
}

int dpht_lookup(DPHT* dpht, char* key) {
    return (dpht_search(dpht, key) != NULL) ? 1 : 0;
}

void dpht_remove_entry(DPHT* dpht, char* key) {
    // Validate input parameters
    if (!dpht || !key) {
        return;
    }

    // Locate the PHT bucket for the given key
    size_t hashValue = dpht_hash(key);
    int index = hashValue % dpht->capacity;
    PHT* table = dpht->tables[index];

    // If the key exists in the table, delete it and decrement size
    if (pht_lookup(table, key)) {
        pht_remove_entry(table, key);
        dpht->size--;
    }
}

void dpht_free(DPHT* dpht) {
    if (!dpht) {
        return; // Nothing to delete
    }

    // Delete each PHT table in the DPHT
    for (int i = 0; i < dpht->capacity; i++) {
        pht_delete(dpht->tables[i]);
    }

    // Free the array of PHT pointers and the DPHT structure itself
    free(dpht->tables);
    free(dpht);
}
