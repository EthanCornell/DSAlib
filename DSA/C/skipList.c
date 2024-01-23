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
//reference: https://en.wikipedia.org/wiki/Skip_list

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_LEVEL 6

// Node structure
typedef struct Node {
    int value;
    struct Node *forward[MAX_LEVEL + 1];
} Node;

// SkipList structure
typedef struct SkipList {
    int level;
    Node *header;
} SkipList;

// Create new node
Node* createNode(int level, int value) {
    Node *n = (Node*)malloc(sizeof(Node));
    n->value = value;
    for (int i = 0; i <= level; i++) {
        n->forward[i] = NULL;
    }
    return n;
}

// Initialize SkipList
SkipList* createSkipList() {
    SkipList *sl = (SkipList*)malloc(sizeof(SkipList));
    sl->level = 0;
    sl->header = createNode(MAX_LEVEL, INT_MIN);
    return sl;
}

// Random level generator
int randomLevel() {
    int level = 0;
    while (rand() < RAND_MAX / 2 && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

// Insert value into SkipList
void insert(SkipList *sl, int value) {
    Node *current = sl->header;
    Node *update[MAX_LEVEL + 1];
    for (int i = sl->level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->value < value) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    current = current->forward[0];
    if (current == NULL || current->value != value) {
        int rl = randomLevel();
        if (rl > sl->level) {
            for (int i = sl->level + 1; i <= rl; i++) {
                update[i] = sl->header;
            }
            sl->level = rl;
        }
        Node *newNode = createNode(rl, value);
        for (int i = 0; i <= rl; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }
}

// Search value in SkipList
int search(SkipList *sl, int value) {
    Node *current = sl->header;
    for (int i = sl->level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->value < value) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];
    if (current && current->value == value) {
        return 1; // Found
    }
    return 0; // Not found
}

// Delete value from SkipList
void delete(SkipList *sl, int value) {
    Node *current = sl->header;
    Node *update[MAX_LEVEL + 1];
    for (int i = sl->level; i >= 0; i--) {
        while (current->forward[i] != NULL && current->forward[i]->value < value) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    current = current->forward[0];
    if (current != NULL && current->value == value) {
        for (int i = 0; i <= sl->level; i++) {
            if (update[i]->forward[i] != current) break;
            update[i]->forward[i] = current->forward[i];
        }
        free(current);
        while (sl->level > 0 && sl->header->forward[sl->level] == NULL) {
            sl->level--;
        }
    }
}



// Node Structure:
// Value
// Array of pointers (one for each level)

// SkipList Structure:
// Head (pointer to first Node, with maximum level)
// Maximum level
// Current level (highest level with at least one node)

// Search(value):
// Start from the head at the highest level.
// Move forward while the next value is less than the search value.
// If next value is greater, move down one level.
// Repeat until found or reach the lowest level without finding it.

// Insert(value):
// Find the position where the value should be inserted (like in Search).
// Randomly determine the level of the new node.
// Update pointers at all levels up to the node's level.

// Delete(value):
// Find the node (like in Search).
// Update pointers to bypass the node at all levels.
// Free the node.

// RandomLevel():
// Randomly decide a new level for insertion (usually with a geometric distribution).

// Complexity Analysis
// Time Complexity:
// Search, Insert, Delete: Average and best-case O(log N), worst-case O(N).
// Space Complexity:
// O(N log N), due to additional pointers at each level.


// Main function with test cases
int main() {
    SkipList *sl = createSkipList();

    insert(sl, 3);
    insert(sl, 6);
    insert(sl, 7);
    insert(sl, 9);
    insert(sl, 12);
    insert(sl, 19);
    insert(sl, 17);

    printf("Search 9: %s\n", search(sl, 9) ? "Found" : "Not Found");
    printf("Search 15: %s\n", search(sl, 15) ? "Found" : "Not Found");

    delete(sl, 7);
    printf("Search 7 after deletion: %s\n", search(sl, 7) ? "Found" : "Not Found");

    // Free memory and clean up (not shown here)
    
    return 0;
}
