#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

static queue_t queue_ready;
static queue_t queue_waiting;
struct uthread_tcb *current_thread;
struct uthread_tcb *used_thread;
struct uthread_tcb *next_thread;

typedef enum
{
  Ready,
  Running,
  Blocked
} threadState;

struct uthread_tcb
{
  uthread_ctx_t context;
  void *stack;
  threadState state;
};

struct uthread_tcb *uthread_current(void)
{
  return current_thread;
}

void uthread_yield(void)
{
  preempt_disable();

  if (queue_dequeue(queue_ready, (void **)(&next_thread)) == 0)
  {

    //Put used_thread thread in ready queue and switch to the next.
    current_thread->state = Ready;
    next_thread->state = Running;
    queue_enqueue(queue_ready, current_thread);
    used_thread = current_thread;
    current_thread = next_thread;
    uthread_ctx_switch(&(used_thread->context), &(next_thread->context));

    preempt_enable();
  }
}

void uthread_exit(void)
{
  preempt_disable();
  if (uthread_current() == NULL)
  {
    exit(0);
  }

  //get the next ready thread out of the ready queue.
  queue_dequeue(queue_ready, (void **)(&next_thread));
  next_thread->state = Running;

  //update threads and change context before exiting
  used_thread = current_thread;
  current_thread = next_thread;
  uthread_ctx_switch(&(used_thread->context), &(next_thread->context));

  preempt_enable();
  exit(0);
}

int uthread_create(uthread_func_t func, void *arg)
{

  struct uthread_tcb *new_thread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
  //fail to create
  if (new_thread == NULL)
  {
    return -1;
  }
  new_thread->stack = uthread_ctx_alloc_stack();
  if (uthread_ctx_init(&(new_thread->context), new_thread->stack, func, arg) == 1)
  {
    return -1;
  }
  new_thread->state = Ready;

  preempt_disable();
  queue_enqueue(queue_ready, new_thread);
  preempt_enable();

  return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
  queue_ready = queue_create();
  queue_waiting = queue_create();

  struct uthread_tcb *new_tcb = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
  if (new_tcb == NULL)
    return -1;
  new_tcb->stack = NULL;
  new_tcb->state = Running;
  current_thread = new_tcb;

  uthread_create(func, arg);
  preempt_start(preempt);

  while (true)
  {
    if (queue_length(queue_ready) == 0 && queue_length(queue_waiting) == 0)
    {
      queue_destroy(queue_ready);
      queue_destroy(queue_waiting);
      queue_ready = NULL;
      queue_waiting = NULL;
      return 0;
    }
    uthread_yield();
  }
  preempt_stop();
}

void uthread_block(void)
{
  preempt_disable();
  if (queue_length(queue_ready) != 0)
  {
    queue_dequeue(queue_ready, (void **)(&next_thread));
    current_thread->state = Blocked;
  }

  queue_enqueue(queue_waiting, current_thread);
  next_thread->state = Running;
  used_thread = current_thread;
  current_thread = next_thread;

  uthread_ctx_switch(&(used_thread->context), &(next_thread->context));

  preempt_enable();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
  preempt_disable();

  queue_delete(queue_waiting, uthread);

  uthread->state = Ready;
  queue_enqueue(queue_ready, uthread);

  preempt_enable();
}