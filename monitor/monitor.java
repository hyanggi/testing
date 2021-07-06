// This program demonstrates the use of a "monitor" (a mutex + condition varialbles) in the classic producer-consumer problem.

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;

public class monitor {
	private static class RingBuffer<T> {
		private final T[] buffer;
		private int readP;
		private int writeP;

		private final Lock lock = new ReentrantLock();
		private final Condition dataAvail = lock.newCondition();
		private final Condition spaceAvail = lock.newCondition();

		public RingBuffer(int size) {
			@SuppressWarnings("unchecked")
			T[] buffer = (T[]) new Object[size + 1];
			// We cannot create generic arrays in Java. So we create an Object[] and pretend it to be a T[].
			// The cast is OK because T is actually Object at run-time, due to type erasure of Java's generics.
			// We can only use annotations on declarations. So we use a temporary variable here.

			this.buffer = buffer;
		}

		public void put(T dataUnit) {
			lock.lock();
			try {
				while ((writeP + 1) % buffer.length == readP)
					spaceAvail.awaitUninterruptibly();

				buffer[writeP] = dataUnit;
				writeP = (writeP + 1) % buffer.length;

				dataAvail.signal(); // Java implementations usually require the lock is locked when signaling.
			}
			finally {
				lock.unlock();
			}
		}

		public T get() {
			lock.lock();
			try {
				while (readP == writeP)
					dataAvail.awaitUninterruptibly();

				T dataUnit = buffer[readP];
				buffer[readP] = null; // Tell the garbage collector this reference is no longer needed.
				readP = (readP + 1) % buffer.length;

				spaceAvail.signal(); // Java implementations usually require the lock is locked when signaling.

				return dataUnit;
			}
			finally {
				lock.unlock();
			}
		}
	}

	private static class DataUnit {
		public final int sourceThreadId;
		public final char data;
		public DataUnit(int sourceThreadId, char data) {
			this.sourceThreadId = sourceThreadId;
			this.data = data;
		}
	}

	private static class Producer implements Runnable {
		private final RingBuffer<DataUnit> rb;
		private final int threadId;
		public Producer(RingBuffer<DataUnit> rb, int threadId) {
			this.rb = rb;
			this.threadId = threadId;
		}
		@Override
		public void run() {
			for (int i=0; i<10; ++i) {
				DataUnit dataUnit = new DataUnit(threadId, (char)((int)'a' + i));
				rb.put(dataUnit);
			}
		}
	}

	private static class Consumer implements Runnable {
		private final RingBuffer<DataUnit> rb;
		private final int threadId;
		public Consumer(RingBuffer<DataUnit> rb, int threadId) {
			this.rb = rb;
			this.threadId = threadId;
		}
		@Override
		public void run() {
			for (int i=0; i<8; ++i) {
				DataUnit dataUnit = rb.get();
				String output = String.format("Thread %d got %c from thread %d", threadId, dataUnit.data, dataUnit.sourceThreadId);
				System.out.println(output);
			}
		}
	}

	private static final int NUM_PRODUCERS = 4;
	private static final int NUM_CONSUMERS = 5;

	public static void main(String[] args) {
		RingBuffer<DataUnit> rb = new RingBuffer<DataUnit>(3);

		Thread[] producers = new Thread[NUM_PRODUCERS];
		for (int i=0; i<NUM_PRODUCERS; ++i) {
			producers[i] = new Thread(new Producer(rb, i));
			producers[i].start();
		}
		Thread[] consumers = new Thread[NUM_CONSUMERS];
		for (int i=0; i<NUM_CONSUMERS; ++i) {
			consumers[i] = new Thread(new Consumer(rb, i + NUM_PRODUCERS));
			consumers[i].start();
		}

		// There is no need to join the threads.
	}
}
