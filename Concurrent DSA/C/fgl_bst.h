#ifndef FGL_BST_H
#define FGL_BST_H

#include <pthread.h>
#include <stdbool.h>

// Node structure
struct Node {
    int value;
    struct Node* left;
    struct Node* right;
    pthread_mutex_t mutex;
};

// Tree structure
struct Tree {
    struct Node* root;
    pthread_mutex_t mutex;
};

// Function prototypes
struct Node* createNode(int value);
void destroyNode(struct Node* node);
void lockNode(struct Node* node);
void unlockNode(struct Node* node);
void insert(struct Tree* tree, int value);
bool delete(struct Tree* tree, int value);
bool search(struct Tree* tree, int value);

#endif // FGL_BST_H
