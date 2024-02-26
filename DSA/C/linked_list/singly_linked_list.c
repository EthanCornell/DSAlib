/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */

#include "singly_linked_list.h"
#include <stdio.h>
#include <stdlib.h>

// Create a new node with given data
Node* createNode(int data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Insert a node at the beginning of the list
void insertAtBeginning(Node **head, int data) {
    Node *newNode = createNode(data);
    newNode->next = *head;
    *head = newNode;
}

// Delete the first node of the list
void deleteFirstNode(Node **head) {
    if (*head == NULL) return;
    Node *temp = *head;
    *head = (*head)->next;
    free(temp);
}

// Insert a node at the end of the list
void insertAtEnd(Node **head, int data) {
    Node *newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    Node *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
}

// Search for a node with given data
Node* search(Node *head, int data) {
    Node *current = head;
    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL; // Data not found
}

// Delete a node with given data
void deleteNode(Node **head, int data) {
    if (*head == NULL) return;

    Node *temp = *head;
    Node *prev = NULL;
    
    // If head node holds the data to be deleted
    if (temp != NULL && temp->data == data) {
        *head = temp->next;
        free(temp);
        return;
    }

    // Search for the node to be deleted
    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    // If data was not present in the list
    if (temp == NULL) return;

    // Unlink the node and free memory
    prev->next = temp->next;
    free(temp);
}



// Display the list
void displayList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

// Free the entire list
void freeList(Node *head) {
    Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Node Structure: Each node contains an integer `
// dataand a pointernext` to the next node.

// createNode Function: Allocates memory for a new node, initializes its data, and sets its next pointer to NULL.
// insertAtBeginning Function: Creates a new node and inserts it at the beginning of the list. It adjusts the head pointer to point to the new node.
// deleteFirstNode Function: Removes the first node from the list. It updates the head pointer to the second node and frees the memory of the removed node.
// displayList Function: Iterates over the list and prints the data of each node.
// freeList Function: Frees the memory of all nodes in the list.
// main Function: Demonstrates the usage of the linked list API with test cases. It creates a list, inserts nodes, displays the list, deletes a node, displays the list again, and finally frees the list.
// Time and Space Complexity Analysis
// Time Complexity:

// insertAtBeginning: O(1) - Insertion at the beginning is a constant-time operation.
// deleteFirstNode: O(1) - Deleting the first node is also a constant-time operation.
// displayList and freeList: O(n) - Both functions involve traversing the entire list, where n is the number of nodes in the list.
// Space Complexity:

// O(n) - The space complexity is linear in the number of nodes in the list.
// insertAtEnd Function: Inserts a new node at the end of the list. It traverses the list to find the last node and then attaches the new node to it.
// search Function: Searches the list for a node with a given data value. It returns the node if found, or NULL if the data is not present in the list.
// deleteNode Function: Deletes a node with a given data value. It handles three cases: when the list is empty, when the node to be deleted is the head, and when the node is in the middle or at the end of the list.
// Time Complexity
// insertAtEnd: O(n) - It needs to traverse the entire list to find the last node.
// search: O(n) - In the worst case, it might have to traverse the entire list.
// deleteNode: O(n) - It may need to traverse the list to find the node to be deleted.
// Space Complexity
// O(1) for all new functions - These operations only require a constant amount of additional space.

// Main function with test cases
int main() {
    Node *head = NULL;

    insertAtBeginning(&head, 10);
    insertAtBeginning(&head, 20);
    insertAtBeginning(&head, 30);
    displayList(head);

    deleteFirstNode(&head);
    displayList(head);

    freeList(head);
    return 0;
}
