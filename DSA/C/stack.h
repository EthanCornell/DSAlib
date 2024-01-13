#ifndef STACK_H
#define STACK_H

// Define the stack structure
typedef struct {
    int *items;     // Dynamic array to store stack elements
    int top;        // Index of the top element
    int capacity;   // Maximum capacity of the stack
} Stack;

// Function to create a stack of given capacity
Stack* createStack(int capacity);

// Function to check if the stack is full
int isFull(Stack *stack);

// Function to check if the stack is empty
int isEmpty(Stack *stack);

// Function to add an item to the stack
void push(Stack *stack, int item);

// Function to remove an item from the stack
int pop(Stack *stack);

// Function to get the top item of the stack
int peek(Stack *stack);

// Function to delete the stack
void deleteStack(Stack *stack);

#endif // STACK_H
