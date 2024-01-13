#include "doubly_linked_list.h"
#include <stdio.h>
#include <stdlib.h>

// Create a new node with the given data
Node* createNode(int data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = newNode->prev = NULL;
    return newNode;
}

// Insert a node at the beginning of the doubly linked list
void insertAtBeginning(Node **head, int data) {
    Node *newNode = createNode(data);
    newNode->next = *head;
    if (*head != NULL) {
        (*head)->prev = newNode;
    }
    *head = newNode;
}

// Insert a node at the end of the doubly linked list
void insertAtEnd(Node **head, int data) {
    Node *newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    Node *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->prev = temp;
}

// Delete a specified node from the doubly linked list
void deleteNode(Node **head, Node *nodeToDelete) {
    if (*head == NULL || nodeToDelete == NULL) return;
    if (*head == nodeToDelete) {
        *head = nodeToDelete->next;
    }
    if (nodeToDelete->next != NULL) {
        nodeToDelete->next->prev = nodeToDelete->prev;
    }
    if (nodeToDelete->prev != NULL) {
        nodeToDelete->prev->next = nodeToDelete->next;
    }
    free(nodeToDelete);
}

// Display the doubly linked list in the forward direction
void displayListForward(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

// Display the doubly linked list in the backward direction
void displayListBackward(Node *head) {
    Node *tail = head;
    // Find the last node
    while (tail != NULL && tail->next != NULL) {
        tail = tail->next;
    }
    // Traverse backwards
    while (tail != NULL) {
        printf("%d -> ", tail->data);
        tail = tail->prev;
    }
    printf("NULL\n");
}

// ### Explanation

// - **Node Structure**: Contains an integer `data`, a pointer `next` to the next node, and a pointer `prev` to the previous node.
// - **createNode Function**: Allocates memory for a new node and initializes its data and pointers.
// - **insertAtBeginning Function**: Inserts a new node at the beginning of the list. It adjusts the `head` pointer and the `prev` pointer of the old head node.
// - **insertAtEnd Function**: Inserts a new node at the end of the list by traversing to the last node and adjusting the pointers.
// - **deleteNode Function**: Deletes a given node from the list. It handles four cases: when the list is empty, when the node to be deleted is the head, when the node has a next node, and when the node has a previous node.
// - **displayListForward Function**: Prints the list from the beginning to the end.
// - **displayListBackward Function**: Prints the list from the end to the beginning by first finding the tail of the list.
// - **main Function**: Demonstrates the usage of the doubly linked list API with test cases. It creates a list, inserts nodes at the beginning and end, displays the list in both directions, deletes a node, and finally frees the list.

// ### Time and Space Complexity Analysis

// - **Time Complexity**:
//   - `insertAtBeginning`, `deleteNode`: O(1) - These operations are constant time.
//   - `insertAtEnd`, `displayListForward`, `displayListBackward`: O(n) - These operations may involve traversing the entire list.
// - **Space Complexity**: 
//   - O(n)


// Main function with test cases
int main() {
    Node *head = NULL;

    insertAtBeginning(&head, 10);
    insertAtBeginning(&head, 20);
    insertAtEnd(&head, 30);
    printf("List in forward direction:\n");
    displayListForward(head);
    printf("List in backward direction:\n");
    displayListBackward(head);

    deleteNode(&head, head->next); // Delete the second node
    printf("List after deletion:\n");
    displayListForward(head);

    // Free the remaining list
    while (head != NULL) {
        deleteNode(&head, head);
    }

    return 0;
}
