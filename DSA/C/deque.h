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
