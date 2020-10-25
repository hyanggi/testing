// This program implements a lock-free one-producer-one-consumer ring buffer.

#include <cstdio>
#include <thread>
#include <atomic>

#if ATOMIC_INT_LOCK_FREE != 2
#error The atomic type used is not always lock-free.
#endif

template <typename T>
class RingBuffer
{
public:
	RingBuffer(unsigned buffer_size);
	~RingBuffer();
	void put(T data_unit);
	T get();

private:
	T *buffer;
	const unsigned size;
	std::atomic_uint read_p = 0;
	std::atomic_uint write_p = 0;
};

template <typename T>
RingBuffer<T>::RingBuffer(unsigned buffer_size): size(buffer_size + 1)
{
	buffer = new T[buffer_size + 1];
}

template <typename T>
RingBuffer<T>::~RingBuffer()
{
	unsigned read = read_p.load(std::memory_order_relaxed);
	unsigned write = write_p.load(std::memory_order_relaxed);
	if (read != write) // if not empty
		std::fprintf(stderr, "Warning: the buffer is not empty when destruction.\n");
	delete[] buffer;
}

template <typename T>
void RingBuffer<T>::put(T data_unit)
{
	unsigned write = write_p.load(std::memory_order_relaxed);

	while (true) {
		unsigned read = read_p.load(std::memory_order_acquire);

		if ((write + 1) % size == read) // if appears full
			std::this_thread::yield();
		else
			break;
	}

	buffer[write] = data_unit;
	write = (write + 1) % size;

	write_p.store(write, std::memory_order_release);
}

template <typename T>
T RingBuffer<T>::get()
{
	unsigned read = read_p.load(std::memory_order_relaxed);

	while (true) {
		unsigned write = write_p.load(std::memory_order_acquire);

		if (read == write) // if appears empty
			std::this_thread::yield();
		else
			break;
	}

	T data_unit = buffer[read];
	read = (read + 1) % size;

	read_p.store(read, std::memory_order_release);

	return data_unit;
}

// The "release" in the producer thread "synchronizes with" the "acquire" in the consumer thread.
// So a chain of "happens before" relationships is established as:
// read_p.load -> buffer write -> write_p.store ---> write_p.load -> buffer read -> read_p.store

// The "release" in the consumer thread "synchronizes with" the "acquire" in the producer thread.
// So a chain of "happens before" relationships is established as:
// write_p.load -> buffer read -> read_p.store ---> read_p.load -> buffer write -> write_p.store

struct DataUnit
{
	int data;
};

void producer(RingBuffer<DataUnit*> *rb)
{
	for (int i=0; i<1000000; ++i) {
		DataUnit *data_unit = new DataUnit{.data = i};
		rb->put(data_unit);
	}
}

void consumer(RingBuffer<DataUnit*> *rb)
{
	for (int i=0; i<1000000; ++i) {
		DataUnit *data_unit = rb->get();
		if (data_unit->data != i)
			std::fprintf(stderr, "Error occurred\n");
		delete data_unit;
	}
}

int main()
{
	RingBuffer<DataUnit*> rb(3);

	// Create threads
	std::thread the_producer(producer, &rb);
	std::thread the_consumer(consumer, &rb);

	// Join the threads
	the_producer.join();
	the_consumer.join();

	return EXIT_SUCCESS;
}
