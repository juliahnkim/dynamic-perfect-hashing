/*
 * Test program for DPHT.c that performs the following:
 * 1. Inserts NUM_KEYS key-value pairs into the PHT.
 * 2. Looks up each key and verifies the value.
 * 3. Updates each key's value and verifies the new value.
 * 4. Deletes every second key and verifies that those keys are removed.
 * 5. Creates a new DPHT and checks that resizing is working properly.
 * 6. Cleans up by deleting both PHTs.
 */

#include <stdio.h>      // For printf
#include <stdlib.h>     // For malloc/free
#include <string.h>     // For strcmp
#include <assert.h>     // For assert
#include <sys/time.h>   // For time functions
#include "PHT.h"
#include "pair.h"
#include "DPHT.h"

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
    DPHT* dpht = dpht_create(16);
    assert(dpht != NULL);
    assert(dpht->size == 0);

    // 1. Insertion Test:
    // Insert NUM_KEYS key-value pairs and verify that each is correctly inserted.
    for (int i = 0; i < NUM_KEYS; i++) {
        snprintf(key, sizeof(key), "key%d", i);
        snprintf(value, sizeof(value), "value%d", i);
        start = get_time();
        int ret = dpht_insert(dpht, key, value);
        printf("%d\n", i);
        end = get_time();
        assert(ret == 1);
        total_insert += (end - start);
        // Immediately verify that the inserted key returns the correct value.
        result = dpht_search(dpht, key);
        assert(result != NULL);
        // Compare result with expected value.
        char expected[64];
        snprintf(expected, sizeof(expected), "value%d", i);
        assert(strcmp(result, expected) == 0);
    }
    printf("Insertion test passed: %d keys inserted, size = %d\n", NUM_KEYS, dpht->size);
    printf("Average insertion time: %f sec\n", total_insert / NUM_KEYS);

    // 2. Lookup Test:
    // Verify that every key in the table returns the expected value.
    for (int i = 0; i < NUM_KEYS; i++) {
        snprintf(key, sizeof(key), "key%d", i);
        start = get_time();
        result = dpht_search(dpht, key);
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
        int ret = dpht_update(dpht, key, value);
        end = get_time();
        assert(ret == 1);
        total_update += (end - start);

        result = dpht_search(dpht, key);
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
        dpht_remove_entry(dpht, key);
        end = get_time();
        total_delete += (end - start);
    }
    printf("Deletion test passed: every second key deleted.\n");
    printf("Average deletion time: %f sec\n", total_delete / (NUM_KEYS / 2));

    // Verify deletions.
    for (int i = 0; i < NUM_KEYS; i += 2) {
        snprintf(key, sizeof(key), "key%d", i);
        result = dpht_search(dpht, key);
        assert(result == NULL);
    }
    // Verify that keys not deleted still return correct updated values.
    for (int i = 1; i < NUM_KEYS; i += 2) {
        snprintf(key, sizeof(key), "key%d", i);
        result = dpht_search(dpht, key);
        assert(result != NULL);
        char expected[64];
        snprintf(expected, sizeof(expected), "new_value%d", i);
        assert(strcmp(result, expected) == 0);
    }
    printf("Verification after deletion passed.\n");

    // 5. Resizing test:
    // Test Resizing functionality
    DPHT* dpht2 = dpht_create(4);
    assert(dpht2 != NULL);
    assert(dpht2->size == 0);
    for (int i = 0; i < 20; i++) {
        snprintf(key, sizeof(key), "key%d", i);
        snprintf(value, sizeof(value), "value%d", i);
        start = get_time();
        int ret = dpht_insert(dpht2, key, value);
        end = get_time();
        assert(ret == 1);
        total_insert += (end - start);
    }
    printf("Resizing test passed: %d keys inserted, size = %d\n", 20, dpht2->size);
    printf("Average insertion time during resizing: %f sec\n", total_insert / 20);

    // Verify that all keys are still present after resizing.
    for (int i = 0; i < 20; i++) {
        snprintf(key, sizeof(key), "key%d", i);
        start = get_time();
        result = dpht_search(dpht2, key);
        end = get_time();
        assert(result != NULL);
        // Compare result with expected value.
        char expected[64];
        snprintf(expected, sizeof(expected), "value%d", i);
        if (strcmp(result, expected) != 0) {
            fprintf(stderr, "Resizing lookup error %s: expected %s, got %s\n", key, expected, result);
        }
        assert(strcmp(result, expected) == 0);
    }

    printf("Resizing test passed.\n");
    printf("Average lookup time during resizing: %f sec\n", total_lookup / 20);

    // Clean up: Delete both DPHTs.
    dpht_free(dpht2);
    dpht_free(dpht);

    printf("All DPHT tests passed successfully.\n");
    return 0;
}
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <assert.h>
// #include "DPHT.h"

// int main(void) {
//     DPHT* ht = create_dpht(4);  // Start with 4 tables
//     assert(ht != NULL);

//     int num_keys = 5;
//     char key[64];
//     char value[64];

//     // Insert keys exactly once.
//     for (int i = 0; i < num_keys; i++) {
//         snprintf(key, sizeof(key), "key%d", i);
//         snprintf(value, sizeof(value), "value%d", i);
//         int ret = insert_dpht(ht, key, value);
//         assert(ret == 1);
//         printf("Inserted: %s -> %s\n", key, value);
//     }

//     // Verify all inserted keys.
//     for (int i = 0; i < num_keys; i++) {
//         snprintf(key, sizeof(key), "key%d", i);
//         char* result = search_dpht(ht, key);
//         assert(result != NULL);
//         snprintf(value, sizeof(value), "value%d", i);
//         assert(strcmp(result, value) == 0);
//         printf("Verified: %s -> %s\n", key, result);
//     }

//     // Update one key.
//     const char* update_key = "key4";
//     int ret = dpht_update(ht, (char*)update_key, "new_value2");
//     assert(ret == 1);
//     char* result = search_dpht(ht, (char*)update_key);
//     assert(result != NULL);
//     assert(strcmp(result, "new_value2") == 0);
//     printf("Updated: %s -> %s\n", update_key, result);

//     // Delete one key.
//     const char* delete_key = "key3";
//     delete_entry_dpht(ht, (char*)delete_key);
//     result = search_dpht(ht, (char*)delete_key);
//     assert(result == NULL);
//     printf("Deleted key: %s\n", delete_key);

//     // Verify remaining keys.
//     for (int i = 0; i < num_keys; i++) {
//         snprintf(key, sizeof(key), "key%d", i);
//         if (strcmp(key, delete_key) == 0)
//             continue;
//         result = search_dpht(ht, key);
//         assert(result != NULL);
//         printf("Remaining: %s -> %s\n", key, result);
//     }

//     // Clean up.
//     delete_table_dpht(ht);
//     printf("All tests passed successfully.\n");

//     return 0;
// }
