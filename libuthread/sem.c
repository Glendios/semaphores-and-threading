#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "uthread.h"
#include "queue.h"
#include "sem.h"
#include "private.h"

typedef struct semaphore
{
	int count;
	queue_t blocked_threads;
	struct uthread_tcb *unblocked_thread;
} semaphore;

/* Initializes and allocates a semaphore of count 'count'*/
sem_t sem_create(size_t count)
{
	sem_t sem = malloc(sizeof(semaphore));
	if (sem == NULL)
		return NULL;

	//Initialize variables
	sem->count = count;
	sem->blocked_threads = queue_create();
	sem->unblocked_thread = NULL;

	return sem;
}

/* Destroys semaphore if found */
int sem_destroy(sem_t sem)
{

	// check if NULL, if true return -1
	if (sem == NULL)
	{
		return -1;
	}
	if (queue_length(sem->blocked_threads) != 0)
	{
		return -1;
	}

	// check if unblocked threads are still being processed.
	if ((sem->unblocked_thread) != 0)
	{
		return -1;
	}
	// if no unblocked threads still processing, free the memory
	queue_destroy(sem->blocked_threads);
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{

	if (sem == NULL)
	{
		return -1;
	}

	preempt_disable();

	// check resources before decrement and block
	if (sem->count == 0)
		do
		{
			queue_enqueue(sem->blocked_threads, uthread_current());
			uthread_block();
			preempt_disable();
		} while (sem->count == 0);
	sem->unblocked_thread = 0;
	sem->count--;

	preempt_enable();

	return 0;
}

int sem_up(sem_t sem)
{

	preempt_disable();
	if (sem == NULL)
	{
		return -1;
	}

	/*increment resource*/
	sem->count++;

	/*unblock threads*/
	if (queue_length(sem->blocked_threads) != 0)
	{
		queue_dequeue(sem->blocked_threads, (void **)&(sem->unblocked_thread));
		uthread_unblock(sem->unblocked_thread);
	}

	preempt_enable();
	return 0;
}
