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
#include "stack.h"


// Stack Structure: This defines the data structure for the stack, including the dynamic array (items), the index of the top element (top), and the stack's capacity (capacity).
// Define the stack structure
// typedef struct {
//     int *items;     // Dynamic array to store stack elements
//     int top;        // Index of the top element
//     int capacity;   // Maximum capacity of the stack
// } Stack;


// createStack Function: Allocates memory for the stack and its items array. It initializes top to -1 (empty stack) and sets the capacity.
// Function to create a stack of given capacity
Stack* createStack(int capacity) {
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->items = (int *)malloc(sizeof(int) * capacity);
    return stack;
}


// **isFull & isEmpty
// Functions**: These are utility functions to check if the stack is full or empty, respectively. isFull returns true if the top index is equal to the last index of the array, and isEmpty returns true if the top index is -1.
// Function to check if the stack is full
int isFull(Stack *stack) {
    return stack->top == stack->capacity - 1;
}

// Function to check if the stack is empty
int isEmpty(Stack *stack) {
    return stack->top == -1;
}

// push Function: Adds an item to the stack. It first checks if the stack is full. If not, it increments the top index and stores the new item at this position.
// Function to add an item to the stack
void push(Stack *stack, int item) {
    if (isFull(stack)) {
        return; // Optionally, resize the stack here
    }
    stack->items[++stack->top] = item;
}

// pop Function: Removes and returns the top item from the stack. If the stack is empty, it returns -1 as an indication. Otherwise, it returns the top item and decrements the top index.
// Function to remove an item from the stack
int pop(Stack *stack) {
    if (isEmpty(stack)) {
        return -1; // Indicate that the stack is empty
    }
    return stack->items[stack->top--];
}

// peek Function: Returns the top item of the stack without removing it. If the stack is empty, it returns -1.
// Function to get the top item of the stack
int peek(Stack *stack) {
    if (isEmpty(stack)) {
        return -1;
    }
    return stack->items[stack->top];
}

// deleteStack Function: Frees the memory allocated for the stack and its items.
// Function to delete the stack
void deleteStack(Stack *stack) {
    if (stack) {
        if (stack->items) {
            free(stack->items);
        }
        free(stack);
    }
}

// Time and Space Complexity Analysis
// Time Complexity:

// push, pop, isFull, isEmpty, peek: O(1) - These operations perform in constant time as they involve basic arithmetic or conditional operations, irrespective of the stack size.
// createStack and deleteStack: O(1) - Allocation and deallocation of memory take constant time, although the actual time depends on the system's memory management.
// Space Complexity:
// O(n) - The main space consumption is by the dynamic array, which stores the stack items. Here, n is the capacity of the stack. The space for the stack structure itself is constant.


// Main function with test cases
int main() {
    Stack *stack = createStack(5);

    push(stack, 10);
    push(stack, 20);
    push(stack, 30);
    printf("Top item: %d\n", peek(stack));
    printf("Popped item: %d\n", pop(stack));
    printf("Top item after pop: %d\n", peek(stack));

    deleteStack(stack);
    return 0;
}
