# Background worker and SPSC queue

`lh/os/worker.h` provides a bounded serial worker. Create with an outstanding-job
capacity, submit work, then take completed records on the caller thread. Invoke
the optional completion there and dispose each taken record exactly once.
Capacity includes completed records until taken. Stop joins active work and
rejects further submissions. Destroy cancels queued work and disposes all
remaining payloads on the caller thread without invoking completions. Rejected
submissions retain payload ownership. Keep callbacks and their code alive until
disposal finishes. Submit/take/lifecycle calls require caller coordination; work
callbacks must not join or destroy their own worker.

`lh/spsc.h` provides an allocation-free push and zero-copy peek/release after
creation. Supply storage for a power-of-two number of fixed-size records. Only
one producer may push and one consumer may peek/release concurrently. Storage
and returned record pointers are borrowed; do not overwrite them externally.
Full queues reject the newest item. Creation checks for lock-free atomic indices;
creation and destruction must not overlap queue operations. This primitive has
no logging, thread, wakeup, retention or flush policy.

Both objects capture the caller runtime allocator at creation. Its context must
outlive the object and any taken job records. Allocation failure is recoverable:
creation returns `lh_null` and submission rejects without taking payload ownership.
Changing the runtime allocator does not change how existing objects are released.
