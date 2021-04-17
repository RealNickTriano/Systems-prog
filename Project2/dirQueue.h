// how many threads are currently putting into dir q
// last thread waiting to deq, at that
//point instead of waiting we need to close dir q,
//wake up other threads and let them terminate
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#ifndef QSIZE
#define QSIZE 8
#endif

#ifndef DEBUG:wq
#define DEBUG 0
#endif

typedef struct
{
	char *data[QSIZE];
	unsigned count;
	unsigned head;
	int open;
    int active_threads;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} dir_queue_t;

int init_dir(dir_queue_t *Q, int a)
{
	Q->count = 0;
	Q->head = 0;
	Q->open = 1;
    Q->active_threads = a;
	pthread_mutex_init(&Q->lock, NULL);
	pthread_cond_init(&Q->read_ready, NULL);
	pthread_cond_init(&Q->write_ready, NULL);

	return 0;
}

int destroy_dir(dir_queue_t *Q)
{
	pthread_mutex_destroy(&Q->lock);
	pthread_cond_destroy(&Q->read_ready);
	pthread_cond_destroy(&Q->write_ready);

	return 0;
}

// add item to end of queue
// if the queue is full, block until space becomes available
int enqueue_dir(dir_queue_t *Q, char *item)
{
	pthread_mutex_lock(&Q->lock);

	while (Q->open)
	{
		pthread_cond_wait(&Q->write_ready, &Q->lock);
	}
	if (!Q->open)
	{
		pthread_mutex_unlock(&Q->lock);
		return -1;
	}

	unsigned i = Q->head + Q->count;
	if (i >= QSIZE){
        size_t data_size = 2 * sizeof(Q->data);
        char **data_new = (char**)realloc(Q->data, sizeof(char*) * data_size);
        if (!data_new) return 1;

        //Q->data = data_new;
		memcpy(Q->data, data_new, sizeof(char*) * data_size);

        if (DEBUG) printf("Increased size to %lu\n", sizeof(Q->data));
    } 

	Q->data[i] = item;
	++Q->count;

	pthread_cond_signal(&Q->read_ready);

	pthread_mutex_unlock(&Q->lock);
	if (DEBUG)
	{
		printf("enqueued - %s\n", item);
	}

	return 0;
}

char *dequeue_dir(dir_queue_t *Q, char *item)
{
	pthread_mutex_lock(&Q->lock);

    if (Q->count == 0)
    {
        Q->active_threads--;
    }
    if (Q->active_threads <= 0)
    {
        pthread_mutex_unlock(&Q->lock);
        pthread_cond_broadcast(&Q->read_ready);
        return NULL;
    }
	while (Q->count == 0 && Q->active_threads > 0 && Q->open)
	{
		
		pthread_cond_wait(&Q->read_ready, &Q->lock);
		
	}
	if (Q->count == 0)
	{
		pthread_mutex_unlock(&Q->lock);
		return NULL;
	}
    Q->active_threads++;

	item = Q->data[Q->head];

	--Q->count;
	++Q->head;

	pthread_mutex_unlock(&Q->lock);
	if (DEBUG)
	{
		printf("dequeued - %s\n", item);
	}

	return item;
}

int qclose_dir(dir_queue_t *Q)
{
	pthread_mutex_lock(&Q->lock);
	Q->open = 0;
	pthread_cond_broadcast(&Q->read_ready);
	pthread_cond_broadcast(&Q->write_ready);
	pthread_mutex_unlock(&Q->lock);

	return 0;
}

int printQueue_dir(dir_queue_t *Q)
{
	for (int i = 0; i < Q->count; i++)
	{
		printf("%s\n", Q->data[i]);
	}
}

