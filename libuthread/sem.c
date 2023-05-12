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

sem_t sem_create(size_t count)
{
	preempt_disable();
	sem_t sem = malloc(sizeof(semaphore));
	if (sem == NULL)
		return NULL;

	preempt_enable();
	sem->count = count;
	sem->blocked_threads = queue_create();
	sem->unblocked_thread = NULL;

	return sem;
}

/* Destroys semaphore if found */
int sem_destroy(sem_t sem)
{
	preempt_disable();
	if (sem == NULL || queue_length(sem->blocked_threads) != 0)
	{
		return -1;
	}
	// check if unblocked threads are still being processed.
	if ((sem->unblocked_thread) != 0)
	{
		return -1;
	}
	preempt_enable();
	// if no unblocked threads still processing, free the memory
	queue_destroy(sem->blocked_threads);
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	preempt_disable();

	if (sem == NULL)
	{
		return -1;
	}

	while (sem->count == 0)
	{
		struct uthread_tcb *running_thread = uthread_current();
		queue_enqueue(sem->blocked_threads, running_thread);
		uthread_block();
	}
	sem->unblocked_thread = 0;
	if (sem->count > 0)
	{
		sem->count--;
	}

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

	if (queue_length(sem->blocked_threads) != 0)
	{
		queue_dequeue(sem->blocked_threads, (void **)&(sem->unblocked_thread));
		uthread_unblock(sem->unblocked_thread);
	}

	sem->count++;
	preempt_enable();
	return 0;
}
