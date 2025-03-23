#include <stdlib.h>
#include <string.h>
#include "PHT.h"

typedef struct pair {
    char* key;
    char* value;
} pair_t;

typedef struct DynamicPerfectHashTable {
    int size;
    int capacity;
    int largestPH;
    hashFunctionPointer hashFunction;
    PHT** tables;
} DPHT;

void insert_dpht();
char* search_dpht();
void delete_entry_dpht();
int lookup_dpht(); // contains
DPHT* resize_dpht();
void delete_table_dpht();
int update_dpht();
