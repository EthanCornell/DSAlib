#ifndef DEQUE_H
#define DEQUE_H

typedef struct {
    int *items;     // Dynamic array to store deque elements
    int front;      // Index of the front element
    int rear;       // Index of the rear element
    int capacity;   // Maximum capacity of the deque
    int size;       // Current size of the deque
} Deque;

// Function to create a deque of given capacity
Deque* createDeque(int capacity);

// Function to check if the deque is full
int isFull(Deque *deque);

// Function to check if the deque is empty
int isEmpty(Deque *deque);

// Function to add an item at the front of the deque
void insertFront(Deque *deque, int item);

// Function to add an item at the rear of the deque
void insertRear(Deque *deque, int item);

// Function to delete an item from the front of the deque
int deleteFront(Deque *deque);

// Function to delete an item from the rear of the deque
int deleteRear(Deque *deque);

// Function to get the front item of the deque
int getFront(Deque *deque);

// Function to get the rear item of the deque
int getRear(Deque *deque);

// Function to delete the deque
void deleteDeque(Deque *deque);

#endif // DEQUE_H
