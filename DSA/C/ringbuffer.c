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
#include <stdbool.h> // For using bool type

/*
Pseudocode
struct RingBuffer {
    int *buffer; // Array to store the data
    int head; // Points to the next insertion point
    int tail; // Points to the next removal point
    int size; // The total size of the buffer
    int count; // Number of items in the buffer
};

function initialize(size):
    Create a RingBuffer with specified size
    Allocate memory for the buffer array of given size
    Set head, tail, and count to 0

function insert(RingBuffer, value):
    if count is equal to size-1:
        return error (Buffer is full)
    Insert value at head
    head = (head + 1) % size
    Increment count

function remove(RingBuffer):
    if count is 0:
        return error (Buffer is empty)
    value = buffer[tail]
    tail = (tail + 1) % size
    Decrement count
    return value

*/

typedef struct {
    void **buffer; // Pointer to array of void pointers
    int head, tail;
    int size;
    int count;
} RingBuffer;

RingBuffer* initialize(int size) {
    RingBuffer *rb = (RingBuffer*)malloc(sizeof(RingBuffer));
    rb->buffer = (void**)malloc(size * sizeof(void*)); // Allocate memory for void pointers
    rb->size = size;
    rb->head = rb->tail = rb->count = 0;
    return rb;
}

void insert(RingBuffer *rb, int value) {
    if (rb->count == rb->size) {
        printf("Buffer is full\n");
        return;
    }
    int* newValue = malloc(sizeof(int)); // Dynamically allocate memory for the integer
    *newValue = value; // Store the value in allocated memory
    rb->buffer[rb->head] = newValue; // Insert the pointer to the allocated memory
    rb->head = (rb->head + 1) % rb->size;
    rb->count++;
}


void* ringBufferRemove(RingBuffer *rb) {
    if (rb->count == 0) {
        printf("Buffer is empty\n");
        return NULL;
    }
    void* value = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count--;
    free(value); // Free the dynamically allocated memory
    return value;
}


// Function to check if the ring buffer is empty
bool is_empty(RingBuffer *rb) {
    return rb->count == 0;
}

// Function to check if the ring buffer is full
bool is_full(RingBuffer *rb) {
    return rb->count == rb->size;
}

// Function to flush (clear) the ring buffer
void flush(RingBuffer *rb) {
    while (!is_empty(rb)) {
        void* removedElement = ringBufferRemove(rb); // Remove elements and free memory
        // No need to explicitly call free here since it's done in ringBufferRemove
    }
    rb->head = rb->tail = rb->count = 0; // Reset the buffer indices and count
}

bool search(RingBuffer *rb, int query) {
    if (rb->count == 0) {
        return false;
    }
    int i = rb->tail;
    do {
        int* value = (int*)rb->buffer[i]; // Cast the void pointer to int pointer
        if (*value == query) { // Correctly compare the dereferenced value
            return true; // Element found
        }
        i = (i + 1) % rb->size;
    } while (i != rb->head);
    return false; // Element not found
}


void printRingBuffer(RingBuffer *rb) {
    printf("Ring Buffer: ");
    if (rb->count == 0) {
        printf("Empty\n");
        return;
    }
    int i = rb->tail;
    do {
        int* value = (int*)rb->buffer[i]; // Correctly cast the void pointer to an int pointer
        printf("%d ", *value); // Dereference the int pointer to print the value
        i = (i + 1) % rb->size;
    } while (i != rb->head);
    printf("\n");
}






// initialize(size): Allocates memory for a RingBuffer and its internal buffer array. Sets the head, tail, and count to 0.
// insert(rb, value): Checks if the buffer is full. If not, inserts the value at the head position, updates the head position, and increments the count.
// remove(rb): Checks if the buffer is empty. If not, retrieves the value at the tail position, updates the tail position, and decrements the count.

// Time Complexity:
// Insertion: O(1)
// Removal: O(1)
// Space Complexity: O(n), where n is the size of the buffer.

int main() {
    RingBuffer *rb = initialize(5); // Create a ring buffer of size 5

    // Test is_empty on a new buffer
    printf("Buffer is %s\n", is_empty(rb) ? "empty" : "not empty");

    // Insert elements and print
    insert(rb, 1); printRingBuffer(rb);
    insert(rb, 2); printRingBuffer(rb);
    insert(rb, 3); printRingBuffer(rb);
    ringBufferRemove(rb); printRingBuffer(rb); // Remove one element
    insert(rb, 4); printRingBuffer(rb);
    insert(rb, 5); printRingBuffer(rb);
    
    // Test is_full after filling the buffer
    printf("Buffer is %s\n", is_full(rb) ? "full" : "not full");

    // Insert into a full buffer to see the message
    insert(rb, 6); printRingBuffer(rb); // Buffer is full

    // Search for elements
    int searchFor = 3;
    if (search(rb, searchFor)) {
        printf("Element %d found in the ring buffer.\n", searchFor);
    } else {
        printf("Element %d not found in the ring buffer.\n", searchFor);
    }

    // Search for an element that is not in the buffer
    searchFor = 6;
    if (search(rb, searchFor)) {
        printf("Element %d found in the ring buffer.\n", searchFor);
    } else {
        printf("Element %d not found in the ring buffer.\n", searchFor);
    }

    // Test flush function
    flush(rb);
    printf("After flushing, buffer is %s\n", is_empty(rb) ? "empty" : "not empty");
    printRingBuffer(rb); // Should print "Ring Buffer: Empty"

    // Insert after flushing to verify buffer can still be used
    insert(rb, 7); printRingBuffer(rb);

    // Remove all elements and print each time
    while (rb->count > 0) {
        ringBufferRemove(rb);
        printRingBuffer(rb);
    }

    // Test is_empty after removing all elements
    printf("After removal, buffer is %s\n", is_empty(rb) ? "empty" : "not empty");

    // Cleanup
    free(rb->buffer);
    free(rb);
    return 0;
}

