#ifndef DATABASE_H
#define DATABASE_H

#include "config.h"

typedef struct KeyValue {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
    struct KeyValue *left;
    struct KeyValue *right;
    int height;
} KeyValue;

void db_init();
int db_set(const char *key, const char *value);
char* db_get(const char *key);
void db_cleanup();

#endif // DATABASE_H
