// This program demonstrates the use of a "monitor" (a mutex + condition varialbles) in the classic producer-consumer problem.

import std.stdio;
import std.traits;
import core.thread;
import core.sync.mutex;
import core.sync.condition;

struct RingBuffer(T)
{
    this(uint size)
    {
        buffer = new T[size + 1];
        mutex = new Mutex;
        dataAvail = new Condition(mutex);
        spaceAvail = new Condition(mutex);
    }

    void put(T dataUnit) // The purpose of this function is to convert rvalues to lvalues and call the following function.
    {
        put(dataUnit);
    }

    void put(ref T dataUnit) // Only lvalues can be passed to (or be returned from) functions by reference.
    {
        {
            mutex.lock();
            scope(exit) mutex.unlock();

            while ((writeP + 1) % buffer.length == readP) // while full
                spaceAvail.wait();

            buffer[writeP] = dataUnit;
            writeP = (writeP + 1) % buffer.length;
        }
        dataAvail.notify();
    }

    T get()
    {
        T dataUnit = delegate T()
        {
            mutex.lock();
            scope(exit) mutex.unlock();

            while (readP == writeP) // while empty
                dataAvail.wait();

            T dataUnit = buffer[readP];
            static if (isImplicitlyConvertible!(typeof(null), T)) // Check if T is a reference type which has the value "null".
                buffer[readP] = null; // Tell the garbage collector this reference is no longer needed, in case what it refers to is managed by the garbage collector.
            readP = (readP + 1) % buffer.length;

            return dataUnit;
        }();
        spaceAvail.notify();
        return dataUnit;
    }

private:
    T[] buffer;
    uint readP = 0;
    uint writeP = 0;
    Mutex mutex;
    Condition dataAvail;
    Condition spaceAvail;
}

struct DataUnit
{
    int sourceThreadID;
    char data;
}

class Producer: Thread
{
    this(RingBuffer!(DataUnit*)* rb, int threadID)
    {
        super(&run);
        this.rb = rb;
        this.threadID = threadID;
    }
private:
    void run()
    {
        for (int i=0; i<10; ++i)
        {
            DataUnit* dataUnit = new DataUnit(threadID, cast(char)('a' + i));
            rb.put(dataUnit);
        }
    }
    RingBuffer!(DataUnit*)* rb;
    immutable(int) threadID;
}

class Consumer: Thread
{
    this(RingBuffer!(DataUnit*)* rb, int threadID)
    {
        super(&run);
        this.rb = rb;
        this.threadID = threadID;
    }
private:
    void run()
    {
        for (int i=0; i<8; ++i)
        {
            DataUnit* dataUnit = rb.get();
            writefln("Thread %d got %c from thread %d", threadID, dataUnit.data, dataUnit.sourceThreadID);
        }
    }
    RingBuffer!(DataUnit*)* rb;
    immutable(int) threadID;
}

immutable(int) numProducers = 4;
immutable(int) numConsumers = 5;

void main()
{
    auto rb = RingBuffer!(DataUnit*)(3);

    // Create threads
    Thread[numProducers] producers;
    for (int i=0; i<numProducers; ++i)
    {
        producers[i] = new Producer(&rb, i);
        producers[i].start();
    }
    Thread[numConsumers] consumers;
    for (int i=0; i<numConsumers; ++i)
    {
        consumers[i] = new Consumer(&rb, i + numProducers);
        consumers[i].start();
    }

    // Join the threads
    foreach (Thread theProducer; producers)
        theProducer.join();
    foreach (Thread theConsumer; consumers)
        theConsumer.join();
}
