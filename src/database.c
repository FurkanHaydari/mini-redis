// database.c - Implementation of the in-memory database using an AVL tree

#include "database.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

// Function prototypes for AVL tree operations
static void free_node(KeyValue *node);
static void free_tree(KeyValue *node);
static int height(KeyValue *node);
static int max(int a, int b);
static KeyValue *create_node(const char *key, json_object *value);
static KeyValue *right_rotate(KeyValue *y);
static KeyValue *left_rotate(KeyValue *x);
static int get_balance(KeyValue *node);
static KeyValue *insert(KeyValue *node, const char *key, json_object *value);
static KeyValue *min_value_node(KeyValue *node);
static KeyValue *delete_node(KeyValue *node, const char *key);

// Root of the AVL tree
static KeyValue *root = NULL;

// Initialize the database
void db_init()
{
    root = NULL;
}

// Retrieve a value from the database
json_object *db_get(const char *key)
{
    KeyValue *current = root;
    while (current)
    {
        int cmp = strcmp(key, current->key);
        if (cmp < 0)
            current = current->left;
        else if (cmp > 0)
            current = current->right;
        else
            return current->value; // Key found, return the JSON object
    }
    log_info("Key not found: %s", key);
    return NULL; // Key not found
}

// Insert or update a key-value pair in the database
int db_set(const char *key, json_object *value)
{
    root = insert(root, key, value);
    return 0;
}

// Delete a key-value pair from the database
int db_delete(const char *key)
{
    if (key == NULL)
        return -1;
    root = delete_node(root, key);
    return 0;
}

// Clean up the entire database
void db_cleanup()
{
    free_tree(root);
    root = NULL;
}

// Free memory for a single node
static void free_node(KeyValue *node)
{
    json_object_put(node->value);
    free(node);
}

// Recursively free the entire tree
static void free_tree(KeyValue *node)
{
    if (node)
    {
        free_tree(node->left);
        free_tree(node->right);
        free_node(node);
    }
}

// Get the height of a node
static int height(KeyValue *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

// Get the maximum of two integers
static int max(int a, int b)
{
    return (a > b) ? a : b;
}

// Create a new node with the given key and value
static KeyValue *create_node(const char *key, json_object *value)
{
    KeyValue *node = (KeyValue *)malloc(sizeof(KeyValue));
    strncpy(node->key, key, MAX_KEY_SIZE - 1);
    node->key[MAX_KEY_SIZE - 1] = '\0';
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

// Perform a right rotation
static KeyValue *right_rotate(KeyValue *y)
{
    KeyValue *x = y->left;
    KeyValue *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

// Perform a left rotation
static KeyValue *left_rotate(KeyValue *x)
{
    KeyValue *y = x->right;
    KeyValue *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

// Get the balance factor of a node
static int get_balance(KeyValue *node)
{
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

// Insert a new key-value pair into the AVL tree
static KeyValue *insert(KeyValue *node, const char *key, json_object *value)
{
    // Perform standard BST insertion
    if (node == NULL)
        return create_node(key, value);

    int cmp = strcmp(key, node->key);
    if (cmp < 0)
        node->left = insert(node->left, key, value);
    else if (cmp > 0)
        node->right = insert(node->right, key, value);
    else
    {
        // Key already exists, update the value
        json_object_put(node->value);
        node->value = value;
        return node;
    }

    // Update height of current node
    node->height = max(height(node->left), height(node->right)) + 1;

    // Get the balance factor and rebalance if needed
    int balance = get_balance(node);

    // Left Left Case
    if (balance > 1 && strcmp(key, node->left->key) < 0)
        return right_rotate(node);

    // Right Right Case
    if (balance < -1 && strcmp(key, node->right->key) > 0)
        return left_rotate(node);

    // Left Right Case
    if (balance > 1 && strcmp(key, node->left->key) > 0)
    {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    // Right Left Case
    if (balance < -1 && strcmp(key, node->right->key) < 0)
    {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

// Find the node with the minimum key value
static KeyValue *min_value_node(KeyValue *node)
{
    KeyValue *current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

// Delete a node from the AVL tree
static KeyValue *delete_node(KeyValue *root, const char *key)
{
    // Perform standard BST delete
    if (root == NULL)
        return root;

    int cmp = strcmp(key, root->key);
    if (cmp < 0)
        root->left = delete_node(root->left, key);
    else if (cmp > 0)
        root->right = delete_node(root->right, key);
    else
    {
        // Node to be deleted found

        // Node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL))
        {
            KeyValue *temp = root->left ? root->left : root->right;

            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else
                *root = *temp;

            free_node(temp);
        }
        else
        {
            // Node with two children
            KeyValue *temp = min_value_node(root->right);
            strncpy(root->key, temp->key, MAX_KEY_SIZE - 1);
            root->key[MAX_KEY_SIZE - 1] = '\0';
            root->value = temp->value;
            root->right = delete_node(root->right, temp->key);
        }
    }

    // If the tree had only one node, return
    if (root == NULL)
        return root;

    // Update height of the current node
    root->height = max(height(root->left), height(root->right)) + 1;

    // Get the balance factor and rebalance if needed
    int balance = get_balance(root);

    // Left Left Case
    if (balance > 1 && get_balance(root->left) >= 0)
        return right_rotate(root);

    // Left Right Case
    if (balance > 1 && get_balance(root->left) < 0)
    {
        root->left = left_rotate(root->left);
        return right_rotate(root);
    }

    // Right Right Case
    if (balance < -1 && get_balance(root->right) <= 0)
        return left_rotate(root);

    // Right Left Case
    if (balance < -1 && get_balance(root->right) > 0)
    {
        root->right = right_rotate(root->right);
        return left_rotate(root);
    }

    return root;
}
