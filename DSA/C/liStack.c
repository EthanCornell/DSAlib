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

// struct frame: Represents a node in the stack.
// struct stack: Represents the stack itself.
// initialize(): Sets up an empty stack.
// push(): Adds an item to the top of the stack.
// pop(): Removes and returns the top item from the stack.

#include "liStack.h"

// Initialize the stack
void initialize(struct stack *stk) {
    stk->head = NULL;
    stk->size = 0;
}

// Push an item onto the stack
void push(struct stack *stk, item x) {
    struct frame *newhead = (struct frame *)malloc(sizeof(struct frame));
    if (newhead == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    newhead->data = x;
    newhead->next = stk->head;
    stk->head = newhead;
    stk->size++;
}

// Pop an item from the stack
item pop(struct stack *stk) {
    if (stk->head == NULL) {
        fprintf(stderr, "Stack underflow error\n");
        exit(EXIT_FAILURE);
    }
    struct frame *oldhead = stk->head;
    item r = oldhead->data;
    stk->head = oldhead->next;
    stk->size--;
    free(oldhead);
    return r;
}

// Main function with test cases
int main() {
    struct stack myStack;
    initialize(&myStack);

    push(&myStack, 10);
    push(&myStack, 20);
    printf("Popped: %d\n", pop(&myStack));
    printf("Popped: %d\n", pop(&myStack));

    // Attempt to pop from an empty stack
    printf("Popped: %d\n", pop(&myStack));

    return 0;
}
