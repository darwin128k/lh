# Execution in LH — concept (draft for discussion)

Status: draft, nothing implemented. This page decides what the execution
engine is before any code moves; the code follows the page, not the other way.

## 1. Three natures, mixed today

LH started as a box of bricks: call a function, get a result. It grew windows,
a web host, threads and queues. Those are not more bricks of the same kind.
The tree holds three different natures:

| Nature | Question | Members | Properties |
|---|---|---|---|
| **Data** | *what* | types, `array`, `str`, memory kernels, atomics, json, xml, compression, crypto, timers as data (`lh/timer`) | no time, no waiting, no threads; a call returns its result; deterministic |
| **Execution** | *when and where* | threads, waiting, wake-ups, timers that fire, queues of work, "do this later", "tell me when it is done" | time and parallelism; ordering, cancellation and shutdown live here |
| **World** | *with what* | files, sockets, windows, input, the module loader | operating-system resources; their operations wait — and should wait through execution |

Execution has no home of its own. It is spread over:

| Where | What it does today |
|---|---|
| `lh/os/loop.h` | a thread that sleeps until a wake, a timer, its period or a stop, then runs one step; can sleep in a *waiter* (`poll` of sockets plus a `lh/os/wake.h` signal) — no code uses that yet |
| `lh/os/worker.h` | a serial job queue on a loop: `work` on the thread, `complete` taken by one owner thread |
| `lh/os/logger/queue.h` | its own record queue (`lh_spsc_t`) on a loop |
| `lh/os/thread/group.h` | threads that belong to one owner and stop together |
| `lh/ui/web/host.c` | its own waiting: each request read in a blocking loop with its own timeouts over `lh_os_net_*` and `lh_os_tick_*` |
| Authum `host/tasks.c`, `api/sql/thread.c` | their own task type over `lh_os_worker_t`; a new database connection per query |

Each one invents "what to do, what to call when done, how to clean up" and
"how to wait" again. That is the mixture: the mechanism exists, but it has no
place and no laws.

## 2. The vocabulary of execution

Execution needs six words, and each comes with laws.

**Task** — `work`, `complete`, `destroy`, `user`.
- `work` runs once, on the executor's thread (or on the caller's, for the
  inline executor).
- `complete` runs on the owner's thread, only when the owner dispatches.
- `destroy` runs exactly once for every accepted task: after `complete`, or
  instead of it when the task is cancelled.
- An accepted task belongs to the executor; a rejected one stays with the caller.

**Executor** — where a task runs; one operation, `submit`.
- *inline*: runs `work` at once on the caller's thread. Synchronous execution
  is this executor, not a second mechanism.
- *thread*: one thread, tasks in submission order (today's `lh_os_worker_t`).
- *pool*: N threads, no order between tasks.
- Code that submits does not know which one it got.
- A full executor rejects (`try_submit` is false); it never blocks the submitter.

**Loop** — where a thread waits.
- Sources: a wake (never lost), a hint (lock-free, may be missed), timers, a
  period, a stop, and the *waiter* — readiness of world resources (sockets).
- Only the loop's own thread touches its timers and sources.
- The world (sockets, files) plugs in as sources; it does not get its own loop.

**Mailbox** — where finished tasks wait for their owner.
- The owner drains it on its own thread (`dispatch`): a game's frame, a UI tick.
- Nothing is delivered behind the owner's back.

**Thread context** — state that lives as long as an executor's thread.
- `open` runs on the thread when it starts, `close` on the same thread when it
  stops. A database connection lives here: one per thread, opened once.

**Stop** — one rule for every executor.
- *drain*: every accepted task runs, its `complete` is delivered, then thread
  contexts close, then threads end.
- *cancel*: tasks not yet started get `destroy` only; the running one finishes.
- Stop never frees state another thread may still use.

## 3. Today's code in that vocabulary

| Today | Becomes |
|---|---|
| `lh_os_loop_t` | the loop, with its waiter as the source of world readiness |
| `lh_os_worker_t` and its job | the *thread* executor and the task |
| `lh_os_logger_queue_t` | a thread executor whose tasks are records, plus a sink |
| web host request reading | sockets as loop sources instead of blocking reads with timeouts |
| `lh_os_thread_group_t` | stays as is: the owner's set of threads, used by executors |
| Authum tasks | an executor and a mailbox drained in the game frame |
| asynchronous SQL | a thread executor with a thread context holding the connection |

## 4. Where execution lives

```
data (lh)                       ← depends on nothing
   ↑
execution                       ← uses data bricks and OS primitives
   ↑                              (thread, sleep/wake, wait for readiness)
world (os)                      ← resources; waits through execution
   ↑
compositions: web host, logger queue, SQL, Authum tasks
```

- Execution lives in the same repository and uses LH's bricks, rules and
  allocator, but it is its own part with its own name, not a folder among the
  bricks: an engine built from LH's bricks.
- Each part is its own CMake target, so the build enforces the direction: the
  data target cannot see execution headers, execution cannot see world headers.
- Consequence: the world (`lh/os` without its thread primitives) sits above
  execution, so it separates from the data part too. AGENTS.md 0.1.1 already
  asks for that ("the OS is assembled from parts").

## 5. Order of the move

Each step is its own change, verified on the GCC option matrix, Linux and MSVC,
and committed by the owner before the next.

1. This page, agreed.
2. Headers of task, executor, loop, mailbox, thread context — documentation only.
3. The OS primitives execution needs (thread, mutex, condition, sleep, wake)
   move out from under `lh/os` into the execution part's base.
4. Task, inline and thread executors, mailbox; `lh_os_worker_t` moves onto them.
5. The loop with sources; `lh_os_loop_t` and the logger queue move onto it.
6. The web host waits through the loop.
7. Asynchronous SQL as a composition.
8. Authum moves onto all of it.

## 6. Open questions for the owner

1. **Name** of the execution part and its prefix.
2. **Who may submit.** Today `lh_os_worker_t` allows one owner thread. A task
   that submits a follow-up, or two subsystems sharing an executor, need submit
   from any thread (a multi-producer queue). Allow it from the start?
3. **Mailbox bound.** Bounded (rejects when full, like the executor) or
   growing?
4. **Pool now or later.** A game server usually needs one database thread; a
   pool loses ordering. Build the pool only when something needs it?
5. **Does the world move now** (step 3 onwards), or does execution first live
   beside `lh/os` and the world follows later?
