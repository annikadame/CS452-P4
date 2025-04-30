#include "lab.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

struct queue
{
    // Array to hold items
    void **buffer;         
    int capacity;
    int size; 
    // Index of the front item             
    int front;
    // Index where next item will be inserted
    int rear;
    bool shutdown;           

    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
};

// Initialzes a new queue
queue_t queue_init(int capacity)
{
    queue_t q = (queue_t)malloc(sizeof(struct queue));
    if (!q)
    {
        perror("Error allocating memory for queue.");
        return NULL;
    }
    q->buffer = (void **)malloc(sizeof(void *) * capacity);
    if (!q->buffer)
    {
        perror("Error allocating memory for buffer.");
        free(q);
        return NULL;
    }
    q->capacity = capacity;
    q->size = 0;
    q->front = 0;
    q->rear = 0;
    q->shutdown = false;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    return q;
}


// Frees all memory and related data signals
void queue_destroy(queue_t q)
{
    if (q == NULL)
    {
        return;
    }

    pthread_mutex_lock(&q->lock);
    // Temp save buffer pointer
    void **buf = q->buffer;
    // Detach to safely free outside lock
    q->buffer = NULL;
    pthread_mutex_unlock(&q->lock);

    // Free buffer outside of critical section
    if(buf) free(buf);

    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);

    free(q);
}


 // Adds an element to the back of the queue
void enqueue(queue_t q, void *data)
{

    // Avoid enqueuing NULL pointer
    if (data == NULL) return;
    pthread_mutex_lock(&q->lock);
    while (q->size == q->capacity && !q->shutdown) {
        //Wait until space is available
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    if (q->shutdown) {
        //Exit early if queue is shutting down
        pthread_mutex_unlock(&q->lock);
        return;
    }

    q->buffer[q->rear] = data;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

 // Removes the first element in the queue.
void *dequeue(queue_t q)
{
    pthread_mutex_lock(&q->lock);

    while (q->size == 0 && !q->shutdown) {
        // Wait for items to be enqueued
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    if (q->size == 0 && q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    void *data = q->buffer[q->front];
    // Wrap front index circularly
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    pthread_cond_signal(&q->not_full); // signal that space is available
    pthread_mutex_unlock(&q->lock);
    return data;
}

// Set the shutdown flag in the queue so all threads can complete and exit properly
void queue_shutdown(queue_t q)
{
    pthread_mutex_lock(&q->lock);
    // Signal shutdown to all threads
    q->shutdown = true;
    pthread_cond_broadcast(&q->not_full);
    pthread_cond_broadcast(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

// Returns true if the queue is empty
bool is_empty(queue_t q)
{
    pthread_mutex_lock(&q->lock);
    bool empty = (q->size == 0);
    pthread_mutex_unlock(&q->lock);
    return empty;
}

// Returns true if the shutdown flag is set
bool is_shutdown(queue_t q)
{
    pthread_mutex_lock(&q->lock);
    bool down = q->shutdown;
    pthread_mutex_unlock(&q->lock);
    return down;
}
