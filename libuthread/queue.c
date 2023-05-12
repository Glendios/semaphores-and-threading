#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

typedef struct node
{
  void *data;
  struct node *next;
  struct node *prev;
} node;

typedef struct node *node_t;

typedef struct queue
{
  int length;
  struct node *front;
  struct node *back;
} queue;

node_t node_create(void)
{
  node_t node_ref = (node_t)malloc(sizeof(node));
  if (node_ref == NULL)
  {
    return NULL;
  }
  node_ref->next = NULL;
  node_ref->prev = NULL;
  return node_ref;
}

queue_t queue_create(void)
{
  queue_t queue_ref = (queue_t)malloc(sizeof(queue));
  if (queue_ref == NULL)
  {
    return NULL;
  }
  queue_ref->front = NULL;
  queue_ref->back = NULL;
  queue_ref->length = 0;
  return queue_ref;
}

int queue_destroy(queue_t queue)
{
  if (queue == NULL)
  {
    return -1;
  }
  if (queue->length != 0)
  {
    return -1;
  }
  free(queue);
  return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
  // check for NULL values and memory allocation error
  if (queue == NULL || data == NULL)
  {
    return -1;
  }

  node_t newNode = (node_t)malloc(sizeof(struct node));
  if (newNode == NULL)
    return -1;
  newNode->data = data;
  newNode->next = NULL;

  if (queue->length != 0)
  { // adding to non-empty queue
    queue->back->next = newNode;
    newNode->prev = queue->back;
  }
  else
  { // adding to empty queue
    queue->front = newNode;
    newNode->prev = NULL;
  }
  queue->length++;
  queue->back = newNode;
  return 0;
}

int helper(queue_t queue, node_t current)
{
  if (current == NULL)
  {
    return -1;
  }
  if (current == queue->front)
  {
    queue->front = current->next;
  }
  if (current == queue->back)
  {
    queue->back = current->prev;
  }
  if (current->prev)
    current->prev->next = current->next;
  if (current->next)
    current->next->prev = current->prev;
  free(current);
  queue->length--;
  return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
  if (queue == NULL || data == NULL)
  {
    return -1;
  }
  if (queue->front == NULL)
  {
    return -1;
  }

  *data = queue->front->data;

  helper(queue, queue->front);
  return 0;
}

int queue_delete(queue_t queue, void *data)
{
  if (queue == NULL || data == NULL)
  {
    return -1;
  }
  node *current = queue->front;

  while (current != NULL && current->data != data)
  {
    current = current->next;
  }
  return helper(queue, current);
}

int queue_iterate(queue_t queue, queue_func_t func)
{
  if (queue == NULL || func == NULL)
  {
    return -1;
  }
  node_t current = queue->front;
  while (current != NULL)
  {
    node_t next = current->next;
    func(queue, current->data);
    current = next;
  }
  return 0;
}

int queue_length(queue_t queue)
{
  if (queue == NULL)
  {
    return -1;
  }
  return queue->length;
}
