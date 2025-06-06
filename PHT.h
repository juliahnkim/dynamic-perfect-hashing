#ifndef PHT_H
#define PHT_H

#include "cmph.h"
#include "pair.h"

/** Structure for the small perfect hash table (bucket) that uses CMPH.
 *
 * \param mph Pointer to the minimal perfect hash function object generated by CMPH.
 * \param entries Array of pointers to key-value pairs.
 *                This array is ordered so that for each key, cmph_search()
 *                returns its unique index.
 * \param size Current number of key-value pairs stored in this bucket.
 * \param capacity Allocated capacity of the entries array.
 */
typedef struct PerfectHashTable {
    cmph_t* mph;
    pair_t** entries;
    int size;
    int capacity;
} PHT;

/** Creates a new perfect hash table (PHT) with the given initial capacity.
 *
 * This function allocates memory for a PHT structure and initializes its
 * internal array of key-value pairs to be empty. The MPH function is initially NULL.
 *
 * \param initial_capacity The initial capacity of the PHT.
 * \returns A pointer to the newly created PHT, or NULL on failure.
 */
PHT* pht_create(int initial_capacity);

/** Inserts a new key-value pair into the perfect hash table.
 *
 * This function appends a new key-value pair to the internal array.
 * If the capacity is exceeded, the array is resized. The MPH is invalidated
 * and will be rebuilt lazily on the next search or update operation.
 *
 * \param pht Pointer to the PHT where the key-value pair will be inserted.
 * \param new_pair Pointer to the key-value pair to be inserted.
 * \returns 1 on success, 0 on failure (e.g., memory allocation error).
 */
int pht_insert(PHT* pht, pair_t* new_pair);

/** Updates the value of an existing key in the PHT.
 *
 * \param pht Pointer to the PHT where the key-value pair will be updated.
 * \param key The key string whose value is to be updated.
 * \param new_value The new value string to be associated with the key.
 * \returns 1 on success, 0 on failure (e.g., key not found).
 */
int pht_update(PHT* pht, const char* key, const char* new_value);

/** Searches for a given key in the PHT.
 *
 * This function locates the value associated with a given key using the MPH.
 * If the MPH has not yet been built or is outdated, it will be rebuilt automatically.
 *
 * \param pht Pointer to the PHT where the key will be searched.
 * \param key The key string to search for.
 * \returns Pointer to the value associated with the key if found, NULL otherwise.
 */
char* pht_search(PHT* pht, const char* key);

/** Checks if a key exists in the PHT.
 *
 * \param pht Pointer to the PHT where the key will be checked.
 * \param key The key string to check for.
 * \returns 1 if the key exists, 0 if it does not.
 */
int pht_lookup(PHT* pht, const char* key);

/** Deletes a key-value pair from the PHT based on its key.
 *
 * This function frees the memory of the deleted pair, updates the internal array,
 * and invalidates the MPH so it is rebuilt on the next access.
 *
 * \param pht Pointer to the PHT where the key-value pair will be deleted.
 * \param key The key string of the pair to be deleted.
 */
void pht_remove_entry(PHT* pht, const char* key);

/** Frees all memory associated with a perfect hash table.
 *
 * This function deletes all key-value pairs, destroys the MPH (if present),
 * and frees the table structure itself.
 *
 * \param pht Pointer to the PHT to be destroyed.
 */
void pht_delete(PHT* pht);

/** Creates a new PHT by copying the contents of an existing PHT.
*
* This is useful when resizing the PHT to a larger capacity.
*
* \param source        Pointer to the source PHT to be copied.
* \param new_capacity  The new capacity for the copied PHT.
* \returns A pointer to the newly created PHT, or NULL on failure.
*/
PHT* pht_create_from_array(PHT* source, int new_capacity);

#endif // PHT_H