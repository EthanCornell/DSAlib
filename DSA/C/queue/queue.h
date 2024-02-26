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
