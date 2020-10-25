# lockfree

The `.c` and `.cpp` files here try to implement a ring buffer that is safe to use when there is a single producer thread and a single consumer thread, without the use of locks, in the C and C++ languages respectively.

## C/C++ multithreading memory model

It is important to understand the multithreading memory model of C/C++ when doing lockfree programming. Without care, modifications of shared variables in one thread can appear completely different to the other thread. The order of changes may be completely different, and there may be intermediate values missing or inserted. This is because of compiler optimization, CPU optimization, CPU caches, and non-atomic accesses. Therefore the C/C++ language standards have developed a conceptual model to restrict what can appear to threads reading shared variables.

Within one thread, the order of evaluation determines a "*sequenced before*" relation between operations.

Between threads, we can establish "*synchronizes with*" and "*dependency-ordered before*" relations through certain library calls.

Combining these relations, we have a "*happens before*" relation established for all the operations in the process. The "*happens before*" relation is a partial order. We can see it as a directed acyclic graph organizing all the operations.

For a variable, for any two operations on it, if at least one of them is non-atomic, and if at least one of them is a write (they "*conflict*"), the two operations must have a "*happens before*" relationship between them, one way or the other, otherwise they result in a "*data race*", which is undefined behavior. When there is no "*data race*":

For a non-atomic variable, a read operation sees the result of the nearest write operation that "*happens before*" it.

For an atomic variable, it is as if all read/write operations on it are serialized based on their "*happens before*" relationships.

Therefore, the most important thing in lockfree programming is establishing the "*happens before*" relation, and ensuring there is no write operation uncomparable to another operation on the same variable in terms of the "*happens before*" relation, unless both operations are atomic.
