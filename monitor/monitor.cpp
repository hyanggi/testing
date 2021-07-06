// This program demonstrates the use of a "monitor" (a mutex + condition varialbles) in the classic producer-consumer problem.

#include <cstdio>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class RingBuffer
{
public:
	RingBuffer(unsigned buffer_size);
	~RingBuffer();
	void put(const T &data_unit);
	void put(T &&data_unit);
	T get();

private:
	const std::unique_ptr<T[]> buffer;
	const unsigned size;
	unsigned read_p = 0;
	unsigned write_p = 0;

	// Monitor (mutex + condition variables)
	std::mutex mutex;
	std::condition_variable data_avail;
	std::condition_variable space_avail;
};

template <typename T>
RingBuffer<T>::RingBuffer(unsigned buffer_size): buffer(std::make_unique<T[]>(buffer_size + 1)), size(buffer_size + 1)
{
}

template <typename T>
RingBuffer<T>::~RingBuffer()
{
}

template <typename T>
void RingBuffer<T>::put(const T &data_unit)
{
	put(T(data_unit));
}

template <typename T>
void RingBuffer<T>::put(T &&data_unit)
{
	std::unique_lock<std::mutex> lock(mutex); // Acquire the mutex

	while ((write_p + 1) % size == read_p) // while full
		space_avail.wait(lock); // Wait on a condition variable

	buffer[write_p] = std::move(data_unit);
	write_p = (write_p + 1) % size;

	lock.unlock(); // Release the mutex

	data_avail.notify_one(); // Notify another thread
}

template <typename T>
T RingBuffer<T>::get()
{
	std::unique_lock<std::mutex> lock(mutex); // Acquire the mutex

	while (read_p == write_p) // while empty
		data_avail.wait(lock); // Wait on a condition variable

	T data_unit = std::move(buffer[read_p]);
	read_p = (read_p + 1) % size;

	lock.unlock(); // Release the mutex

	space_avail.notify_one(); // Notify another thread

	return data_unit;
}

struct DataUnit
{
	int source_thread_id;
	char data;
};

void producer(RingBuffer<std::unique_ptr<DataUnit>> *rb, int thread_id)
{
	for (int i=0; i<10; ++i) {
		std::unique_ptr<DataUnit> data_unit(new DataUnit{.source_thread_id = thread_id, .data = char('a' + i)});
		rb->put(std::move(data_unit));
	}
}

void consumer(RingBuffer<std::unique_ptr<DataUnit>> *rb, int thread_id)
{
	for (int i=0; i<8; ++i) {
		std::unique_ptr<DataUnit> data_unit = rb->get();
		std::printf("Thread %d got %c from thread %d\n", thread_id, data_unit->data, data_unit->source_thread_id);
	}
}

constexpr int num_producers = 4;
constexpr int num_consumers = 5;

int main()
{
	RingBuffer<std::unique_ptr<DataUnit>> rb(3);

	// Create threads
	std::thread producers[num_producers];
	for (int i=0; i<num_producers; ++i)
		producers[i] = std::thread(producer, &rb, i);
	std::thread consumers[num_consumers];
	for (int i=0; i<num_consumers; ++i)
		consumers[i] = std::thread(consumer, &rb, i + num_producers);

	// Join the threads
	for (std::thread &the_producer: producers)
		the_producer.join();
	for (std::thread &the_consumer: consumers)
		the_consumer.join();

	return EXIT_SUCCESS;
}
