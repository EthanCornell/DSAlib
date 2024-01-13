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

#include "deque.h"
#include <stdio.h>
#include <stdlib.h>

Deque* createDeque(int capacity) {
    Deque *deque = (Deque *)malloc(sizeof(Deque));
    deque->capacity = capacity;
    deque->front = -1;
    deque->rear = 0;
    deque->size = 0;
    deque->items = (int *)malloc(deque->capacity * sizeof(int));
    return deque;
}

int isFull(Deque *deque) {
    return deque->size == deque->capacity;
}

int isEmpty(Deque *deque) {
    return deque->size == 0;
}

void insertFront(Deque *deque, int item) {
    if (isFull(deque)) {
        return;
    }
    deque->front = (deque->front - 1 + deque->capacity) % deque->capacity;
    deque->items[deque->front] = item;
    if (isEmpty(deque)) {
        deque->rear = deque->front;
    }
    deque->size++;
}

void insertRear(Deque *deque, int item) {
    if (isFull(deque)) {
        return;
    }
    deque->items[deque->rear] = item;
    deque->rear = (deque->rear + 1) % deque->capacity;
    if (isEmpty(deque)) {
        deque->front = deque->rear;
    }
    deque->size++;
}

int deleteFront(Deque *deque) {
    if (isEmpty(deque)) {
        return -1; // Indicate empty deque
    }
    int item = deque->items[deque->front];
    deque->front = (deque->front + 1) % deque->capacity;
    deque->size--;
    if (isEmpty(deque)) {
        deque->rear = deque->front = -1; // Reset indices if deque is empty
    }
    return item;
}

int deleteRear(Deque *deque) {
    if (isEmpty(deque)) {
        return -1; // Indicate empty deque
    }
    deque->rear = (deque->rear - 1 + deque->capacity) % deque->capacity;
    int item = deque->items[deque->rear];
    deque->size--;
    if (isEmpty(deque)) {
        deque->front = deque->rear = -1; // Reset indices if deque is empty
    }
    return item;
}

int getFront(Deque *deque) {
    if (isEmpty(deque)) {
        return -1; // Indicate empty deque
    }
    return deque->items[deque->front];
}

int getRear(Deque *deque) {
    if (isEmpty(deque)) {
        return -1; // Indicate empty deque
    }
    return deque->items[(deque->rear - 1 + deque->capacity) % deque->capacity];
}

void deleteDeque(Deque *deque) {
    if (deque) {
        if (deque->items) {
            free(deque->items);
        }
        free(deque);
    }
}

// ### Explanation

// - **Deque Structure**: Contains a dynamic array, indices for the front and rear elements, the capacity of the deque, and its current size.
// - **createDeque Function**: Allocates memory for the deque and initializes its properties.
// - **isFull & isEmpty Functions**: Check if the deque is full or empty, respectively.
// - **insertFront & insertRear Functions**: Add an item at the front or rear of the deque. They manage the circular behavior by wrapping the indices.
// - **deleteFront & deleteRear Functions**: Remove an item from the front or rear of the deque, returning the removed item. They handle circular indexing and reset the indices if the deque becomes empty.
// - **getFront & getRear Functions**: Return the item at the front or rear of the deque without removing it.
// - **deleteDeque Function**: Frees the memory allocated for the deque.
// - **main Function**: Demonstrates the usage of the deque API with test cases.

// ### Time and Space Complexity Analysis

// - **Time Complexity**:
//   - All operations (`insertFront`, `insertRear`, `deleteFront`, `deleteRear`, `getFront`, `getRear`, `isFull`, `isEmpty`): O(1) - These operations are constant time as they only involve updating or reading from an index in an array.
// - **Space Complexity**: 
//   - O(n) - The main space usage comes from the dynamic array that stores the deque elements, where `n` is the capacity of the deque.



// Main function with test cases
int main() {
    Deque *deque = createDeque(5);

    insertFront(deque, 10);
    insertRear(deque, 20);
    printf("Front item: %d\n", getFront(deque));
    printf("Rear item: %d\n", getRear(deque));

    printf("Deleted front item: %d\n", deleteFront(deque));
    printf("Deleted rear item: %d\n", deleteRear(deque));

    deleteDeque(deque);
    return 0;
}
