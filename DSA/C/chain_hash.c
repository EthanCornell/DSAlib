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

#include <stdio.h>
#include <stdlib.h>

//Separate Chaining
typedef struct ListNode {
    int key;
    struct ListNode* next;
} ListNode;

typedef struct HashTable {
    ListNode** lists;
    int size;
} HashTable;

HashTable* createHashTable(int size) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->size = size;
    table->lists = (ListNode**)malloc(sizeof(ListNode*) * size);
    for (int i = 0; i < size; i++) {
        table->lists[i] = NULL;
    }
    return table;
}

int hashFunction(int key, int size) {
    return key % size;
}



// Step-by-Step Explanation:
// Compute the Index: The hashFunction is called with the given key and the size of the table to compute the index where the new node should be inserted. 
// This index corresponds to the linked list in the lists array where the node will be placed.
// Allocate Memory for the New Node: Memory is dynamically allocated for a new ListNode structure. This node will store the key passed to the function. 
// If memory allocation fails, an error message is printed, and the function returns immediately.
// Insertion at the List's Head: The new node is inserted at the beginning of the linked list located at the computed index. 
// This is achieved by setting the newNode->next pointer to the current head of the list (table->lists[index]) and then updating table->lists[index] to point to the new node. 
// This way, the new node becomes the new head of the list.
void chainedHashInsert(HashTable* table, int key) {
    // Compute the index using the hash function
    int index = hashFunction(key, table->size);

    // Allocate memory for the new node
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (newNode == NULL) {
        printf("Unable to allocate memory for new node\n");
        return;
    }
    newNode->key = key;

    // Insert the new node at the beginning of the list at the computed index
    newNode->next = table->lists[index];
    table->lists[index] = newNode;
}

// Step-by-Step Explanation:
// Compute the Index: The function starts by computing the index for the given key using the hashFunction. 
// This index points to the specific linked list within the hash table where the key, if present, would be located.
// Traverse the Linked List: It then traverses the linked list starting from the head node at the computed index. 
// The traversal continues until either the key is found or the end of the list is reached.
// Key Comparison: During the traversal, each node's key is compared with the search key. 
// If a match is found (current->key == key), the function immediately returns a pointer to the node containing the matching key.
// Return Node or NULL: If the function traverses the entire list without finding the key, it concludes that the key is not present in the hash table and returns NULL.
// This search operation has a time complexity that varies depending on the distribution of keys and the load factor of the hash table. 
// In the best case, if the key is at the beginning of the list or if there are no collisions, the time complexity is 
// O(1). In the worst case, if many keys are hashed to the same index, the time complexity could degrade to O(n) for a search operation, where 
// n is the number of keys in the list at that index.

ListNode* chainedHashSearch(HashTable* table, int key) {
    // Compute the index for the given key using the hash function
    int index = hashFunction(key, table->size);

    // Start from the head of the list at the computed index
    ListNode* current = table->lists[index];

    // Traverse the list to find the node with the given key
    while (current != NULL) {
        if (current->key == key) {
            return current; // Key found, return the node
        }
        current = current->next; // Move to the next node in the list
    }

    // Key not found, return NULL
    return NULL;
}

// Step-by-Step Explanation:
// Compute the Index: First, compute the index where the key, if present, would be located in the hash table by using the hashFunction.
// Traversal: Start traversing the linked list at the computed index with two pointers: current for the current node and prev for the node preceding current. Initially, prev is NULL because current starts at the head of the list.
// Find the Node: As you traverse the list, compare each node's key with the key to be deleted. If a match is found, proceed to remove the node from the list.
// Remove the Node:
// If the node to be deleted is the head of the list (prev is NULL), update the head of the list to be the next node in the list.
// If the node to be deleted is not the head, adjust the next pointer of the prev node to bypass the current node, effectively removing current from the list.
// Free Memory: Free the memory allocated for the node that is being deleted to avoid memory leaks.
// Exit: After deleting the node, exit the function. If no node with the key is found (current reaches NULL), the function simply returns without doing anything.
// This chainedHashDelete function handles deletion efficiently, with the time complexity for the deletion operation generally being 
// O(1) in the best case (if the node to be deleted is at the beginning of the list) and 
// O(n) in the worst case, where n is the number of elements in the list at the hash index.

void chainedHashDelete(HashTable* table, int key) {
    // Compute the index for the given key using the hash function
    int index = hashFunction(key, table->size);

    // Start from the head of the list at the computed index
    ListNode* current = table->lists[index];
    ListNode* prev = NULL;

    // Traverse the list to find the node with the given key
    while (current != NULL) {
        if (current->key == key) {
            // Key found, now remove the node from the list
            if (prev == NULL) {
                // The node to delete is the head of the list
                table->lists[index] = current->next;
            } else {
                // The node to delete is in the middle or end of the list
                prev->next = current->next;
            }
            free(current); // Free the memory allocated for the node
            return; // Key has been deleted, exit the function
        }
        // Move to the next node in the list
        prev = current;
        current = current->next;
    }

    // If the function reaches here, the key was not found in the list
}



// Time Complexity:
// Insert: O(1) on average if the load factor is kept reasonable.
// Search/Delete: O(N/M) on average, where N is the number of keys and M is the table size (depends on the load factor and hash function quality).
// Space Complexity: O(N + M), where N is the number of elements and M is the size of the table.

// Optimized Method
// Given the requirement to handle a large number of items (>10^9), separate chaining is more suitable because:
// It handles high load factors better than open addressing, which can suffer from clustering issues.
// It's easier to resize dynamically as it can accommodate more elements without significant performance degradation.



int main() {
    HashTable* table = createHashTable(10);
    // Add test cases for insert, search, and delete
    // Example: chainedHashInsert(table, 5);
    // Example: ListNode* node = chainedHashSearch(table, 5);
    // Example: chainedHashDelete(table, 5);
    chainedHashInsert(table, 5);
    ListNode* node = chainedHashSearch(table, 5);
    chainedHashDelete(table, 5);
    return 0;
}
