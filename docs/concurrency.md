# OS threads and synchronization

`lh/os/thread.h`, `lh/os/mutex.h`, `lh/os/condition.h` and `lh/os/tick.h`
provide OS primitives. Enable `LH_LIBRARY_OPTION_OS`; SQLite, UI and a game host
are not required. CMake selects `os/system/posix` or `os/system/win`.
These primitives need an OS backend; they do not claim freestanding MCU support.

## Lifetime and error reporting

Initialize each object once before use. A live object must not be copied or
reinitialized. Explicitly join threads and close synchronization objects after
their last use. There is no hidden thread pool, module registry or global owner.
Contract failures set `lh_os_last_error`; native failures set the separate,
thread-local `lh_os_system_last_error`. Successful calls do not clear those slots.

## Threads

```c
lh_os_thread_t thread;
lh_os_thread_init(&thread);
if (lh_os_thread_start(&thread, run, user, -1, 0))
{
    /* Arrange for run(user) to return before joining. */
    lh_os_thread_join(&thread);
}
```

`cpu=-1` inherits affinity. Explicit CPU affinity is supported on Linux and
Windows (the current Windows processor group); an unavailable request fails
before the callback runs. Other POSIX platforms reject explicit affinity.
`lh_os_thread_block_sigpipe` blocks SIGPIPE only in the new POSIX worker, leaving
the caller's mask unchanged. It has no effect on Windows. No user callback runs
when start returns false. On success it can run before start returns.

## Backend notes

The Windows backend uses CRT-aware thread startup and a startup gate: rejected
affinity requests let the worker exit normally without invoking user code.
It never forcibly terminates an already-created worker. Conditions resolve the
native condition-variable API at open (Vista or newer); missing APIs fail cleanly
without making other OS services depend on that runtime feature.

## Mutexes and conditions

Lock a mutex exactly once before condition wait. Wait releases it atomically and
returns with it reacquired. Always check a shared predicate in a loop; spurious
wakeups are permitted. Concurrent waiters use the same mutex. Signal wakes one;
broadcast wakes all current waiters. Destroy neither condition nor mutex until
all waiters have returned. Recursive locking is outside this interface's contract.

## Monotonic time and sleep

`lh_os_tick_us` and `lh_os_tick_ms64` use the same monotonic origin. The original
32-bit `lh_os_tick_ms` wraps and remains available for existing timers. Convenience
clock calls return zero on native failure. `lh_os_system_tick_us(out)` is the
checked form and preserves output on failure. Windows reads have no mutable
frequency cache and divide the uptime before scaling. `lh_os_thread_sleep(ms)`
retries interrupted POSIX sleeps; it does not schedule game callbacks.

## Tests

Standalone check (no GoogleTest required): build `lh_test_os_concurrency` and run
`ctest -R lh_os_concurrency --output-on-failure`. Tests cover mutex contention,
signal/broadcast with predicate loops, affinity, failed starts without callbacks,
self-join rejection, error isolation, monotonic time and sleep. Authum separately
tests module ownership, wake-all-before-join, cancellation and frame delivery.
