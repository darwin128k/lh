# SPSC queue

`lh/core/spsc.h` provides an allocation-free push and zero-copy peek/release after
creation. Supply storage for a power-of-two number of fixed-size records. Only
one producer may push and one consumer may peek/release concurrently. Storage
and returned record pointers are borrowed; do not overwrite them externally.
Full queues reject the newest item. Creation checks for lock-free atomic indices;
creation and destruction must not overlap queue operations. This primitive has
no logging, thread, wakeup, retention or flush policy.

The queue captures the caller runtime allocator at creation. Its context must
outlive the queue. Allocation failure is recoverable: creation returns `lh_null`.
Changing the runtime allocator does not change how an existing queue is released.

Background work (a thread that runs tasks, results delivered on the owner's
thread) is the execution part, `lh/rail/`: see
[docs/design/execution.md](../design/execution.md). It replaced the removed
`lh/os/worker.h`.
