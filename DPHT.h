#ifndef DPHT_H
#define DPHT_H

#include <stdlib.h>
#include <string.h>
#include "PHT.h"
#include "pair.h"

/** Typedef for the hashfunction pointer
* this allows the function to be more easily called later
 */

typedef size_t (*hash_t)(void*, size_t);

/** Hash function for the upper level of the DPHT
 *
 * This function generates a hash value from the key passed to it.
 * param: raw_key which is the string being hashed
 * param: key_size which is the length of the string
 * returns: the hash value in a size_t
 */

size_t _ht_default_hash(void* raw_key, size_t key_size);

/** Hash function for the upper level of the DPHT
 *
 * This function generates a hash value from the key passed to it.
 * param: raw_key which is the string being hashed
 * param: key_size which is the length of the string
 * returns: the hash value in a size_t
 */

/** Function to get the index of a key
 *
 * This function generates a index from the key passed to it.
 * param: ht the pointer to the hashtable
 * param: key the string that is the key
 * returns: the index in a size_t
 */

size_t _ht_index_find(DPHT* ht, void* key);

/** Function to get the index of a key
 *
 * This function generates a index from the key passed to it.
 * \param ht The pointer to the hashtable
 * \param key The string that is the key
 * \returns The index in a size_t
 */

typedef struct DynamicPerfectHashTable {
    int size;
    int capacity;
    int largestPH;
    hash_t hashFunction;
    PHT** tables;
} DPHT;

DPHT* create_dpht(int buckets);

void insert_dpht(DPHT* ht, char* key, char* value);

char* search_dpht(DPHT* ht, char* key);

void delete_entry_dpht(DPHT* ht, char* key);

int lookup_dpht(PHT* ht, char* key);

void resize_dpht(DPHT* ht);

void delete_table_dpht(DPHT* ht);

int update_dpht(DPHT* ht, char* key, char* value);
