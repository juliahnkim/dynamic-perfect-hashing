#include "DPHT.h"
#include "PHT.h"
#include "pair.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

size_t _ht_index_find(DPHT* ht, void* key){
	size_t index;
	index = ht->hashFunction(key, strlen(key));
	index = index % ht->capacity;
	return index;
}

typedef struct DynamicPerfectHashTable {
    int size;
    int capacity;
    int largestPH;
    hash_t hashFunction;
    PHT** tables;
} DPHT;

DPHT* create_dpht(int buckets){
	DPHT* dpht = (DPHT*)malloc(sizeof(DPHT));
	dpht->size=0;
	dpht->capacity=buckets;
	dpht->largestPH=0;
	dpht->hashFunction=&_ht_default_hash;
	dpht->tables=(PHT**)malloc(sizeof(PHT*)*buckets);
	for(i=0; i<buckets; i++){
		dpht->tables[i]=pht_create(4);
	}
}

int insert_dpht(DPHT* ht, char* key, char* value){
	if(ht==NULL){
		return;
	}
	size_t index = _ht_index_find(ht, key);
	pair_t* newPair=pair_create(key, value);
	if(newPair==NULL){
		return;
	}

	if(ht->size+1>ht->capacity*100){
		resize_dpht(ht);
	}
	int ret_val;
	ret_val=pht_insert(ht->tables[index], newPair);
	ht->size+=1;
	if(ht->tables[index]->size > ht->largestPH){
		ht->largestPH=ht->tables[index]->size
	}
	return ret_val;
	
}

char* search_dpht(DPHT* ht, char* key){
	size_t index = _ht_index_find(ht, key);
	return pht_search(ht->tables[index], key);
}

void delete_entry_dpht(DPHT* ht, char* key){
	size_t index = _ht_index_find(ht, key);
	return pht_delete_entry(ht->tables[index], key);
}

int lookup_dpht(PHT* ht, char* key){
	size_t index = _ht_index_find(ht, key);
	return pht_lookup(ht->tables[index], key);
}

void resize_dpht(DPHT* ht){
	PHT** tempArray= (PHT**)malloc(sizeof(PHT*)*ht->capacity*3);
	int i;
	int j;
	for(i=0; i<ht->capacity*3; i++){
		tempArray[i]->elements=(pair_t**)malloc(sizeof(pair_t*)*ht->largestPH);
		tempArray[i]->size=0;
	}
	int prev_cap= ht->capacity;
	ht->capacity=ht->capacity*3;
	size_t index;
	for(i=0; i<prev_cap; i++){
		for(j=0; j<ht->tables[i]->capacity;j++){
			if(ht->tables[i]->elements[j]==NULL){
				continue;
			}
			index = _ht_index_find(ht, ht->tables[i]->elements[j]->key);
			tempArray[index]->elements[tempArray[index]->size]=ht->tables[i]->elements[j];
			tempArray[index]->size+=1;
		}
	}
	for(i=0; i<prev_cap; i++){
		pht_delete(ht->tables[i]);
	}
	ht->tables=(PHT**)malloc(sizeof(PHT*)*capacity;
	for(i=0; i<ht->capacity; i++){
		ht->tables[i] = pht_create_from_array(tempArray[i], ht->largestPH);
	}

	for(i=0; i<ht->capacity; i++){
		pht_delete(tempArray[i]);
	}
	free(tempArray);
	ht->largestPH=ht->tables[0]->size;
	for(i=0; i<ht->capacity; i++){
		if(ht->tables[i]->size>ht->largestPH){
			ht->largestPH=ht->tables[i]->size;
		}
	}
}

void delete_table_dpht(DPHT* ht){
	int i;
	for(i=0; i<ht->capacity; i++){
		delete_table_pht(ht->tables[i]);
	}
	free(ht);
}

int update_dpht(DPHT* ht, char* key, char* value){
	size_t index = _ht_index_find(ht, key);
	return pht_update(ht->tables[index], key);

}
