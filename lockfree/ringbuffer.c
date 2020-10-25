// This program implements a lock-free one-producer-one-consumer ring buffer.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <threads.h>
#include <stdatomic.h>

#if ATOMIC_INT_LOCK_FREE != 2
#error The atomic type used is not always lock-free.
#endif

typedef void *T;

struct RingBuffer
{
	T *buffer;
	unsigned size;
	atomic_uint read_p;
	atomic_uint write_p;
};

int RingBuffer_init(struct RingBuffer *rb, unsigned size)
{
	rb->buffer = malloc((size + 1) * sizeof(T));
	if (rb->buffer == NULL) {
		fprintf(stderr, "malloc failed.\n");
		return -1;
	}

	rb->size = size + 1;
	atomic_init(&rb->read_p, 0);
	atomic_init(&rb->write_p, 0);

	return 0;
}

void RingBuffer_destroy(struct RingBuffer *rb)
{
	unsigned read = atomic_load_explicit(&rb->read_p, memory_order_relaxed);
	unsigned write = atomic_load_explicit(&rb->write_p, memory_order_relaxed);
	if (read != write) // if not empty
		fprintf(stderr, "Warning: The buffer is not empty when destruction.\n");
	free(rb->buffer);
}

void RingBuffer_put(struct RingBuffer *rb, T data_unit)
{
	unsigned write = atomic_load_explicit(&rb->write_p, memory_order_relaxed);

	while (true) {
		unsigned read = atomic_load_explicit(&rb->read_p, memory_order_acquire);

		if ((write + 1) % rb->size == read) // if appears full
			thrd_yield();
		else
			break;
	}

	rb->buffer[write] = data_unit;
	write = (write + 1) % rb->size;

	atomic_store_explicit(&rb->write_p, write, memory_order_release);
}

T RingBuffer_get(struct RingBuffer *rb)
{
	unsigned read = atomic_load_explicit(&rb->read_p, memory_order_relaxed);

	while (true) {
		unsigned write = atomic_load_explicit(&rb->write_p, memory_order_acquire);

		if (read == write) // if appears empty
			thrd_yield();
		else
			break;
	}

	T data_unit = rb->buffer[read];
	read = (read + 1) % rb->size;

	atomic_store_explicit(&rb->read_p, read, memory_order_release);

	return data_unit;
}

struct DataUnit
{
	int data;
};

int producer(void *arg)
{
	struct RingBuffer *rb = arg;

	for (int i=0; i<1000000; ++i) {
		struct DataUnit *data_unit = malloc(sizeof(struct DataUnit));
		if (data_unit == NULL) {
			fprintf(stderr, "malloc failed.\n");
			exit(EXIT_FAILURE);
		}
		data_unit->data = i;
		RingBuffer_put(rb, data_unit);
	}

	return 0;
}

int consumer(void *arg)
{
	struct RingBuffer *rb = arg;

	for (int i=0; i<1000000; ++i) {
		struct DataUnit *data_unit = RingBuffer_get(rb);
		if (data_unit->data != i)
			fprintf(stderr, "Error occurred\n");
		free(data_unit);
	}

	return 0;
}

int main(void)
{
	struct RingBuffer rb;
	if (RingBuffer_init(&rb, 3) < 0)
		return EXIT_FAILURE;

	// Create threads
	thrd_t the_producer;
	if (thrd_create(&the_producer, producer, &rb) != thrd_success) {
		fprintf(stderr, "Creating thread failed.\n");
		return EXIT_FAILURE;
	}
	thrd_t the_consumer;
	if (thrd_create(&the_consumer, consumer, &rb) != thrd_success) {
		fprintf(stderr, "Creating thread failed.\n");
		return EXIT_FAILURE;
	}

	int res_producer;
	int res_consumer;
	// Join the threads
	if (thrd_join(the_producer, &res_producer) != thrd_success) {
		fprintf(stderr, "Joining thread failed.\n");
		return EXIT_FAILURE;
	}
	if (thrd_join(the_consumer, &res_consumer) != thrd_success) {
		fprintf(stderr, "Joining thread failed.\n");
		return EXIT_FAILURE;
	}
	if (res_producer != 0) {
		fprintf(stderr, "Error returned from thread.\n");
		return EXIT_FAILURE;
	}
	if (res_consumer != 0) {
		fprintf(stderr, "Error returned from thread.\n");
		return EXIT_FAILURE;
	}

	RingBuffer_destroy(&rb);

	return EXIT_SUCCESS;
}
