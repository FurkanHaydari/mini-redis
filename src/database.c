#include "database.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static KeyValue *root = NULL;

// AVL Tree helper functions
static void free_tree(KeyValue *node) {
    if (node) {
        free_tree(node->left);
        free_tree(node->right);
        free(node);
    }
}

static KeyValue* insert(KeyValue *node, const char *key, const char *value);
static KeyValue* delete_node(KeyValue *root, const char *key);

void db_init() {
    root = NULL;
}

int db_set(const char *key, const char *value) {
    root = insert(root, key, value);
    return 0;
}

char* db_get(const char *key) {
    KeyValue *current = root;
    while (current) {
        int cmp = strcmp(key, current->key);
        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            return current->value;
        }
    }
    log_info("Key not found: %s", key);
    return NULL;
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
static int height(KeyValue *node) {
    return node ? node->height : 0;
}

static int max(int a, int b) {
    return (a > b) ? a : b;
}

static KeyValue* create_node(const char *key, const char *value) {
    KeyValue *node = (KeyValue *)malloc(sizeof(KeyValue));
    if (!node) {
        log_error("Memory allocation failed");
        return NULL;
    }
    strncpy(node->key, key, MAX_KEY_SIZE);
    strncpy(node->value, value, MAX_VALUE_SIZE);
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

static KeyValue* insert(KeyValue *node, const char *key, const char *value) {
    if (!node) return create_node(key, value);
    
    int cmp = strcmp(key, node->key);
    if (cmp < 0) {
        node->left = insert(node->left, key, value);
    } else if (cmp > 0) {
        node->right = insert(node->right, key, value);
    } else {
        strncpy(node->value, value, MAX_VALUE_SIZE); // Update value if key exists
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

static KeyValue* delete_node(KeyValue *root, const char *key) {
    if (!root) return root;

    int cmp = strcmp(key, root->key);
    if (cmp < 0) {
        root->left = delete_node(root->left, key);
    } else if (cmp > 0) {
        root->right = delete_node(root->right, key);
    } else {
        if (!root->left) {
            KeyValue *temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            KeyValue *temp = root->left;
            free(root);
            return temp;
        }
        KeyValue *temp = min_value_node(root->right);
        strncpy(root->key, temp->key, MAX_KEY_SIZE);
        strncpy(root->value, temp->value, MAX_VALUE_SIZE);
        root->right = delete_node(root->right, temp->key);
    }

    root->height = max(height(root->left), height(root->right)) + 1;
    int balance = get_balance(root);

    if (balance > 1 && get_balance(root->left) >= 0) {
        return right_rotate(root);
    }
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }
    if (balance < -1 && get_balance(root->right) <= 0) {
        return left_rotate(root);
    }
    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    return root;
}
