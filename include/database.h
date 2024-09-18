#ifndef DATABASE_H
#define DATABASE_H

#include "config.h"
#include <json-c/json.h>

// Version 1.0
// Last modified: 2023-05-15

// KeyValue structure representing a node in the AVL tree
typedef struct KeyValue
{
    char key[MAX_KEY_SIZE];
    struct json_object *value;
    struct KeyValue *left;
    struct KeyValue *right;
    int height;
} KeyValue;

// Database operation function prototypes

// Initialize the database
// Returns: void
void db_init(void);

// Retrieve a value from the database
// Parameters:
//   key: The key to look up
// Returns: json_object* if found, NULL if not found
json_object *db_get(const char *key);

// Insert or update a key-value pair in the database
// Parameters:
//   key: The key to set
//   value: The value to associate with the key
// Returns: 0 on success, -1 on failure
int db_set(const char *key, json_object *value);

// Delete a key-value pair from the database
// Parameters:
//   key: The key to delete
// Returns: 0 on success, -1 if key not found
int db_delete(const char *key);

// Clean up the entire database
// Returns: void
void db_cleanup(void);

#endif // DATABASE_H
