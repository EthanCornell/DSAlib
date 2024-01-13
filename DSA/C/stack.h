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
