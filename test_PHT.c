/*
 * Test program for PHT.c that performs the following:
 * 1. Inserts NUM_KEYS key-value pairs into the PHT.
 * 2. Looks up each key and verifies the value.
 * 3. Updates each key's value and verifies the new value.
 * 4. Deletes every second key and verifies that those keys are removed.
 * 5. Creates a new PHT from the current one and verifies the keys.
 * 6. Cleans up by deleting both PHTs.
 */

#include <stdio.h>      // For printf
#include <stdlib.h>     // For malloc/free
#include <string.h>     // For strcmp
#include <assert.h>     // For assert
#include <sys/time.h>   // For time functions
#include "PHT.h"
#include "pair.h"

/* Helper function: Returns the current time in seconds */
double get_time(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}

#define NUM_KEYS 20 // Number of keys to test with

int main(void) {
    char key[64], value[64];
    double start, end;
    double total_insert = 0.0, total_lookup = 0.0, total_update = 0.0, total_delete = 0.0;
    char* result;

    // Create a new PHT with an initial capacity (e.g., 4).
    PHT* pht = pht_create(4);
    assert(pht != NULL);
    assert(pht->size == 0);

    // 1. Insertion Test:
    // Insert NUM_KEYS key-value pairs and verify that each is correctly inserted.
    for (int i = 0; i < NUM_KEYS; i++) {
        snprintf(key, sizeof(key), "key%d", i);
        snprintf(value, sizeof(value), "value%d", i);
        start = get_time();
        int ret = pht_insert(pht, pair_create(key, value));
        end = get_time();
        assert(ret == 1);
        total_insert += (end - start);

        // Immediately verify that the inserted key returns the correct value.
        result = pht_search(pht, key);
        assert(result != NULL);
        // Compare result with expected value.
        char expected[64];
        snprintf(expected, sizeof(expected), "value%d", i);
        assert(strcmp(result, expected) == 0);
    }
    printf("Insertion test passed: %d keys inserted, size = %d\n", NUM_KEYS, pht->size);
    printf("Average insertion time: %f sec\n", total_insert / NUM_KEYS);

    // 2. Lookup Test:
    // Verify that every key in the table returns the expected value.
    for (int i = 0; i < NUM_KEYS; i++) {
        snprintf(key, sizeof(key), "key%d", i);
        start = get_time();
        result = pht_search(pht, key);
        end = get_time();
        assert(result != NULL);
        char expected[64];
        snprintf(expected, sizeof(expected), "value%d", i);
        if (strcmp(result, expected) != 0) {
            fprintf(stderr, "Lookup error for %s: expected %s, got %s\n", key, expected, result);
        }
        assert(strcmp(result, expected) == 0);
        total_lookup += (end - start);
    }
    printf("Lookup test passed for all %d keys.\n", NUM_KEYS);
    printf("Average lookup time: %f sec\n", total_lookup / NUM_KEYS);

    // 3. Update Test:
    // Update each key's value and verify the new value.
    for (int i = 0; i < NUM_KEYS; i++) {
        snprintf(key, sizeof(key), "key%d", i);
        snprintf(value, sizeof(value), "new_value%d", i);
        start = get_time();
        int ret = pht_update(pht, key, value);
        end = get_time();
        assert(ret == 1);
        total_update += (end - start);

        result = pht_search(pht, key);
        assert(result != NULL);
        char expected[64];
        snprintf(expected, sizeof(expected), "new_value%d", i);
        if (strcmp(result, expected) != 0) {
            fprintf(stderr, "Update error for %s: expected %s, got %s\n", key, expected, result);
        }
        assert(strcmp(result, expected) == 0);
    }
    printf("Update test passed for all %d keys.\n", NUM_KEYS);
    printf("Average update time: %f sec\n", total_update / NUM_KEYS);

    // 4. Deletion Test:
    // Delete every second key and verify that those keys are removed.
    for (int i = 0; i < NUM_KEYS; i += 2) {
        snprintf(key, sizeof(key), "key%d", i);
        start = get_time();
        pht_remove_entry(pht, key);
        end = get_time();
        total_delete += (end - start);
    }
    printf("Deletion test passed: every second key deleted.\n");
    printf("Average deletion time: %f sec\n", total_delete / (NUM_KEYS / 2));

    // Verify deletions.
    for (int i = 0; i < NUM_KEYS; i += 2) {
        snprintf(key, sizeof(key), "key%d", i);
        result = pht_search(pht, key);
        assert(result == NULL);
    }
    // Verify that keys not deleted still return correct updated values.
    for (int i = 1; i < NUM_KEYS; i += 2) {
        snprintf(key, sizeof(key), "key%d", i);
        result = pht_search(pht, key);
        assert(result != NULL);
        char expected[64];
        snprintf(expected, sizeof(expected), "new_value%d", i);
        assert(strcmp(result, expected) == 0);
    }
    printf("Verification after deletion passed.\n");

    // 5. Create-from-array Test:
    // Create a new PHT from the current one.
    int new_capacity = pht->size + 1;
    PHT* new_pht = pht_create_from_array(pht, new_capacity);
    assert(new_pht != NULL);
    assert(new_pht->size == pht->size);
    // Check each key in the new table.
    for (int i = 0; i < NUM_KEYS; i++) {
        snprintf(key, sizeof(key), "key%d", i);
        result = pht_search(new_pht, key);
        if (i % 2 == 0) { // Deleted keys
            assert(result == NULL);
        }
        else { // Keys that remain
            assert(result != NULL);
            char expected[64];
            snprintf(expected, sizeof(expected), "new_value%d", i);
            assert(strcmp(result, expected) == 0);
        }
    }
    printf("Create-from-array test passed.\n");

    // Clean up: Delete both PHTs.
    pht_delete(new_pht);
    pht_delete(pht);

    printf("All PHT tests passed successfully.\n");
    return 0;
}
