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

#include "set.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize the set
void initializeSet(Set *set) {
    set->count = 0;
    for (int i = 0; i < SET_CAPACITY; i++) {
        set->items[i] = -1;  // Initialize all elements to -1
    }
}

// Insert an item into the set
int insertToSet(Set *set, int item) {
    if (set->count >= SET_CAPACITY) {
        return -1;  // Set is full
    }
    if (isInSet(set, item)) {
        return 0;  // Item already exists
    }
    set->items[set->count++] = item;
    return 1;
}

// Delete an item from the set
int deleteFromSet(Set *set, int item) {
    for (int i = 0; i < set->count; i++) {
        if (set->items[i] == item) {
            set->items[i] = set->items[--set->count];
            return 1; // Item deleted successfully
        }
    }
    return 0; // Item not found
}

// Check if an item is in the set
int isInSet(Set *set, int item) {
    for (int i = 0; i < set->count; i++) {
        if (set->items[i] == item) {
            return 1; // Item found
        }
    }
    return 0; // Item not found
}

// Main function with test cases
int main() {
    Set mySet;
    initializeSet(&mySet);
    insertToSet(&mySet, 10);
    insertToSet(&mySet, 20);
    insertToSet(&mySet, 10);  // Duplicate entry

    printf("Is 10 in set? %d\n", isInSet(&mySet, 10));
    printf("Is 30 in set? %d\n", isInSet(&mySet, 30));

    deleteFromSet(&mySet, 20);
    printf("Is 20 in set after deletion? %d\n", isInSet(&mySet, 20));

    return 0;
}



// ### Explanation

// - **Set Structure**: Contains an array `items` for storing elements and an integer `count` to track the number of elements.
// - **initializeSet Function**: Initializes the set, setting all elements to -1.
// - **insertToSet Function**: Inserts a new item into the set if it's not already present and if there's space.
// - **deleteFromSet Function**: Removes an item from the set if it's present.
// - **isInSet Function**: Checks if an item is present in the set.
// - **main Function**: Demonstrates the usage of the set API with test cases.

// ### Time and Space Complexity Analysis

// - **Time Complexity**:
//   - `insertToSet`, `deleteFromSet`, `isInSet`: O(n) in the worst case - These operations might need to traverse the entire set to find or insert an item.
// - **Space Complexity**: 
//   - O(n) - The space complexity is linear in the capacity of the set.
