#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "DPHT.h"

/**
 * Example main program simulating a network device's flow table using a DPHT.
 *
 * In network processing, a flow is a group of packets that share common header fields.
 * The flow table stores a flow entry for each flow, where:
 *    - The **flow key** (e.g., "flow_1234") is derived from the packet headers.
 *    - The **flow value** (e.g., "next_hop_1234") contains metadata such as the processing
 *      action or next-hop information.
 *
 * The flow table (implemented via a Dynamic Perfect Hash Table) does not store the actual packets;
 * it stores only the metadata required to quickly decide how to process each arriving packet.
 *
 * This program:
 *   1. Creates a DPHT to simulate a flow table.
 *   2. Inserts many flow entries (each representing a flow) into the DPHT.
 *   3. Looks up flows to simulate the per-packet matching process.
 *   4. Updates certain flow entries to reflect dynamic network changes.
 *   5. Deletes selected flow entries, simulating flow expiry or re-routing.
 *   6. Prints timing and status information.
 *
 * \returns 0 on successful execution.
 */
int main(void) {
    const int NUM_FLOW_ENTRIES = 10000;  // Number of distinct flow entries to simulate
    clock_t start, end;
    double cpu_time_used;

    // 1. Create a DPHT to simulate the flow table.
    // The DPHT will hold flow entries (not the packet data) for fast lookup.
    DPHT* flowTable = dpht_create(256);
    if (!flowTable) {
        fprintf(stderr, "Error: Could not create the DPHT for flow entries\n");
        return EXIT_FAILURE;
    }
    printf("Flow Table (DPHT) created with initial capacity: %d buckets\n", flowTable->capacity);

    // 2. Insert flow entries into the flow table.
    // In practice, a flow key is built from packet header fields,
    // but here we simulate by using "flow_<id>" and assign a next hop value "next_hop_<id>".
    start = clock();
    for (int i = 0; i < NUM_FLOW_ENTRIES; i++) {
        char flowKey[64];
        char nextHop[64];

        // The flow key uniquely identifies the flow (group of packets).
        snprintf(flowKey, sizeof(flowKey), "flow_%d", i);
        // The flow value contains metadata (such as next-hop or action), not the packet itself.
        snprintf(nextHop, sizeof(nextHop), "next_hop_%d", i);

        // Insert the flow entry into the DPHT (flow table)
        if (!dpht_insert(flowTable, flowKey, nextHop)) {
            fprintf(stderr, "Insertion error for flow key: %s\n", flowKey);
        }
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Inserted %d flow entries in %f seconds.\n", NUM_FLOW_ENTRIES, cpu_time_used);

    // 3. Lookup flows to simulate matching an incoming packet's header.
    // For each incoming packet, the device extracts its flow key,
    // then looks it up in the DPHT to decide what action to take.
    start = clock();
    int lookupSuccess = 0;
    for (int i = 0; i < NUM_FLOW_ENTRIES; i++) {
        char flowKey[64];
        snprintf(flowKey, sizeof(flowKey), "flow_%d", i);

        // Retrieve the next-hop (or action) associated with the flow.
        char* action = dpht_search(flowTable, flowKey);
        if (action) {
            lookupSuccess++;
        }
        else {
            fprintf(stderr, "Lookup failed for flow key: %s\n", flowKey);
        }
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Looked up %d flows in %f seconds.\n", lookupSuccess, cpu_time_used);

    // 4. Update selected flow entries to simulate routing or policy changes.
    // For instance, when a route changes, a flow's next hop might be updated.
    start = clock();
    for (int i = 0; i < NUM_FLOW_ENTRIES; i += 2) { // Update every other flow entry
        char flowKey[64], newNextHop[64];
        snprintf(flowKey, sizeof(flowKey), "flow_%d", i);
        snprintf(newNextHop, sizeof(newNextHop), "next_hop_%d_updated", i);

        if (!dpht_update(flowTable, flowKey, newNextHop)) {
            fprintf(stderr, "Update failed for flow key: %s\n", flowKey);
        }
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Updated %d flow entries in %f seconds.\n", NUM_FLOW_ENTRIES / 2, cpu_time_used);

    // 5. Delete selected flow entries to simulate flow expiry.
    // For example, remove flows that are no longer active or have been replaced.
    start = clock();
    int deleteCount = 0;
    for (int i = 0; i < NUM_FLOW_ENTRIES; i++) {
        if (i % 3 == 0) { // Delete flows with indices that are multiples of 3
            char flowKey[64];
            snprintf(flowKey, sizeof(flowKey), "flow_%d", i);
            dpht_remove_entry(flowTable, flowKey);
            deleteCount++;
        }
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Deleted %d flow entries in %f seconds.\n", deleteCount, cpu_time_used);

    // 6. Final status: Output the final number of flow entries stored in the table.
    printf("Final number of flow entries in the table: %d\n", flowTable->size);

    // 7. Cleanup: Free all allocated memory by deleting the flow table.
    dpht_free(flowTable);
    printf("Flow table deleted. All resources have been freed.\n");

    return 0;
}
