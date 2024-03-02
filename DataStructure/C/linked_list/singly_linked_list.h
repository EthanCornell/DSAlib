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

#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

// Define the node structure
typedef struct Node {
    int data;           // Data of the node
    struct Node *next;  // Pointer to the next node
} Node;

// Function to create a new node with given data
Node* createNode(int data);

// Function to insert a node at the beginning of the list
void insertAtBeginning(Node **head, int data);

// Function to delete the first node of the list
void deleteFirstNode(Node **head);

// Insert a node at the end of the list
void insertAtEnd(Node **head, int data);

// Search for a node with given data
Node* search(Node *head, int data);

// Delete a node with given data
void deleteNode(Node **head, int data) ;

// Function to display the list
void displayList(Node *head);

// Function to free the entire list
void freeList(Node *head);

#endif // SINGLY_LINKED_LIST_H
