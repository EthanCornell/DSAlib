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
#include <assert.h>
typedef struct {
    int data;
    int refCount;
} CoWData;
typedef struct {
    CoWData **buffer; // Pointer to array of pointers to CoWData
    int head, tail;
    int size;
    int count;
} RingBuffer;

RingBuffer* initialize(size_t size);
bool is_empty(RingBuffer *rb);
void insert(RingBuffer* rb, int data);
void printRingBuffer(RingBuffer* rb);
size_t calculateMemoryUsage(RingBuffer* rb);
void modifyData(RingBuffer* rb, int index, int newValue);
void* ringBufferRemove(RingBuffer* rb);
bool is_full(RingBuffer *rb);
bool search(RingBuffer *rb, int query);
void flush(RingBuffer* rb);
size_t calculateMemoryUsage(RingBuffer *rb);
void resizeRingBuffer(RingBuffer* rb, size_t newSize);

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

RingBuffer* initialize(size_t size) {
    RingBuffer *rb = (RingBuffer*)malloc(sizeof(RingBuffer));
    rb->buffer = (CoWData**)malloc(size * sizeof(CoWData*));
    rb->size = size;
    rb->head = rb->tail = rb->count = 0;
    return rb;
}

void insert(RingBuffer *rb, int value) {
    if (rb->count == rb->size) {
        printf("Buffer is full\n");
        return;
    }
    CoWData* newData = malloc(sizeof(CoWData));
    newData->data = value;
    newData->refCount = 1; // Initial reference count
    rb->buffer[rb->head] = newData;
    rb->head = (rb->head + 1) % rb->size;
    rb->count++;
}

void* ringBufferRemove(RingBuffer *rb) {
    if (rb->count == 0) {
        printf("Buffer is empty\n");
        return NULL;
    }
    CoWData* value = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count--;
    
    // Decrement reference count and free if no longer needed
    if (--value->refCount == 0) {
        free(value);
    }
    
    return NULL; // Modified to not return the value directly
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



// New function to calculate current memory usage
size_t calculateMemoryUsage(RingBuffer *rb) {
    size_t memoryUsage = 0;
    for (int i = rb->tail; rb->count > 0 && i != rb->head; i = (i + 1) % rb->size) {
        memoryUsage += sizeof(int); // Assuming each element is an integer
    }
    return memoryUsage;
}

// New function to simulate modifying data with COW
void modifyData(RingBuffer *rb, int index, int newValue) {
    if (index < 0 || index >= rb->count) {
        printf("Index out of bounds\n");
        return;
    }
    int actualIndex = (rb->tail + index) % rb->size;
    CoWData* data = rb->buffer[actualIndex];
    
    // Check if data can be safely modified or needs to be copied
    if (data->refCount > 1) {
        // Copy needed, decrement original's refCount and create a new copy
        data->refCount--;
        CoWData* newData = malloc(sizeof(CoWData));
        newData->data = newValue;
        newData->refCount = 1;
        rb->buffer[actualIndex] = newData;
    } else {
        // Safe to modify directly
        data->data = newValue;
    }
}


void resizeRingBuffer(RingBuffer* rb, size_t newSize) {
    // Allocate new array of pointers to CoWData with newSize
    CoWData** newArray = (CoWData**)malloc(newSize * sizeof(CoWData*));

    // Check for allocation failure
    if (!newArray) {
        perror("Failed to resize ring buffer");
        return;
    }

    // Initialize the new array to avoid uninitialized reads
    for (size_t i = 0; i < newSize; ++i) {
        newArray[i] = NULL;
    }

    // Copy elements from old array to new array
    int oldIndex = rb->tail;
    for (int i = 0; i < rb->count; ++i) {
        int newIndex = i % newSize;
        newArray[newIndex] = rb->buffer[oldIndex];
        oldIndex = (oldIndex + 1) % rb->size;
    }

    // Free old array - No need to free individual CoWData elements here, as we're just moving pointers
    free(rb->buffer);

    // Update ring buffer structure
    rb->buffer = newArray;
    rb->size = newSize;
    rb->head = rb->count % newSize; // New head position
    rb->tail = 0; // Reset tail if buffer is not full, adjust if necessary
}



int main() {
    RingBuffer *rb = initialize(5); // Create a ring buffer of size 5
    //test functions
    size_t memoryUsage;
    // Test is_empty on a new buffer
    printf("Buffer is %s\n", is_empty(rb) ? "empty" : "not empty");

    // Insert elements and print
    insert(rb, 1); printRingBuffer(rb);
    
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after inserting 1: %zu bytes\n", memoryUsage);
    
    insert(rb, 2); printRingBuffer(rb);
    modifyData(rb, 1, 25);
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after inserting 2: %zu bytes\n", memoryUsage);
    
    insert(rb, 3); printRingBuffer(rb);
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after inserting 3: %zu bytes\n", memoryUsage);
    
    ringBufferRemove(rb); printRingBuffer(rb); // Remove one element
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after removing one element : %zu bytes\n", memoryUsage);
    
    insert(rb, 4); printRingBuffer(rb);
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after inserting 4: %zu bytes\n", memoryUsage);
    
    insert(rb, 5); printRingBuffer(rb);
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after inserting 5: %zu bytes\n", memoryUsage);
    
    // Test is_full after filling the buffer
    printf("Buffer is %s\n", is_full(rb) ? "full" : "not full");

    // Insert into a full buffer to see the message
    insert(rb, 6); printRingBuffer(rb); // Buffer is full
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after inserting 6: %zu bytes\n", memoryUsage);

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
            memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after flushing: %zu bytes\n", memoryUsage);

    // Insert after flushing to verify buffer can still be used
    insert(rb, 7); printRingBuffer(rb);
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after insert 7: %zu bytes\n", memoryUsage);

    // Remove all elements and print each time
    while (rb->count > 0) {
        ringBufferRemove(rb);
        printRingBuffer(rb);
    }
    memoryUsage = calculateMemoryUsage(rb);
    printf("Memory usage after flushing: %zu bytes\n", memoryUsage);
    // Test is_empty after removing all elements
    printf("After removal, buffer is %s\n", is_empty(rb) ? "empty" : "not empty");

    
    // Cleanup
    flush(rb);

    
    // Insert initial data
    insert(rb, 10); // Index 0
    insert(rb, 20); // Index 1
    printf("Initial data inserted.\n");

    // Simulate shared access by "copying" data to another part of the buffer without actual copy
    // For demonstration purposes, we'll just simulate this logically since our structure doesn't directly support shared access
    printf("Simulating shared data access...\n");
    insert(rb, rb->buffer[0]->data); // Index 2, simulating shared access to the value at index 0
    rb->buffer[2]->refCount = rb->buffer[0]->refCount + 1; // Increment refCount to simulate sharing

    // Modify data at index 1, which should not be shared and can be safely modified
    printf("Modifying data at index 1 to 25...\n");
    modifyData(rb, 1, 25);

    // Now, modify data at index 0, which is shared, to trigger a copy
    printf("Modifying shared data at index 0 to trigger Copy-On-Write...\n");
    modifyData(rb, 0, 15);

    // Output the results to verify correct behavior
    for (int i = 0; i < 3; ++i) {
        printf("Data at index %d: %d, RefCount: %d\n", i, rb->buffer[i]->data, rb->buffer[i]->refCount);
    }

    // Expected output:
    // Data at index 0: 15, RefCount: 1 (modified and copied)
    // Data at index 1: 25, RefCount: 1 (modified directly)
    // Data at index 2: 10, RefCount: 2 (original data before modification, refCount shared with simulated "copy")

    // Cleanup
    flush(rb);

    
    
    // test copy on write
    // Initialize a ring buffer with some data
    
    // Assuming rb is already initialized and used
    // resizeRingBuffer(rb, 10);

    for (int i = 0; i < 5; ++i) {
        insert(rb, i);
    }

    // Clone the ring buffer
    RingBuffer* rb_clone = initialize(rb->size);
    for (int i = rb->tail; i != rb->head; i = (i + 1) % rb->size) {
        insert(rb_clone, rb->buffer[i]->data);
    }

    // Modify the clone
    modifyData(rb_clone, 0, 99);

    // Verify the original is unchanged
    assert(rb->buffer[rb->tail]->data != 99);

    // Check if the original data was duplicated only after modification
    // This can be done by checking the reference count of the original data
    assert(rb->buffer[rb->tail]->refCount == 1);

    // Clean up
    flush(rb);
    flush(rb_clone);
    free(rb->buffer);
    free(rb_clone->buffer);
    // free(rb);
    // free(rb_clone);

    printf("Copy-on-write test passed.\n");
    
    
    return 0;
}
