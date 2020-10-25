#!/usr/bin/env python3

# This program demonstrates the use of "monitor" (mutex + condition varialbles) in the classic producer-consumer problem.

import threading

class RingBuffer:
	def __init__(self, size):
		self.buffer = [None for i in range(size + 1)]
		self.read_p = 0
		self.write_p = 0
		self.lock = threading.Lock()
		self.data_avail = threading.Condition(self.lock)
		self.space_avail = threading.Condition(self.lock)

	def put(self, data_unit):
		with self.lock:
			while (self.write_p + 1) % len(self.buffer) == self.read_p: # while full
				self.space_avail.wait()

			self.buffer[self.write_p] = data_unit
			self.write_p = (self.write_p + 1) % len(self.buffer)

			self.data_avail.notify() # Python requires the lock is locked when notifying

	def get(self):
		with self.lock:
			while self.read_p == self.write_p: # while empty
				self.data_avail.wait()

			data_unit = self.buffer[self.read_p]
			self.buffer[self.read_p] = None # Decrease the reference count of this object for timely destruction.
			self.read_p = (self.read_p + 1) % len(self.buffer)

			self.space_avail.notify() # Python requires the lock is locked when notifying

			return data_unit

class DataUnit:
	def __init__(self, source_thread_id, data):
		self.source_thread_id = source_thread_id
		self.data = data

def producer(rb, thread_id):
	for i in range(10):
		data_unit = DataUnit(thread_id, chr(ord('a') + i))
		rb.put(data_unit)

def consumer(rb, thread_id):
	for i in range(8):
		data_unit = rb.get()
		print(f"Thread {thread_id} got {data_unit.data} from thread {data_unit.source_thread_id}")

num_producers = 4
num_consumers = 5

rb = RingBuffer(3)

# Create threads
producers = []
for i in range(num_producers):
	producers.append(threading.Thread(target = producer, args = (rb, i)))
	producers[-1].start()
consumers = []
for i in range(num_consumers):
	consumers.append(threading.Thread(target = consumer, args = (rb, i + num_producers)))
	consumers[-1].start()
