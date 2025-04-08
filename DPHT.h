#ifndef DPHT_H
#define DPHT_H

#include <stdlib.h>
#include <string.h>
#include "PHT.h"
#include "pair.h"

/** Structure for the dynamic perfect hash table (DPHT).
 *
 * \param size The total number of key-value pairs stored in the DPHT.
 * \param capacity The number of PHT tables in the DPHT.
 * \param tables the array of PHT pointers
 */
typedef struct DynamicPerfectHashTable {
    int size;
    int capacity;
    PHT** tables;
} DPHT;

/** Creates a new Dynamic Perfect Hash Table (DPHT).
 *
 * This function allocates and initializes a DPHT structure consisting of
 * an array of smaller PHT (Perfect Hash Table) buckets.
 *
 * \param initialTables The number of PHT buckets to initialize.
 *                      If less than 1, a default value is used.
 * \returns A pointer to the newly created DPHT, or NULL if memory allocation fails.
 */

DPHT* dpht_create(int initialTables);

/** Inserts a key-value pair into the DPHT.
 *
 * This function hashes the key to determine the appropriate PHT bucket,
 * and inserts the new pair into that bucket. If the key already exists,
 * the existing value is updated instead. The DPHT may trigger a rehash
 * if the load factor exceeds the predefined threshold.
 *
 * \param dpht Pointer to the DPHT structure.
 * \param key Pointer to the key string.
 * \param value Pointer to the value string.
 * \returns 1 on success, 0 on failure (e.g., memory allocation error).
 */
int dpht_insert(DPHT* dpht, char* key, char* value);

/** Searches for a key in the DPHT.
 *
 * This function computes the hash of the given key to locate the appropriate
 * PHT bucket, and then performs a lookup within that bucket.
 *
 * \param dpht Pointer to the DPHT structure.
 * \param key Pointer to the key string to search for.
 * \returns The associated value if the key is found, or NULL otherwise.
 */
char* dpht_search(DPHT* dpht, char* key);

/** Updates the value associated with an existing key in the DPHT.
 *
 * This function hashes the key to find its corresponding PHT bucket
 * and updates the value if the key is present.
 *
 * \param dpht Pointer to the DPHT structure.
 * \param key Pointer to the key string to update.
 * \param new_value Pointer to the new value string.
 * \returns 1 if the key was found and updated, 0 otherwise.
 */
int dpht_update(DPHT* dpht, char* key, char* new_value);

/** Checks if a given key exists in the DPHT.
 *
 * This function performs a lookup for the key and returns whether it exists.
 *
 * \param dpht Pointer to the DPHT structure.
 * \param key Pointer to the key string to check.
 * \returns 1 if the key is found, 0 otherwise.
 */
int dpht_lookup(DPHT* dpht, char* key);

/** Deletes a key-value pair from the DPHT if the key exists.
 *
 * This function hashes the key to find the appropriate PHT bucket,
 * and removes the corresponding entry. The total size of the DPHT
 * is decremented if the deletion is successful.
 *
 * \param dpht Pointer to the DPHT structure.
 * \param key Pointer to the key string to delete.
 */
void dpht_remove_entry(DPHT* dpht, char* key);

/** Deletes the entire DPHT and frees all associated memory.
 *
 * This function deallocates each internal PHT bucket and
 * then releases the DPHT structure itself.
 *
 * \param dpht Pointer to the DPHT structure to delete.
 */
void dpht_free(DPHT* dpht);

#endif // DPHT_H
