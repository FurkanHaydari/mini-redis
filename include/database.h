#ifndef DATABASE_H
#define DATABASE_H

#include "config.h"
#include <json-c/json.h>

typedef struct KeyValue {
    char key[MAX_KEY_SIZE];
    struct json_object *value;
    struct KeyValue *left;
    struct KeyValue *right;
    int height;
} KeyValue;

void db_init();
json_object* db_get(const char *key);
int db_set(const char *key, json_object *value);
int db_delete(const char *key);
void db_cleanup();


#endif // DATABASE_H
