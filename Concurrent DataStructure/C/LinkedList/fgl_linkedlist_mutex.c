#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Fine-Grained Locking linked list with Mutex
typedef struct FGL_Node {
    int data;
    struct FGL_Node* next;
    pthread_mutex_t lock;
} FGL_Node;


void fgl_list_init(FGL_Node** head) {
    // Allocate and initialize your head node here, if necessary
    *head = NULL;
}


FGL_Node* fgl_create_node(int data) {
    FGL_Node* newNode = (FGL_Node*)malloc(sizeof(FGL_Node));
    if (newNode == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    pthread_mutex_init(&newNode->lock, NULL);
    return newNode;
}

void fgl_list_insert(FGL_Node** head, int data) {
    FGL_Node* newNode = fgl_create_node(data);
    
    // Special handling if the list is empty
    if (*head == NULL) {
        *head = newNode;
    } else {
        // Lock the head only if it is not NULL
        pthread_mutex_lock(&(*head)->lock);
        newNode->next = *head; // Point the new node to the current head
        *head = newNode;
        pthread_mutex_unlock(&(*head)->lock); // Unlock the head
    }
}


void fgl_list_delete(FGL_Node ** head, int data) {
    if (*head == NULL) return;

    FGL_Node *temp = *head, *prev = NULL;

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->data == data) {
        *head = temp->next; // Changed head
        pthread_mutex_unlock(&temp->lock);
        free(temp); // free old head
        return;
    }

    // Search for the key to be deleted, keep track of the previous node
    while (temp != NULL && temp->data != data) {
        if (prev != NULL) {
            pthread_mutex_unlock(&prev->lock);
        }
        prev = temp;
        temp = temp->next;
        if (temp != NULL) {
            pthread_mutex_lock(&temp->lock); // Lock current node to be checked
        }
    }

    // If key was not present in linked list
    if (temp == NULL) {
        if (prev != NULL) {
            pthread_mutex_unlock(&prev->lock);
        }
        return;
    }

    // Unlink the node from linked list
    if (prev != NULL) {
        prev->next = temp->next;
        pthread_mutex_unlock(&prev->lock);
    }

    pthread_mutex_unlock(&temp->lock);
    free(temp); // Free memory
}


int fgl_list_search(FGL_Node* head, int data) {
    FGL_Node* current = head;
    while (current != NULL) {
        pthread_mutex_lock(&current->lock); // Lock the current node
        if (current->data == data) {
            pthread_mutex_unlock(&current->lock); // Unlock before returning
            return 1; // Data found
        }
        pthread_mutex_unlock(&current->lock); // Unlock the current node
        current = current->next; // Move to the next node
    }
    return 0; // Data not found
}

// gcc -pg fgl_linkedlist.c -o fgl_linkedlist -pthread -O3