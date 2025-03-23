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

void insert_dph();
char* search_dph();
void delete_entry_dph();
int lookup_dph(); // contains
DPHT* resize_dph();
void delete_table_dph();
int update_dph();
