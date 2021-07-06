// This program demonstrates the use of a "monitor" (a mutex + condition varialbles) in the classic producer-consumer problem.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef void *T;

struct RingBuffer
{
	T *buffer;
	unsigned size;
	unsigned read_p;
	unsigned write_p;

	// Monitor (mutex + condition variables)
	pthread_mutex_t mutex;
	pthread_cond_t data_avail;
	pthread_cond_t space_avail;
};

int RingBuffer_init(struct RingBuffer *rb, unsigned size)
{
	rb->buffer = malloc((size + 1) * sizeof(T));
	if (rb->buffer == NULL) {
		fprintf(stderr, "malloc failed.\n");
		return -1;
	}

	rb->size = size + 1;
	rb->read_p = 0;
	rb->write_p = 0;

	// Initialize the monitor
	if (pthread_mutex_init(&rb->mutex, NULL) != 0) {
		fprintf(stderr, "Initializing mutex failed.\n");
		free(rb->buffer);
		return -1;
	}
	if (pthread_cond_init(&rb->data_avail, NULL) != 0) {
		fprintf(stderr, "Initializing condition variable failed.\n");
		if (pthread_mutex_destroy(&rb->mutex) != 0)
			fprintf(stderr, "Destroying mutex failed.\n");
		free(rb->buffer);
		return -1;
	}
	if (pthread_cond_init(&rb->space_avail, NULL) != 0) {
		fprintf(stderr, "Initializing condition variable failed.\n");
		if (pthread_cond_destroy(&rb->data_avail) != 0)
			fprintf(stderr, "Destroying condition variable failed.\n");
		if (pthread_mutex_destroy(&rb->mutex) != 0)
			fprintf(stderr, "Destroying mutex failed.\n");
		free(rb->buffer);
		return -1;
	}

	return 0;
}

void RingBuffer_destroy(struct RingBuffer *rb)
{
	// Destroy the monitor
	if (pthread_cond_destroy(&rb->space_avail) != 0)
		fprintf(stderr, "Destroying condition variable failed.\n");
	if (pthread_cond_destroy(&rb->data_avail) != 0)
		fprintf(stderr, "Destroying condition variable failed.\n");
	if (pthread_mutex_destroy(&rb->mutex) != 0)
		fprintf(stderr, "Destroying mutex failed.\n");

	if (rb->read_p != rb->write_p) // if not empty
		fprintf(stderr, "Warning: The buffer is not empty when destruction.\n");
	free(rb->buffer);
}

int RingBuffer_put(struct RingBuffer *rb, T data_unit)
{
	// Acquire the mutex
	if (pthread_mutex_lock(&rb->mutex) != 0) {
		fprintf(stderr, "Acquiring mutex failed.\n");
		return -1;
	}

	while ((rb->write_p + 1) % rb->size == rb->read_p) // while full
	// Wait on a condition variable
		if (pthread_cond_wait(&rb->space_avail, &rb->mutex) != 0) {
			fprintf(stderr, "Waiting on condition variable failed.\n");
			if (pthread_mutex_unlock(&rb->mutex) != 0)
				fprintf(stderr, "Releasing mutex failed.\n");
			return -1;
		}

	rb->buffer[rb->write_p] = data_unit;
	rb->write_p = (rb->write_p + 1) % rb->size;

	// Release the mutex
	if (pthread_mutex_unlock(&rb->mutex) != 0) {
		fprintf(stderr, "Releasing mutex failed.\n");
		return -1;
	}

	// Signal another thread
	if (pthread_cond_signal(&rb->data_avail) != 0) {
		fprintf(stderr, "Signaling on condition variable failed.\n");
		return -1;
	}

	return 0;
}

int RingBuffer_get(struct RingBuffer *rb, T *data_unit)
{
	// Acquire the mutex
	if (pthread_mutex_lock(&rb->mutex) != 0) {
		fprintf(stderr, "Acquiring mutex failed.\n");
		return -1;
	}

	while (rb->read_p == rb->write_p) // while empty
	// Wait on a condition variable
		if (pthread_cond_wait(&rb->data_avail, &rb->mutex) != 0) {
			fprintf(stderr, "Waiting on condition variable failed.\n");
			if (pthread_mutex_unlock(&rb->mutex) != 0)
				fprintf(stderr, "Releasing mutex failed.\n");
			return -1;
		}

	*data_unit = rb->buffer[rb->read_p];
	rb->read_p = (rb->read_p + 1) % rb->size;

	// Release the mutex
	if (pthread_mutex_unlock(&rb->mutex) != 0) {
		fprintf(stderr, "Releasing mutex failed.\n");
		return -1;
	}

	// Signal another thread
	if (pthread_cond_signal(&rb->space_avail) != 0) {
		fprintf(stderr, "Signaling on condition variable failed.\n");
		return -1;
	}

	return 0;
}

struct DataUnit
{
	int source_thread_id;
	char data;
};

struct Args // Arguments passed to threads
{
	int thread_id;
	struct RingBuffer *rb;
};

void *producer(void *args)
{
	int thread_id = ((struct Args *)args)->thread_id;
	struct RingBuffer *rb = ((struct Args *)args)->rb;

	for (int i=0; i<10; ++i) {
		struct DataUnit *data_unit = malloc(sizeof(struct DataUnit));
		if (data_unit == NULL) {
			fprintf(stderr, "malloc failed.\n");
			exit(EXIT_FAILURE);
		}
		data_unit->source_thread_id = thread_id;
		data_unit->data = (char)('a' + i);
		if (RingBuffer_put(rb, data_unit) < 0)
			exit(EXIT_FAILURE);
	}

	return NULL;
}

void *consumer(void *args)
{
	int thread_id = ((struct Args *)args)->thread_id;
	struct RingBuffer *rb = ((struct Args *)args)->rb;

	for (int i=0; i<8; ++i) {
		struct DataUnit *data_unit;
		if (RingBuffer_get(rb, (void**)&data_unit) < 0)
			exit(EXIT_FAILURE);
		printf("Thread %d got %c from thread %d\n", thread_id, data_unit->data, data_unit->source_thread_id);
		free(data_unit);
	}

	return NULL;
}

#define NUM_PRODUCERS 4
#define NUM_CONSUMERS 5

int main(void)
{
	struct RingBuffer rb;
	if (RingBuffer_init(&rb, 3) < 0)
		return EXIT_FAILURE;

	struct Args args_producer[NUM_PRODUCERS];
	for (int i=0; i<NUM_PRODUCERS; ++i) {
		args_producer[i].thread_id = i;
		args_producer[i].rb = &rb;
	}
	struct Args args_consumer[NUM_CONSUMERS];
	for (int i=0; i<NUM_CONSUMERS; ++i) {
		args_consumer[i].thread_id = i + NUM_PRODUCERS;
		args_consumer[i].rb = &rb;
	}
	// Create threads
	pthread_t producers[NUM_PRODUCERS];
	for (int i=0; i<NUM_PRODUCERS; ++i)
		if (pthread_create(&producers[i], NULL, producer, &args_producer[i]) != 0) {
			fprintf(stderr, "Creating thread failed.\n");
			return EXIT_FAILURE;
		}
	pthread_t consumers[NUM_CONSUMERS];
	for (int i=0; i<NUM_CONSUMERS; ++i)
		if (pthread_create(&consumers[i], NULL, consumer, &args_consumer[i]) != 0) {
			fprintf(stderr, "Creating thread failed.\n");
			return EXIT_FAILURE;
		}

	void *res_producer[NUM_PRODUCERS];
	void *res_consumer[NUM_CONSUMERS];
	// Join the threads
	for (int i=0; i<NUM_PRODUCERS; ++i)
		if (pthread_join(producers[i], &res_producer[i]) != 0) {
			fprintf(stderr, "Joining thread failed.\n");
			return EXIT_FAILURE;
		}
	for (int i=0; i<NUM_CONSUMERS; ++i)
		if (pthread_join(consumers[i], &res_consumer[i]) != 0) {
			fprintf(stderr, "Joining thread failed.\n");
			return EXIT_FAILURE;
		}
	for (int i=0; i<NUM_PRODUCERS; ++i)
		if (res_producer[i] != NULL) {
			fprintf(stderr, "Error returned from thread.\n");
			return EXIT_FAILURE;
		}
	for (int i=0; i<NUM_CONSUMERS; ++i)
		if (res_consumer[i] != NULL) {
			fprintf(stderr, "Error returned from thread.\n");
			return EXIT_FAILURE;
		}

	RingBuffer_destroy(&rb);

	return EXIT_SUCCESS;
}
