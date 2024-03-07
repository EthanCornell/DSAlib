// the Producer-Consumer problem in C using POSIX threads (pthreads) for creating producer and consumer threads and a semaphore for synchronization.
// counting semaphores. Here's a brief overview of how each semaphore is used:
// empty Semaphore : This semaphore is initialized with the size of the buffer(in this case, BUFFER_SIZE) and
// is used to track the number of empty slots available in the buffer.Before a producer can place an item into the buffer,
// it must perform a sem_wait(&empty) operation, which decrements the semaphore value.This action signifies that an empty slot has been filled.If the buffer is full(meaning no empty slots are available), the semaphore's value would be zero, and the producer will block at the sem_wait(&empty) call until a consumer consumes an item from the buffer, thereby posting (incrementing) the empty semaphore and signaling that there is now an empty slot available.
// full Semaphore : This semaphore is initialized to 0, reflecting that initially,
// there are no items(full slots) in the buffer.It is used to track the number of items in the buffer.When a producer adds an item to the buffer,
// it performs a sem_post(&full) operation,which increments the semaphore value,
// indicating that there's an additional item in the buffer for consumption. Conversely, before a consumer can remove an item from the buffer, it must perform a sem_wait(&full) operation, decrementing the semaphore value to signal that an item has been consumed. If the buffer is empty, the consumer will block at the sem_wait(&full) call until a producer produces an item, thereby posting the full semaphore and indicating that there is an item available for consumption.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void *producer(void *param)
{
    int item;
    for (int i = 0; i < 20; i++)
    {
        // Produce an item
        item = i;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        // Insert item into the buffer
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        printf("Producer produced %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    pthread_exit(0);
}

void *consumer(void *param)
{
    int item;
    for (int i = 0; i < 20; i++)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        // Remove item from the buffer
        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        printf("Consumer consumed %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    pthread_exit(0);
}

int main()
{
    pthread_t tid1, tid2;

    // Initialize semaphore and mutex
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // Create producer and consumer threads
    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // Clean up
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}

// gcc -pg -fsanitize=address -g -o prodcons prodcons.c -lpthread -lrt -O3