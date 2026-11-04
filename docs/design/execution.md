# Execution in LH — concept (draft for discussion)

Status: decisions taken (section 5), API draft for review (section 6),
nothing implemented. This page decides what the execution engine is before
any code moves; the code follows the page, not the other way.

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
| `lh_os_loop_t` | the loop, with its waiter as the source of world readiness (`lh_exec_loop_t` since step 5) |
| `lh_os_worker_t` and its job | the *thread* executor and the task (worker removed in step 4b) |
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

## 5. Decisions (owner, 2026-11-04)

1. **Name: `exec`.** Headers in `include/lh/exec/`, sources in `src/lh/exec/`,
   names `lh_exec_*` (the part carries its name, AGENTS.md section 5).
2. **Any thread may submit.** The submission queue is multi-producer,
   single-consumer: a task may submit a follow-up, two subsystems may share one
   executor.
3. **Bounded.** Every owner's mailbox has a capacity; a submission that would
   exceed it is refused at once (`try_submit` is false), never queued.
4. **Pool later.** One thread per executor keeps tasks in order; the pool comes
   when something needs it.
5. **Beside `lh/os` first.** `lh/exec` is built next to `lh/os` and uses its
   thread, mutex and condition for now; `lh/os` users move onto `lh/exec` one by
   one, and the primitives move out of `lh/os` last.

## 6. API draft

Every entity has its own header, `fields.h`, getters and setters, as AGENTS.md
asks. Only the shape is shown here.

### Task — `lh/exec/task.h`

```c
typedef lh_void(lh_exec_task_fn)(lh_ptr user, lh_ptr local);  /* work: local = thread context state */
typedef lh_void(lh_exec_done_fn)(lh_ptr user);                /* complete and destroy */

struct lh_exec_task {
    lh_exec_task_fields(lh_exec_task_fn, lh_exec_done_fn, lh_ptr, lh_atomic_ptr_t,
                        struct lh_exec_mailbox *);
    /* work_cb, complete_cb, destroy_cb, user, next (queue link), mailbox */
};

lh_void lh_exec_task_init(lh_exec_task_t *self, lh_exec_task_cb work, lh_exec_done_cb complete,
                          lh_exec_done_cb destroy, lh_ptr user);
```

- The task record belongs to the caller: embedded in the caller's own object,
  or allocated by the caller. `lh/exec` allocates nothing per task. `destroy`
  is where the caller frees it.
- `next` is the intrusive link of the queues below, `mailbox` the owner that
  gets `complete`; both are set by the executor, not by the caller.

### Queue — `lh/exec/queue.h`

A multi-producer, single-consumer intrusive queue of tasks: `lh_exec_queue_push`
from any thread (lock-free, one `exchange` on the tail), `lh_exec_queue_try_pop`
on the one consumer thread. Needs one new brick:
`lh_atomic_ptr_exchange_acq_rel`.

### Mailbox — `lh/exec/mailbox.h`

```c
lh_void   lh_exec_mailbox_init(lh_exec_mailbox_t *self, lh_uint_t capacity);
lh_bool_t lh_exec_mailbox_try_reserve(lh_exec_mailbox_t *self);   /* any thread, at submit */
lh_void   lh_exec_mailbox_release(lh_exec_mailbox_t *self);       /* a place never posted */
lh_void   lh_exec_mailbox_post(lh_exec_mailbox_t *self, lh_exec_task_t *task); /* after work */
lh_exec_task_t *lh_exec_mailbox_try_take(lh_exec_mailbox_t *self);           /* owner thread */
lh_void   lh_exec_mailbox_deliver(lh_exec_mailbox_t *self, lh_exec_task_t *task);
lh_uint_t lh_exec_mailbox_dispatch(lh_exec_mailbox_t *self);       /* take + deliver, owner */
```

- The capacity counts tasks accepted for this owner and not yet dispatched —
  queued, running and finished. A full mailbox refuses the submission. The
  count is exact under contention (`lh_atomic_uint_try_compare_exchange_acq_rel`).
- `init`, not `try_init`: the finished tasks are linked through themselves (an
  `lh_exec_queue_t` inside the record), so nothing is allocated and nothing can
  fail. The mailbox must not move after `init`.
- `dispatch` runs `complete` then `destroy` for every finished task, on the
  owner's thread, and frees their places. It delivers at most as many tasks as
  there were places taken when it began, so a `complete` that submits again
  cannot keep one dispatch running forever.

### Executor — `lh/exec/executor.h`

```c
struct lh_exec_executor {
    lh_exec_executor_fields(lh_exec_executor_accept_fn, lh_ptr);  /* accept_cb, context */
};

lh_bool_t lh_exec_executor_try_accept(const lh_exec_executor_t *self, lh_exec_task_t *task);
lh_bool_t lh_exec_executor_try_submit(const lh_exec_executor_t *self, lh_exec_task_t *task,
                                      lh_exec_mailbox_t *mailbox);
lh_void   lh_exec_executor_submit(...);  /* the try_ form + lh_runtime_check */
```

- One interface for every kind. `try_submit` reserves a place in `mailbox`,
  records the mailbox in the task, then hands the task over (`try_accept`, the
  kind's own call); when the kind refuses, the place goes back. False means
  nothing was accepted and the task is still the caller's.
- The kind's call is `accept`, not `submit`: it only takes a task that already
  holds its place. The mailbox part is the same for every kind, so it lives once,
  in `try_submit`.
- A refused checked `submit` fails with `lh_runtime_error_code_not_accepted`.

### Inline executor — `lh/exec/inline.h`

Runs `work` at once on the submitting thread (with `local` = null), then posts
the task: `complete` still comes only at `dispatch`, so code behaves the same on
the synchronous and the threaded executor. Tests use it to run logic without
threads or timing. It has no state, so one shared record serves every caller:
`lh_exec_inline_get_executor()`.

### Thread executor and its context — `lh/exec/thread.h`, `lh/exec/context.h`

```c
struct lh_exec_context {   /* state that lives as long as the executor's thread */
    lh_exec_context_fields(lh_exec_context_open_fn, lh_exec_context_close_fn, lh_ptr);
};

lh_bool_t lh_exec_thread_try_start(lh_exec_thread_t *self, const lh_exec_context_t *context);
const lh_exec_executor_t *lh_exec_thread_get_executor(const lh_exec_thread_t *self);
lh_bool_t lh_exec_thread_try_stop(lh_exec_thread_t *self, lh_exec_stop_kind_t kind,
                                  lh_u32_t timeout_ms);
lh_void   lh_exec_thread_destroy(lh_exec_thread_t *self);
```

- One thread, tasks in submission order. `open` runs on the thread before its
  first task and returns the `local` state every `work` receives; `close` runs
  on the same thread after the last one (a database connection, `mysql_thread_end`).
- The record is the caller's (`try_start` fills it, as `lh/os` concurrency takes
  caller storage), not allocated by `try_create`: an Authum owner or an SQL
  driver embeds it. It must not move until `destroy`.
- `lh_exec_stop_kind_t` (`lh/exec/stop/kind.h`): `drain` runs every accepted
  task, `cancel` marks the not-started ones cancelled; the running task always
  finishes. Every accepted task still goes to its mailbox, and the owner's
  dispatch runs `destroy` (and, unless cancelled, `complete` first) — so
  `destroy` is on the owner's thread in both kinds. A stop never waits for the
  owner's dispatch: the owner is usually the thread that stops.
- `try_stop` waits at most `timeout_ms` and says whether the thread ended; it
  may be called again, and a drain may become a cancel. `destroy` waits without
  a bound, then joins. A thread stuck in a call that frees itself later (what
  the HTTP server does today by hand) comes with step 6, which needs it.
- Submission takes the executor's mutex for a moment (link the task, wake the
  thread). The loop's own wake takes a mutex as well, so moving the executor
  onto the loop gains nothing there; it moves when it needs the loop's waiter
  (step 6).

### The loop — `lh/exec/loop.h`

`lh_exec_loop_t` (step 5) is the former `lh_os_loop_t`, moved and renamed, its
behaviour unchanged: a thread that runs a step, fires its due timers and sleeps
until a wake (never lost), a hint (lock-free, may be missed for one period), the
nearest timer, the end of its period, or a stop. A loop made with a waiter
(`lh_exec_loop_try_create_waiting`) sleeps in the waiter's `wait` — a `poll` of
sockets and a wake signal — which is how the world plugs in as sources. The
logger queue (`lh_os_logger_queue_t`) and Authum's HTTP server run on it.

The thread executor is not built on the loop yet: nothing it does today needs
timers or a waiter. Step 6 (the HTTP server waiting through the loop) is the
first need, and the executor moves then.

## 7. Order of the move

Each step is its own change, verified on the GCC option matrix, Linux and MSVC,
and committed by the owner before the next.

1. This page, agreed.
2. `lh_atomic_ptr_exchange_acq_rel`; task and queue, with tests.
3. Mailbox, executor interface, inline executor.
4. Thread executor, context, stop (4a); Authum's tasks move onto it and
   `lh_os_worker_t`, left without users, is removed (4b, owner, 2026-11-04).
5. The loop in `lh/exec`: `lh_os_loop_t` becomes `lh_exec_loop_t`; the logger
   queue and Authum's HTTP server use it under the new name. The thread
   executor moves onto the loop with step 6, its first need.
6. The HTTP server (with the fastdl hardening) and the web host wait through
   the loop.
7. Asynchronous SQL as a composition.
8. The thread, mutex and condition move out of `lh/os` into `lh/exec`'s base.
