#include "database.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

static void free_node(KeyValue *node);
static void free_tree(KeyValue *node);
static int height(KeyValue *node);
static int max(int a, int b);
static KeyValue* create_node(const char *key, json_object *value);
static KeyValue* right_rotate(KeyValue *y);
static KeyValue* left_rotate(KeyValue *x);
static int get_balance(KeyValue *node);
static KeyValue* insert(KeyValue *node, const char *key, json_object *value);
static KeyValue* min_value_node(KeyValue *node);
static KeyValue* delete_node(KeyValue *node, const char *key);

static KeyValue *root = NULL;

void db_init() {
    root = NULL;
}

json_object* db_get(const char *key) {
    KeyValue *current = root;
    while (current) {
        int cmp = strcmp(key, current->key);
        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            return current->value; // Return the pointer to the JSON object
        }
    }
    log_info("Key not found: %s", key);
    return NULL; // Return NULL if the key is not found
}

int db_set(const char *key, json_object *value) {
    root = insert(root, key, value);
    return 0;
}

int db_delete(const char *key) {
    if (key == NULL) return -1;
    root = delete_node(root, key);
    return 0;
}

void db_cleanup() {
    free_tree(root);
    root = NULL;
}

// AVL Tree helper functions implementation
static void free_node(KeyValue *node) {
    // Free the memory of the node and its JSON value
    json_object_put(node->value);
    free(node);
}

static void free_tree(KeyValue *node) {
    if (node) {
        free_tree(node->left);
        free_tree(node->right);
        free_node(node);
    }
}

static int height(KeyValue *node) {
    return node ? node->height : 0;
}

static int max(int a, int b) {
    return (a > b) ? a : b;
}

static KeyValue* create_node(const char *key, json_object *value) {
    KeyValue *node = (KeyValue *)malloc(sizeof(KeyValue));
    if (!node) {
        log_error("Memory allocation failed");
        return NULL;
    }
    strncpy(node->key, key, MAX_KEY_SIZE);
    node->value = value;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

static KeyValue* right_rotate(KeyValue *y) {
    KeyValue *x = y->left;
    KeyValue *T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    
    return x;
}

static KeyValue* left_rotate(KeyValue *x) {
    KeyValue *y = x->right;
    KeyValue *T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    
    return y;
}

static int get_balance(KeyValue *node) {
    return node ? height(node->left) - height(node->right) : 0;
}

static KeyValue* insert(KeyValue *node, const char *key, json_object *value) {
    if (!node) return create_node(key, value);
    
    int cmp = strcmp(key, node->key);
    if (cmp < 0) {
        node->left = insert(node->left, key, value);
    } else if (cmp > 0) {
        node->right = insert(node->right, key, value);
    } else {
        // cmp = 0 means key found
        json_object_put(node->value); // Decrease old json object reference counter 
        node->value = value; //update value
        return node;
    }

    node->height = max(height(node->left), height(node->right)) + 1;
    int balance = get_balance(node);
    
    if (balance > 1 && strcmp(key, node->left->key) < 0) {
        return right_rotate(node);
    }
    if (balance < -1 && strcmp(key, node->right->key) > 0) {
        return left_rotate(node);
    }
    if (balance > 1 && strcmp(key, node->left->key) > 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    if (balance < -1 && strcmp(key, node->right->key) < 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

static KeyValue* min_value_node(KeyValue *node) {
    KeyValue *current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

static KeyValue* delete_node(KeyValue *node, const char *key) {
    if (!node) return node;

    int cmp = strcmp(key, node->key);
    if (cmp < 0) {
        node->left = delete_node(node->left, key);
    } else if (cmp > 0) {
        node->right = delete_node(node->right, key);
    } else {
        if (!node->left) {
            KeyValue *temp = node->right;
            free_node(node);
            return temp;
        } else if (!node->right) {
            KeyValue *temp = node->left;
            free_node(node);
            return temp;
        }
        KeyValue *temp = min_value_node(node->right);
        strncpy(node->key, temp->key, MAX_KEY_SIZE);
        node->value = temp->value;
        node->right = delete_node(node->right, temp->key);
    }

    node->height = max(height(node->left), height(node->right)) + 1;
    int balance = get_balance(node);

    if (balance > 1 && get_balance(node->left) >= 0) {
        return right_rotate(node);
    }
    if (balance > 1 && get_balance(node->left) < 0) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    if (balance < -1 && get_balance(node->right) <= 0) {
        return left_rotate(node);
    }
    if (balance < -1 && get_balance(node->right) > 0) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}
