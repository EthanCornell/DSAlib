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

#ifndef SET_H
#define SET_H

#define SET_CAPACITY 100  // Define the capacity of the set

typedef struct {
    int items[SET_CAPACITY];  // Array to store set elements
    int count;                // Number of elements in the set
} Set;

// Function to initialize the set
void initializeSet(Set *set);

// Function to insert an element into the set
int insertToSet(Set *set, int item);

// Function to delete an element from the set
int deleteFromSet(Set *set, int item);

// Function to check if an element is in the set
int isInSet(Set *set, int item);

#endif // SET_H
