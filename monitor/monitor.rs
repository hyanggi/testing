// This program demonstrates the use of a "monitor" (a mutex + condition varialbles) in the classic producer-consumer problem.

struct RingBuffer<T> {
	buffer: Box<[Option<T>]>,
	read_p: usize,
	write_p: usize,
}

struct Monitor<T> {
	mutex: std::sync::Mutex<RingBuffer<T>>,
	data_avail: std::sync::Condvar,
	space_avail: std::sync::Condvar,
}

impl <T> Monitor<T> {
	fn new(size: usize) -> Monitor<T> {
		let mut buffer = Vec::with_capacity(size + 1);
		for _ in 0..size+1 {
			buffer.push(None)
		}
		let rb = RingBuffer {
			buffer: buffer.into_boxed_slice(),
			read_p: 0,
			write_p: 0,
		};
		Monitor {
			mutex: std::sync::Mutex::new(rb),
			data_avail: std::sync::Condvar::new(),
			space_avail: std::sync::Condvar::new(),
		}
	}

	fn put(&self, data_unit: T) {
		{
			let mut guard = self.mutex.lock().unwrap();

			while {
				let rb = &*guard;
				(rb.write_p + 1) % rb.buffer.len() == rb.read_p
			} {
				guard = self.space_avail.wait(guard).unwrap();
			}
			let rb = &mut *guard;

			rb.buffer[rb.write_p] = Some(data_unit);
			rb.write_p = (rb.write_p + 1) % rb.buffer.len();
		}
		self.data_avail.notify_one();
	}

	fn get(&self) -> T {
		let data_unit = {
			let mut guard = self.mutex.lock().unwrap();

			while {
				let rb = &*guard;
				rb.read_p == rb.write_p
			} {
				guard = self.data_avail.wait(guard).unwrap();
			}
			let rb = &mut *guard;

			let data_unit = rb.buffer[rb.read_p].take().unwrap();
			rb.read_p = (rb.read_p + 1) % rb.buffer.len();

			data_unit
		};
		self.space_avail.notify_one();
		data_unit
	}
}

struct DataUnit {
	source_thread_id: usize,
	data: char,
}

fn producer(monitor: &Monitor<Box<DataUnit>>, thread_id: usize) {
	for i in 0..10 {
		let data_unit = DataUnit {
			source_thread_id: thread_id,
			data: std::char::from_u32('a' as u32 + i).unwrap(),
		};
		monitor.put(Box::new(data_unit));
	}
}

fn consumer(monitor: &Monitor<Box<DataUnit>>, thread_id: usize) {
	for _ in 0..8 {
		let data_unit = monitor.get();
		println!("Thread {} got {} from thread {}", thread_id, data_unit.data, data_unit.source_thread_id);
	}
}

const NUM_PRODUCERS: usize = 4;
const NUM_CONSUMERS: usize = 5;

fn main() {
	let monitor = Monitor::<Box<DataUnit>>::new(3);
	let arc = std::sync::Arc::new(monitor);

	let mut producers = Vec::with_capacity(NUM_PRODUCERS);
	for i in 0..NUM_PRODUCERS {
		let arc = std::sync::Arc::clone(&arc);
		let handle = std::thread::spawn( move || { producer(&arc, i) } );
		producers.push(handle);
	}
	let mut consumers = Vec::with_capacity(NUM_CONSUMERS);
	for i in 0..NUM_CONSUMERS {
		let arc = std::sync::Arc::clone(&arc);
		let handle = std::thread::spawn( move || { consumer(&arc, i + NUM_PRODUCERS) } );
		consumers.push(handle);
	}

	for handle in producers {
		handle.join().unwrap();
	}
	for handle in consumers {
		handle.join().unwrap();
	}
}
