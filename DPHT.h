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
 * \param raw_key The string being hashed
 * \param key_size The length of the string
 * \returns The hash value in a size_t
 */

size_t _ht_default_hash(void* raw_key, size_t key_size);

/** Function to get the index of a key
 *
 * This function generates a index from the key passed to it.
 * \param ht the pointer to the hashtable
 * \param key the string that is the key
 * \returns the index in a size_t
 */

size_t _ht_index_find(DPHT* ht, void* key);

/** Struct of the DPHT
 *
 * \param size the integer representing the number of elements in the table
 * \param capacity the size of the PHT* array
 * \param largestPH the integer size of the largest PHT table
 * \param hashFunction the pointer to the hash function in use
 * \param tables the array of PHT pointers
 */

typedef struct DynamicPerfectHashTable {
    int size;
    int capacity;
    int largestPH;
    hash_t hashFunction;
    PHT** tables;
} DPHT;

/** Function to create a DPHT
 *
 * This function creates a DPHT
 * \param buckets the size of the array to be created
 * \returns the pointer to the DPHT created by the function
 */

DPHT* create_dpht(int buckets);

/** Function to insert a key-value pair into a DPHT
 *
 * \param ht the pointer to the DPHT
 * \param key The string that is the key
 * \param value The key that is the value
 * \returns an integer indicating success or failure
 */

int insert_dpht(DPHT* ht, char* key, char* value);

/** Function to search for the value associated with a key
 *
 * \param ht the pointer to the DPHT
 * \param key The string that is the key
 * \returns a string that is the value associated with the key
 */

char* search_dpht(DPHT* ht, char* key);

/** Function to delete a key-value pair from a DPHT
 *
 * \param ht the pointer to the DPHT
 * \param key The string that is the key
 */

void delete_entry_dpht(DPHT* ht, char* key);

/** Function to check if a key is in the table
 *
 * \param ht the pointer to the DPHT
 * \param key The string that is the key
 * \returns an int representing if the key is present
 */

int lookup_dpht(DPHT* ht, char* key);

/** Function to resize a DPHT
 *
 * \param ht the pointer to the DPHT
 */

void resize_dpht(DPHT* ht);

/** Function to delete a DPHT
 *
 * \param ht the pointer to the DPHT
 */

void delete_table_dpht(DPHT* ht);

/** Function to update a key-value pair in a DPHT
 *
 * \param ht the pointer to the DPHT
 * \param key The string that is the key
 * \param value The key that is the value
 * \returns an integer indicating success or failure
 */

int update_dpht(DPHT* ht, char* key, char* value);

#endif DPHT_H
