#include "PHT.h"
#include "pair.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** Rebuilds the MPH for the current set of keys in the PHT, using CMPH.
 *
 * This function is reorders the entries array so that cmph_search() returns
 * the correct index for each key.
 *
 * \param pht Pointer to the PHT whose MPH needs to be rebuilt.
 */
static void pht_rebuild(PHT* pht) {
    if (!pht) {
        return; // Invalid PHT
    }

    if (pht->size == 0) { // No entries to rebuild
        if (pht->mph) { // Free the MPH if it exists
            cmph_destroy(pht->mph);
            pht->mph = NULL;
        }
        return;
    }

    // Allocate an array of keys from the current entries
    // CMPH vector adapter requires a constant array of strings
    const char** keys = (const char**)malloc(sizeof(char*) * pht->size);
    if (!keys) {
        return; // Memory allocation failed
    }
    for (int i = 0; i < pht->size; i++) {
        keys[i] = pht->entries[i]->key;
    }

    // Build the MPH function using CHD algorithm

    // Create an input adapter that allows CMPH to read the keys
    cmph_io_adapter_t* source = cmph_io_vector_adapter(keys, pht->size);

    // Create a new CMPH confgiuration object using the provided keys
    // Used to specify the parameters for building the MPH
    cmph_config_t* config = cmph_config_new(source);

    // Set the algorithm to CHD for building the MOH
    cmph_config_set_algo(config, CMPH_CHD);

    // No additional information should be printed during the build process
    cmph_config_set_verbosity(config, 0);

    // Create the MPH
    cmph_t* mph = cmph_new(config);

    // Destroy (free) the configuration object and the input adapter
    // as they are no longer needed after the MPH is built
    cmph_config_destroy(config);
    cmph_io_vector_adapter_destroy(source);

    // Free the keys array as it is no longer needed
    free(keys);

    // Allocate a new array of entries
    pair_t** new_entries = (pair_t**)calloc(pht->size, sizeof(pair_t*));
    if (!new_entries) {
        cmph_destroy(mph);  // Free the mph if memory allocation failed
        return;
    }

    // Populate the new entries array using the MPH
    for (int i = 0; i < pht->size; i++) {
        pair_t* entry = pht->entries[i];
        unsigned int hash = cmph_search(mph, entry->key, (cmph_uint32)strlen(entry->key));
        new_entries[hash] = entry;
    }

    free(pht->entries); // Free the old entries array
    pht->entries = new_entries; // Assign the new entries array

    // Replace the old MPH (if it exists) with the new one
    if (pht->mph) {
        cmph_destroy(pht->mph);
    }
    pht->mph = mph;
}

PHT* pht_create(int initial_capacity) {
    PHT* pht = (PHT*)malloc(sizeof(PHT));
    if (!pht) {
        return NULL; // Memory allocation failed
    }

    // Initialize the PHT with the given initial capacity and default values
    pht->capacity = initial_capacity;
    pht->size = 0;
    pht->entries = (pair_t**)malloc(sizeof(pair_t*) * initial_capacity);
    if (!pht->entries) {
        free(pht);
        return NULL; // Memory allocation failed
    }
    for (int i = 0; i < initial_capacity; i++) {
        pht->entries[i] = NULL;
    }
    pht->mph = NULL;

    return pht;
}

/** Resizes the PHT to a new capacity.
 *
 * This function reallocates the entries array to accommodate more entries.
 *
 * \param pht Pointer to the PHT to be resized.
 * \param new_capacity The new capacity for the PHT.
 * \returns 1 on success, 0 on failure (e.g., memory allocation error).
 */
static int pht_resize(PHT* pht, int new_capacity) {
    if (!pht || new_capacity <= pht->size) {
        return 0; // Invalid parameters
    }

    // Allocate a new array with the new capacity
    // Realloc resizes the memory block pointed to by pht->entries
    // and frees the old memory block if successful
    pair_t** new_entries = (pair_t**)realloc(pht->entries, sizeof(pair_t*) * new_capacity);
    if (!new_entries) {
        return 0; // Memory allocation failed
    }

    pht->entries = new_entries;
    pht->capacity = new_capacity;

    return 1; // Success
}

int pht_insert(PHT* pht, pair_t* new_pair) {
    if (!pht || !new_pair) {
        return 0; // Invalid parameters
    }

    // Resize if necessary
    if (pht->size >= pht->capacity) {
        if (!pht_resize(pht, pht->capacity * 2)) {
            return 0; // Memory allocation failed
        }
    }
    // Insert the new pair at the end of the entries array
    pht->entries[pht->size] = new_pair;
    pht->size++;

    // Rebuild the MPH and reorder the entries based on the new MPH
    pht_rebuild(pht);

    return 1;
}

char* pht_search(PHT* pht, const char* key) {
    if (!pht || !key || pht->size == 0 || !pht->mph) {
        return NULL; // Invalid PHT or key
    }

    unsigned int hash = cmph_search(pht->mph, key, (cmph_uint32)strlen(key));
    pair_t* entry = pht->entries[hash];

    if (entry && strcmp(entry->key, key) == 0) {
        return entry->value; // Key found
    }
    return NULL; // Key not found
}

int pht_lookup(PHT* pht, const char* key) {
    return (pht_search(pht, key) != NULL) ? 1 : 0;
}

int pht_update(PHT* pht, const char* key, const char* new_value) {
    if (!pht || !key || !new_value || pht->size == 0 || !pht->mph) {
        return 0; // Invalid parameters
    }

    unsigned int hash = cmph_search(pht->mph, key, (cmph_uint32)strlen(key));
    pair_t* entry = pht->entries[hash];

    if (entry && strcmp(entry->key, key) == 0) {    // Key found
        return pair_update_value(entry, new_value);
    }
    return 0; // Key not found
}

void pht_delete_entry(PHT* pht, const char* key) {
    if (!pht || !key || pht->size == 0 || !pht->mph) {
        return; // Invalid parameters
    }
    unsigned int hash = cmph_search(pht->mph, key, (cmph_uint32)strlen(key));
    pair_t* entry = pht->entries[hash];
    if (entry && strcmp(entry->key, key) == 0) {
        free_pair(entry);
        // Replace the deleted element with the last element
        // because all entries are stored in a contiguous array
        pht->entries[hash] = pht->entries[pht->size - 1];
        pht->size--;
        pht_rebuild(pht);   // Rebuild the MPH after deletion
    }
}

void pht_delete(PHT* pht) {
    if (!pht) {
        return; // Nothing to delete
    }
    // Free all entries
    for (int i = 0; i < pht->size; i++) {
        if (pht->entries[i]) {
            free_pair(pht->entries[i]);
        }
    }
    free(pht->entries); // Free the entries array
    if (pht->mph) {
        cmph_destroy(pht->mph); // Free the MPH
    }
    free(pht); // Free the PHT structure
}

PHT* pht_create_from_array(PHT* source, int new_capacity) {
    if (!source || new_capacity <= source->size) {
        return NULL; // Invalid parameters
    }
    // Create a new PHT with the specified capacity
    PHT* new_pht = pht_create(new_capacity);
    if (!new_pht) {
        return NULL; // Memory allocation failed
    }
    // Copy entries from the source PHT to the new PHT
    for (int i = 0; i < source->size; i++) {
        pair_t* entry = source->entries[i];
        if (!pht_insert(new_pht, entry)) {
            pht_delete_table(new_pht);  // Free the new PHT if insertion fails
            return NULL; // Memory allocation for entries array failed
        }
    }
    return new_pht;
}