#ifndef QUEUE_H
#define QUEUE_H

// Define the queue structure
typedef struct {
    int *items;     // Dynamic array to store queue elements
    int front;      // Index of the front element
    int rear;       // Index of the rear element
    int capacity;   // Maximum capacity of the queue
    int size;       // Current size of the queue
} Queue;

// Function to create a queue of given capacity
Queue* createQueue(int capacity);

// Function to check if the queue is full
int isFull(Queue *queue);

// Function to check if the queue is empty
int isEmpty(Queue *queue);

// Function to add an item to the queue
void enqueue(Queue *queue, int item);

// Function to remove an item from the queue
int dequeue(Queue *queue);

// Function to get the front item of the queue
int front(Queue *queue);

// Function to delete the queue
void deleteQueue(Queue *queue);

#endif // QUEUE_H
