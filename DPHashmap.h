#include <cmph.h>

typedef struct pair {
    char* key;
    char* value;
} pair_t;


typedef struct PerfectHashTable {
    int size;
    int capacity;
    cmph_t* mph;
    pair_t** elements;
} PHT;

typedef int(*hashFunctionPointer)(char*, int);

typedef struct DynamicPerfectHashTable {
    int size;
    int capacity;
    hashFunctionPointer hashFunction;
    PHT** tables;
} DPHT;

void insert();
char* search();
void delete_entry();
int lookup(); // contains
PHT* resize();
DPHT* resize();
void delete_table();
int update();





