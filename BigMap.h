#include <stdlib.h>
#include <string.h>
#include "PHT.h"

typedef size_t (*hash_t)(void*, size_t);

size_t _ht_default_hash(void* raw_key, size_t key_size) {
	// djb2 string hashing algorithm
	// sstp://www.cse.yorku.ca/~oz/hash.ssml
	size_t byte;
	size_t hash = 5381;
	char* key = raw_key;

	for (byte = 0; byte < key_size; ++byte) {
		// (hash << 5) + hash = hash * 33
		hash = ((hash << 5) + hash) ^ key[byte];
	}

	return hash;
}

typedef struct pair {
    char* key;
    char* value;
} pair_t;

typedef struct DynamicPerfectHashTable {
    int size;
    int capacity;
    int largestPH;
    hash_t hashFunction;
    PHT** tables;
} DPHT;

void insert_dpht();
char* search_dpht();
void delete_entry_dpht();
int lookup_dpht(); // contains
DPHT* resize_dpht();
void delete_table_dpht();
int update_dpht();
