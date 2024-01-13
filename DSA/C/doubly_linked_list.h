#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

// Define the node structure
typedef struct Node {
    int data;               // Data of the node
    struct Node *next;      // Pointer to the next node
    struct Node *prev;      // Pointer to the previous node
} Node;

// Function to create a new node with given data
Node* createNode(int data);

// Function to insert a node at the beginning of the list
void insertAtBeginning(Node **head, int data);

// Function to insert a node at the end of the list
void insertAtEnd(Node **head, int data);

// Function to delete a node from the list
void deleteNode(Node **head, Node *nodeToDelete);

// Function to display the list from beginning to end
void displayListForward(Node *head);

// Function to display the list from end to beginning
void displayListBackward(Node *head);

#endif // DOUBLY_LINKED_LIST_H
