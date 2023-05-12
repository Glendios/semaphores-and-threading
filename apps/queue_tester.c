#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)                 \
	do                                      \
	{                                       \
		printf("ASSERT: " #assert " ... "); \
		if (assert)                         \
		{                                   \
			printf("PASS\n");               \
		}                                   \
		else                                \
		{                                   \
			printf("FAIL\n");               \
			exit(1);                        \
		}                                   \
	} while (0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void **)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Callback function that increments items */
static void iterator_inc(queue_t q, void *data)
{
	int *a = (int *)data;

	if (*a == 42)
	{
		queue_delete(q, data);
	}
	else
		*a += 1;
}

void test_iterator(void)
{
	printf("IN test_iterator\n");
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
	size_t i;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	/* Increment every item of the queue, delete item '42' */
	queue_iterate(q, iterator_inc);
	assert(data[0] == 2);
	assert(queue_length(q) == 9);

	queue_destroy(q);
}

void test_delete(void)
{
	printf("IN test_iterator\n");
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
	size_t i;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	/* Increment every item of the queue, delete item '42' */
	queue_iterate(q, iterator_inc);
	assert(data[0] == 2);
	assert(queue_length(q) == 9);

	queue_destroy(q);
}

void test_delete_multiple(void)
{
	printf("IN test_iterator\n");
	queue_t q;
	int data[] = {1, 5, 42, 4, 5, 42, 42, 42, 8, 9};
	size_t i;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	/* Increment every item of the queue, delete 42's */
	queue_iterate(q, iterator_inc);
	assert(data[0] == 2);
	assert(queue_length(q) == 6);

	queue_destroy(q);
}

/* tests iterator and return of entity not found */
void test_iterator_no_Num(void)
{
	printf("IN test_iterator\n");
	queue_t q;
	int data[] = {1, 2, 3, 6, 7, 8, 9};
	size_t i;

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	/* Increment every item of the queue, delete item '42' */
	/* data should remain the same */
	queue_iterate(q, iterator_inc);
	assert(data[0] == 2);
	assert(queue_length(q) == 7);

	queue_destroy(q);
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_iterator();
	test_delete();
	test_iterator_no_Num();
	test_delete_multiple();
	printf("end test\n");
	return 0;
}
