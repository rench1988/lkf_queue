#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include "lkf_queue.h"

#define MAXNUM 1000000


void *q_reader(void *arg) 
{
	lkf_queue_t *queue = (lkf_queue_t *)arg;

	int i, num;
	for (i = 1; i <= MAXNUM; i++) {
		do {
			num = (int)lkf_queue_pop(queue);
		} while (!num);

		assert(num == i);
	}

	return NULL;
}

void *q_writer(void *arg)
{
	lkf_queue_t *queue = (lkf_queue_t *)arg;

	int i;
	for (i = 1; i <= MAXNUM; ++i) {
		lkf_queue_push(queue, i);
	}

	return NULL;
}

void *r_reader(void *arg)
{
	lkf_ring_t *ring = (lkf_ring_t *)arg;

	int i, num;
	for (i = 1; i <= MAXNUM; i++) {
		do {
			num = lkf_ring_pop(ring);
		} while(!num);

		assert(num == i);
	}

	return NULL;
}

void *r_writer(void *arg)
{
	lkf_ring_t *ring = (lkf_ring_t *)arg;

	int i;
	for (i = 1; i <= MAXNUM; i++) {
		while (lkf_ring_push(ring, i)) {
			;
		}
	}

	return NULL;
}


int main(int argc, char const *argv[])
{
	pthread_t tid[2];

	lkf_queue_t *queue = lkf_queue_init();

	pthread_create(&tid[0], NULL, q_reader, queue);
	pthread_create(&tid[1], NULL, q_writer, queue);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	printf("Queue Test Passed!\n");

	lkf_ring_t *ring = lkf_ring_init(127);

	pthread_create(&tid[0], NULL, r_reader, ring);
	pthread_create(&tid[1], NULL, r_writer, ring);

	pthread_join(tid[1], NULL);
	pthread_join(tid[0], NULL);

	printf("Ring Test Passed\n");
	return 0;
}

