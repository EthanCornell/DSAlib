#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    atomic_uintptr_t top;
} WaitFreeStack;

void stack_init(WaitFreeStack* stack) {
    atomic_store(&stack->top, (uintptr_t)NULL);
}

Node* create_node(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    node->data = data;
    node->next = NULL;
    return node;
}

void stack_push(WaitFreeStack* stack, int data) {
    Node* newNode = create_node(data);
    Node* oldTop;
    do {
        oldTop = (Node*)atomic_load(&stack->top);
        newNode->next = oldTop;
    // Attempt to set the new top, if the top hasn't changed since this operation started
    } while (!atomic_compare_exchange_weak(&stack->top, (uintptr_t*)&oldTop, (uintptr_t)newNode));
}

//  atomic pop operation that tries to minimize waiting by using atomic compare-and-exchange operations to update the stack's top
int stack_pop(WaitFreeStack* stack, int* poppedValue) {
    Node* oldTop;
    Node* newTop;
    do {
        oldTop = (Node*)atomic_load(&stack->top);
        if (oldTop == NULL) {
            return 0; // Stack is empty, cannot pop
        }
        newTop = oldTop->next;
        // Attempt to set the new top, if the top hasn't changed since this operation started
        
    } while (!atomic_compare_exchange_weak(&stack->top, (uintptr_t*)&oldTop, (uintptr_t)newTop));

    *poppedValue = oldTop->data; // Retrieve the value to return
    free(oldTop); // Free the popped node
    return 1; // Success
}


// int main() {
//     WaitFreeStack stack;
//     stack_init(&stack);
    
//     // Example usage
//     stack_push(&stack, 10);
//     printf("Pushed: %d",10);
//     stack_push(&stack, 20);
//     printf("Pushed: %d",20);
//     int value;
//     if (stack_pop(&stack, &value)) {
//         printf("Popped: %d\n", value);
//     } else {
//         printf("Failed to pop: Stack might be empty.\n");
//     }

//     // Continue with stack operations...

//     return 0;
// }

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

typedef struct {
    WaitFreeStack* stack;
    int thread_id;
} ThreadData;

void* thread_push(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int value = data->thread_id;
    for (int i = 0; i < 10; ++i) {
        stack_push(data->stack, value * 10 + i);
    }
    return NULL;
}

void* thread_pop(void* arg) {
    WaitFreeStack* stack = (WaitFreeStack*)arg;
    int poppedValue;
    for (int i = 0; i < 10; ++i) {
        if (stack_pop(stack, &poppedValue)) {
            printf("Thread popped: %d\n", poppedValue);
        } else {
            printf("Thread failed to pop, stack might be empty.\n");
        }
    }
    return NULL;
}

int main() {
    WaitFreeStack stack;
    stack_init(&stack);

    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS / 2]; // Half for push, half for pop

    // Create threads for pushing
    for (int i = 0; i < NUM_THREADS / 2; ++i) {
        data[i].stack = &stack;
        data[i].thread_id = i;
        pthread_create(&threads[i], NULL, thread_push, &data[i]);
    }

    // Create threads for popping
    for (int i = NUM_THREADS / 2; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_pop, &stack);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}