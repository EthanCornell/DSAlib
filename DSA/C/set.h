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
