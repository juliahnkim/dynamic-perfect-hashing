#include "DPHT.h"
#include "PHT.h"
#include "pair.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DPHT_DEFAULT_CAPACITY 10

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

DPHT* create_dpht(int buckets){
	DPHT* dpht = (DPHT*)malloc(sizeof(DPHT));
	dpht->size=0;
	if(DPHT_DEFAULT_CAPACITY>buckets){
		dpht->capacity=DPHT_DEFAULT_CAPACITY;
	}
	else{
	dpht->capacity=buckets;
	}
	dpht->hashFunction=&_ht_default_hash;
	dpht->tables=(PHT**)malloc(sizeof(PHT*)*buckets);
	for(int i=0; i<buckets; i++){
		dpht->tables[i]=pht_create(4);
    }
    return dpht;
}

int insert_dpht(DPHT* ht, char* key, char* value){
	if(ht==NULL){
		return 0;
	}
	size_t index = _ht_index_find(ht, key);
	int exists = pht_lookup(ht->tables[index], key);
	if(exists==1){
		return dpht_update(ht, key, value);
	}
	pair_t* newPair=pair_create(key, value);
	if(newPair==NULL){
		return 0;
	}

	if((ht->size+1)>((ht->capacity)*100)){
		resize_dpht(ht);
		index = _ht_index_find(ht, key);
	}
	int ret_val;
    ret_val = pht_insert(ht->tables[index], newPair);
    
    if (ret_val == 0) {
        pair_free(newPair);
        return 0;
    }
	ht->size++;
	return ret_val;
	
}

int dpht_update(DPHT* ht, char* key, char* new_value) {
	if(ht==NULL){
		return 0;
	}
	size_t index = _ht_index_find(ht, key);
	int exists = pht_lookup(ht->tables[index], key);
	if(exists==0){
		return 0;
	}
	return pht_update(ht->tables[index], key, new_value);

}

char* search_dpht(DPHT* ht, char* key){
	size_t index = _ht_index_find(ht, key);
	return pht_search(ht->tables[index], key);
}

void delete_entry_dpht(DPHT* ht, char* key){
	size_t index = _ht_index_find(ht, key);
	int exists = pht_lookup(ht->tables[index], key);
    pht_delete_entry(ht->tables[index], key);
	if(exists==1){
		ht->size--;
	}
}

int lookup_dpht(DPHT* ht, char* key){
	size_t index = _ht_index_find(ht, key);
	return pht_lookup(ht->tables[index], key);
}

void resize_dpht(DPHT* ht){
	PHT** tempArray= (PHT**)malloc(sizeof(PHT*)*(ht->capacity)*3);
	int i;
	int j;
	int largestPH=ht->tables[0]->size;
	for(i=0; i<ht->capacity; i++){
		if(ht->tables[i]->size>largestPH){
			largestPH=ht->tables[i]->size;
		}
	}
		
    for (i = 0; i < ht->capacity * 3; i++) {
        tempArray[i] = pht_create(largestPH);
    }
	int prev_cap= ht->capacity;
	ht->capacity=ht->capacity*3;
	size_t index;
	for(i=0; i<prev_cap; i++){
		for(j=0; j<ht->tables[i]->capacity;j++){
			if(ht->tables[i]->entries[j]==NULL){
				continue;
			}
			index = _ht_index_find(ht, ht->tables[i]->entries[j]->key);
			tempArray[index]->entries[tempArray[index]->size]=ht->tables[i]->entries[j];
			tempArray[index]->size+=1;
		}
	}
	for(i=0; i<prev_cap; i++){
		free(ht->tables[i]);
    }
    free(ht->tables);
    ht->tables = (PHT**)malloc(sizeof(PHT*) * ht->capacity);
	for(i=0; i<ht->capacity; i++){
		ht->tables[i] = pht_create_from_array(tempArray[i], tempArray[i]->size);
	}

	for(i=0; i<ht->capacity; i++){
		free(tempArray[i]);
	}
	free(tempArray);
}

void delete_table_dpht(DPHT* ht){
	int i;
	for(i=0; i<ht->capacity; i++){
		pht_delete(ht->tables[i]);
	}
	free(ht);
}

int update_dpht(DPHT* ht, char* key, char* value){
	size_t index = _ht_index_find(ht, key);
	return pht_update(ht->tables[index], key, value);
}
