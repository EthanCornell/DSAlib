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

#ifndef LISTACK_H
#define LISTACK_H

#include <stdio.h>
#include <stdlib.h>

// Define the item type
typedef int item;

// Structure for a single frame
struct frame {
    item data;
    struct frame *next;
};

// Structure for the stack
struct stack {
    struct frame *head;
    int size;
};

// Function prototypes
void initialize(struct stack *stk);
void push(struct stack *stk, item x);
item pop(struct stack *stk);

#endif // LISTACK_H
