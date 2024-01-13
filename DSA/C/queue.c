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

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// The Queue structure includes an array (items), indices for the front and rear elements, the capacity of the queue, and its current size.
// createQueue initializes the queue, setting the front to 0, the rear to the last position (for circular functionality), and allocating memory for the array.
// isFull and isEmpty check if the queue is full or empty
// respectively, based on the size and capacity.

// enqueue adds an item to the rear of the queue. It wraps around to the beginning of the array if the end is reached (circular queue behavior).
// dequeue removes an item from the front of the queue and returns it. The front index is incremented and wrapped around if necessary.
// front returns the front item of the queue without removing it.
// deleteQueue frees the memory allocated for the queue and its items array.
// The main function demonstrates the usage of the queue API with test cases.


// Create a queue of given capacity
Queue* createQueue(int capacity) {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1; // Important for circular queue
    queue->items = (int *)malloc(sizeof(int) * capacity);
    return queue;
}

// Check if the queue is full
int isFull(Queue *queue) {
    return (queue->size == queue->capacity);
}

// Check if the queue is empty
int isEmpty(Queue *queue) {
    return (queue->size == 0);
}

// Add an item to the queue
void enqueue(Queue *queue, int item) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->items[queue->rear] = item;
    queue->size = queue->size + 1;
}

// Remove an item from the queue
int dequeue(Queue *queue) {
    if (isEmpty(queue))
        return -1;
    int item = queue->items[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Get the front item of the queue
int front(Queue *queue) {
    if (isEmpty(queue))
        return -1;
    return queue->items[queue->front];
}

// Delete the queue
void deleteQueue(Queue *queue) {
    if (queue) {
        if (queue->items) {
            free(queue->items);
        }
        free(queue);
    }
}

// Time and Space Complexity Analysis
// Time Complexity:

// enqueue, dequeue, isFull, isEmpty, front: O(1) - These operations are constant time as they involve simple arithmetic and conditional checks.
// createQueue and deleteQueue: O(1) - Memory allocation and deallocation are generally considered constant time operations.
// Space Complexity:

// O(n) - The main space usage comes from the dynamic array that stores the queue elements. Here, n is the capacity of the queue. The space for the queue structure itself is constant.

// Main function with test cases
int main() {
    Queue *queue = createQueue(5);

    enqueue(queue, 10);
    enqueue(queue, 20);
    enqueue(queue, 30);
    printf("Front item: %d\n", front(queue));
    printf("Dequeued item: %d\n", dequeue(queue));
    printf("Front item after dequeue: %d\n", front(queue));

    deleteQueue(queue);
    return 0;
}
