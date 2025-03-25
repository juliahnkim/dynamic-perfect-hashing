#include "pair.h"
#include <stdlib.h>
#include <string.h>

pair_t* pair_create(const char* key, const char* value) {
    if (!key || !value) {
        return NULL; // Invalid input
    }

    pair_t* new_pair = (pair_t*)malloc(sizeof(pair_t));
    if (!new_pair) {
        return NULL;    // Memory allocation failed
    }

    new_pair->key = strdup(key);
    new_pair->value = strdup(value);

    return new_pair;
}

int pair_update_value(pair_t* pair, const char* new_value) {
    if (!pair || !new_value) {
        return 0;   // Invalid input
    }

    char* temp = strdup(new_value);

    free(pair->value);
    pair->value = temp;

    return 1;
}

void pair_free(pair_t* pair) {
    if (!pair) return;
    free(pair->key);
    free(pair->value);
    free(pair);
}