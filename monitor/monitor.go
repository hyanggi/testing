// This program demonstrates the use of "monitor" (mutex + condition varialbles) in the classic producer-consumer problem.

package main

import (
	"fmt"
	"sync"
)

type RingBuffer struct {
	buffer     []interface{}
	readP      int
	writeP     int
	mutex      sync.Mutex
	dataAvail  *sync.Cond
	spaceAvail *sync.Cond
}

func NewRingBuffer(size int) *RingBuffer {
	rb := RingBuffer{buffer: make([]interface{}, size+1)}
	rb.dataAvail = sync.NewCond(&rb.mutex)
	rb.spaceAvail = sync.NewCond(&rb.mutex)
	return &rb
}

func (rb *RingBuffer) Put(dataUnit interface{}) {
	func() {
		rb.mutex.Lock()
		defer rb.mutex.Unlock()

		for (rb.writeP+1)%len(rb.buffer) == rb.readP { // while full
			rb.spaceAvail.Wait()
		}

		rb.buffer[rb.writeP] = dataUnit
		rb.writeP = (rb.writeP + 1) % len(rb.buffer)
	}()
	rb.dataAvail.Signal()
}

func (rb *RingBuffer) Get() interface{} {
	dataUnit := func() interface{} {
		rb.mutex.Lock()
		defer rb.mutex.Unlock()

		for rb.readP == rb.writeP { // while empty
			rb.dataAvail.Wait()
		}

		dataUnit := rb.buffer[rb.readP]
		rb.buffer[rb.readP] = nil // In case the value held is a pointer, tell the garbage collector it is no longer needed.
		rb.readP = (rb.readP + 1) % len(rb.buffer)

		return dataUnit
	}()
	rb.spaceAvail.Signal()
	return dataUnit
}

type theDataUnit struct {
	sourceThreadId int
	data           rune
}

func producer(rb *RingBuffer, threadId int, wg *sync.WaitGroup) {
	for i := 0; i < 10; i++ {
		dataUnit := &theDataUnit{threadId, 'a' + int32(i)}
		rb.Put(dataUnit)
	}
	wg.Done()
}

func consumer(rb *RingBuffer, threadId int, wg *sync.WaitGroup) {
	for i := 0; i < 8; i++ {
		dataUnit := rb.Get().(*theDataUnit)
		fmt.Printf("Thread %d got %c from thread %d\n", threadId, dataUnit.data, dataUnit.sourceThreadId)
	}
	wg.Done()
}

const numProducers = 4
const numConsumers = 5

func main() {
	rb := NewRingBuffer(3)

	var wg sync.WaitGroup
	wg.Add(numProducers)
	wg.Add(numConsumers)

	for i := 0; i < numProducers; i++ {
		go producer(rb, i, &wg)
	}
	for i := 0; i < numConsumers; i++ {
		go consumer(rb, i+numProducers, &wg)
	}

	wg.Wait()
}
