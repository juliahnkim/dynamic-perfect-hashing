#ifndef PAIR_H
#define PAIR_H

/** Structure representing key-value pair.
 *
 * Both key and value are dynamically allocated strings.
 */
typedef struct pair {
    char* key;    // Pointer to the key string
    char* value;  // Pointer to the associated value string
} pair_t;

/** Creates a new pair_t structure and initializes it with the given key and value.
 *
 * \param key Pointer to the key string.
 * \param value Pointer to the associated value string.
 * \returns A pointer to the newly created pair_t structure, or NULL on failure.
 */
pair_t* pair_create(const char* key, const char* value);

/** Updates the value in a key-value pair.
 *
 * \param pair Pointer to the pair_t structure to be updated.
 * \param new_value Pointer to the new value string.
 * \returns 1 on success, 0 on failure (e.g., key not found).
 */
int pair_update_value(pair_t* pair, const char* new_value);

/** Frees the memory allocated for a pair_t structure
 *
 * \param pair Pointer to the pair_t structure to be freed.
 */
void pair_free(pair_t* pair);

#endif
