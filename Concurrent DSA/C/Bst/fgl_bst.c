#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "fgl_bst.h"

// struct Node {
//     int value;
//     struct Node* left;
//     struct Node* right;
//     pthread_mutex_t mutex;
// };

// struct Tree {
//     struct Node* root;
//     pthread_mutex_t mutex;
// };

struct Node* createNode(int value) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    if (node == NULL) {
        return NULL;
    }
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    pthread_mutex_init(&node->mutex, NULL);
    return node;
}

void destroyNode(struct Node* node) {
    if (node != NULL) {
        pthread_mutex_destroy(&node->mutex);
        free(node);
    }
}

void lockNode(struct Node* node) {
    pthread_mutex_lock(&node->mutex);
}

void unlockNode(struct Node* node) {
    pthread_mutex_unlock(&node->mutex);
}

void insert(struct Tree* tree, int value) {
    pthread_mutex_lock(&tree->mutex);
    if (tree->root == NULL) {
        tree->root = createNode(value);
        pthread_mutex_unlock(&tree->mutex);
        return;
    }
    pthread_mutex_unlock(&tree->mutex);

    struct Node* parent = NULL;
    struct Node* current = tree->root;
    while (current != NULL) {
        lockNode(current);
        if (parent != NULL) {
            unlockNode(parent);
        }
        parent = current;

        if (value < current->value) {
            current = current->left;
        } else if (value > current->value) {
            current = current->right;
        } else {
            // Value already exists, no insertion needed
            unlockNode(parent);
            return;
        }
    }

    struct Node* newNode = createNode(value);
    if (value < parent->value) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }
    unlockNode(parent);
}

bool delete(struct Tree* tree, int value) {
    struct Node* parent = NULL;
    struct Node* node = tree->root;
    struct Node** childLink = &(tree->root);

    // Find the node to delete and its parent.
    while (node != NULL) {
        lockNode(node);
        if (value < node->value) {
            if (parent != NULL) {
                unlockNode(parent);
            }
            parent = node;
            childLink = &(node->left);
            node = node->left;
        } else if (value > node->value) {
            if (parent != NULL) {
                unlockNode(parent);
            }
            parent = node;
            childLink = &(node->right);
            node = node->right;
        } else {
            break; // Found the node to delete.
        }
    }

    if (node == NULL) {
        // Value not found.
        if (parent != NULL) {
            unlockNode(parent);
        }
        return false;
    }

    // Case 1: Deleting a node with no children or one child.
    if (node->left == NULL || node->right == NULL) {
        struct Node* child = (node->left != NULL) ? node->left : node->right;

        // Replace the node with its child.
        *childLink = child;

        unlockNode(node);
        if (parent != NULL) {
            unlockNode(parent);
        }
        destroyNode(node);
    } else {
        // Case 2: Deleting a node with two children.
        // Find the in-order successor (smallest in the right subtree).
        struct Node* successorParent = node;
        struct Node* successor = node->right;
        lockNode(successor);

        while (successor->left != NULL) {
            if (successorParent != node) {
                unlockNode(successorParent);
            }
            successorParent = successor;
            successor = successor->left;
            lockNode(successor);
        }

        // Copy the successor's value to the node and delete the successor.
        node->value = successor->value;
        if (successorParent->left == successor) {
            successorParent->left = successor->right;
        } else {
            successorParent->right = successor->right;
        }

        unlockNode(node);
        unlockNode(successor);
        if (successorParent != node) {
            unlockNode(successorParent);
        }
        destroyNode(successor);
    }

    return true;
}


bool search(struct Tree* tree, int value) {
    struct Node* current = tree->root;
    while (current != NULL) {
        lockNode(current);  // Lock the current node
        if (value < current->value) {
            struct Node* temp = current->left;
            unlockNode(current);  // Unlock the current node before moving to the left child
            current = temp;
        } else if (value > current->value) {
            struct Node* temp = current->right;
            unlockNode(current);  // Unlock the current node before moving to the right child
            current = temp;
        } else {
            unlockNode(current);  // Found the value, unlock the current node before returning
            return true;  // Value found
        }
    }
    // The value was not found in the tree.
    return false;
}

