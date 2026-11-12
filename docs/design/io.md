# Asynchronous I/O in LH — concept (draft for discussion)

Status: draft, nothing decided, nothing implemented. This page compares LH with
libuv (the library Node.js runs on), names what LH lacks to be the base of
servers and sites, and proposes the bricks and their order. The code follows the
page once the owner has decided section 5.

## 1. How libuv is organized

libuv is one event loop per thread and two kinds of objects on it.

```
uv_loop_t — one per thread; uv_run() turns these phases in a circle:
  timers → pending callbacks → idle / prepare → poll for I/O → check → close callbacks

handles: long-lived, belong to a loop          requests: one operation each
  uv_tcp_t, uv_udp_t, uv_pipe_t, uv_tty_t         uv_write_t, uv_connect_t, uv_shutdown_t
  uv_timer_t, uv_async_t (a wake from any thread) uv_fs_t (a file call, on the pool)
  uv_signal_t, uv_process_t, uv_fs_event_t        uv_getaddrinfo_t (DNS, on the pool)
  uv_poll_t (readiness of a foreign fd)           uv_work_t (the caller's own work, on the pool)
```

- **A stream** (`uv_stream_t`, the base of TCP, pipes and TTYs) is the central
  abstraction: `uv_listen` + `uv_accept`; `uv_read_start(alloc_cb, read_cb)`
  hands each arrival to the caller in a buffer the caller allocated;
  `uv_write(req, bufs, cb)` reports when the bytes left; `uv_shutdown`; and
  `uv_close(handle, close_cb)`, after which the handle's memory is the
  caller's again.
- **Every callback runs on the loop's thread.** The only thread-safe call is
  `uv_async_send` (a wake). Work that would block — every file call, DNS,
  the caller's own computation — runs on a thread pool (4 threads by default)
  and its callback comes back to the loop.
- **One model over two kinds of kernels.** Linux (epoll, io_uring for files)
  and BSD / macOS (kqueue) report *readiness*; Windows (IOCP) reports
  *completion*. libuv's public API is completion-shaped (a write says when it
  finished, a read hands over the bytes), so on Unix it performs the call when
  the socket is ready and on Windows it starts an overlapped call and waits for
  its completion. The caller never sees which.
- **Memory is the caller's.** A handle or a request is a struct the caller
  allocates and embeds; libuv allocates almost nothing on its own.
- Around the loop: threads, mutexes, condition variables, `uv_hrtime`,
  `uv_dlopen`, environment, paths, error names (`uv_strerror`, `uv_err_name`).

## 2. LH in libuv's terms

| libuv | LH today |
|---|---|
| `uv_loop_t`, timers in the loop | `lh_rail_loop_t` with its timer group (`lh/rail/loop.h`) |
| `uv_async_t` | `lh/os/wake.h`, and the loop's own `wake` / `hint` |
| `uv_work_t`, callback on the owner | `lh_rail_task_t` + executor + `lh_rail_mailbox_t`, stricter: bounded mailboxes, `drain` / `cancel` stop, exactly one `destroy` |
| the loop on a board without an OS | **not in libuv**; `lh/rail/bare.h` runs rail on a Cortex-M under `main` |
| waiting for sockets | a loop's *waiter*: `lh_os_net_socket_poll` = `poll()` on POSIX, `select()` on Windows (FD_SETSIZE, 64 sockets by default) |
| `uv_stream_t`, `uv_read_start`, `uv_write` | **none**: sockets are blocking, or polled and read by hand (the web host, Authum's HTTP server) |
| thread pool | **none**: one thread per executor ("pool later", execution.md) |
| `uv_fs_*` asynchronous | **none**: `lh_os_fs_file_*` is blocking |
| `uv_getaddrinfo` | **none**: LH has no DNS at all, and its socket addresses are IPv4 only |
| `uv_process_t`, `uv_signal_t`, `uv_fs_event_t`, `uv_tty_t`, `uv_pipe_t` | **none** |
| `uv_dlopen`, threads, mutexes, time, env | `lh/os/shared`, `lh/os/thread`, `lh/os/mutex`, `lh/os/tick`, `lh/os/env` |

LH already has the harder half — execution with laws (`execution.md`), and it
reaches places libuv does not (bare metal). What it lacks is the world side of
I/O: a scalable wait, a stream, a pool, and the operations built on them.

## 3. What to add, in order of need

1. **A poller with a backend per kernel** — `epoll` (Linux), `kqueue` (BSD,
   macOS), Windows through IOCP; `poll()` stays as the portable fallback. It is
   a waiter for `lh_rail_loop_try_create_waiting`, so the loop does not change.
   Without it a server is capped at 64 sockets on Windows and walks every
   socket on every wake elsewhere.
2. **A stream** — `lh_rail_stream_t`: read start / stop, write with
   completion, shutdown, close with completion, listen and accept. This is the
   brick a server or a site is written against. Its first backend is the OS
   socket; the second is a W5500 on a bare STM32 (its TCP/IP is in the chip,
   so the brick sees a socket that reports *completion*, as IOCP does). The
   same server code runs on a PC and on the board.
3. **A pool executor** — `lh_rail_pool_t`, N threads, no order between tasks,
   behind the existing `lh_rail_executor_t` interface. On a base without
   threads it refuses (`try_start` false), and callers fall back to the inline
   executor.
4. **Asynchronous files and DNS on the pool** — a request is a task whose
   `work` calls the blocking brick (`lh_os_fs_file_*`, a new blocking
   `lh/os/net/resolve`) and whose `complete` lands in the caller's mailbox.
   DNS brings IPv6 socket addresses with it (`lh/core/net/ip6`).
5. Later, each for a concrete need: processes with pipes, signals, file
   watching, TTY.

## 4. Where it lives

```
lh/core      data: buffers, addresses (ip4, ip6), parsers          — no OS, no waiting
lh/rail      execution: loop, task, executor, mailbox, pool, stream — through the rail base
lh/os        the world: sockets, files, poller backends, resolve    — native calls in lh/os/system
```

- The stream is execution (it lives on a loop, its callbacks run on the loop's
  thread), so it is `lh/rail/stream`. What it reads and writes is a backend
  table, like `lh_rail_base_t`: the OS socket fills one in `lh/os`, the W5500
  driver fills another for a board.
- The poller is the world: `lh/os/poller`, its kernels in
  `lh/os/system/{linux,bsd,win,posix}/poller.c`, one portable API above them
  (AGENTS.md 0.2).
- Each level keeps working without the ones above: `lh/os` sockets stay
  usable blocking, and a stream can be driven on a loop that is never started
  (`lh_rail_loop_pass` from `main`).

## 5. Decisions for the owner

1. **The model.** Completion-shaped like libuv (a write reports when it is
   done, a read hands bytes over), implemented as "ready, then call" on
   epoll / kqueue and as overlapped calls on IOCP. The alternative,
   readiness everywhere (the Windows side emulated through `AFD_POLL`, as
   `wepoll` does), is less code but cannot serve the W5500 or files.
   Proposed: completion.
2. **Who owns a callback's thread.** As libuv: every stream callback runs on
   its loop's thread; work for another owner (a game thread) goes through a
   task and its mailbox, as today. Proposed: yes.
3. **Buffers.** libuv asks the caller for a buffer before each read
   (`alloc_cb`). LH can take a fixed buffer per stream (simpler, enough for a
   board) or the `alloc_cb`. Proposed: a fixed buffer given at
   `read_start`, an `alloc_cb` form later if something needs it.
4. **Names.** `lh_rail_stream_*`, `lh_rail_pool_*`, `lh_os_poller_*`,
   `lh_os_net_resolve_*`.
5. **Scope of the first change.** Proposed: the poller only (step 2 of
   section 7), measured against `poll()` / `select()` with `bench/`.

## 6. API draft

Every entity gets its own header, `fields.h`, level-1 accessors and a test, as
AGENTS.md asks; only the shape is shown.

### Poller — `lh/os/poller.h`

```c
lh_bool_t lh_os_poller_try_open(lh_os_poller_t *self);
lh_void   lh_os_poller_close(lh_os_poller_t *self);
lh_bool_t lh_os_poller_try_add(lh_os_poller_t *self, const lh_os_net_socket_t *socket,
                               lh_uint_t events, lh_ptr user);
lh_bool_t lh_os_poller_try_modify(lh_os_poller_t *self, const lh_os_net_socket_t *socket,
                                  lh_uint_t events, lh_ptr user);
lh_void   lh_os_poller_remove(lh_os_poller_t *self, const lh_os_net_socket_t *socket);
lh_ssize_t lh_os_poller_wait(lh_os_poller_t *self, lh_os_poller_event_t *events,
                             lh_usize_t capacity, lh_u32_t timeout_ms);
lh_void   lh_os_poller_wake(lh_os_poller_t *self);   /* any thread; cuts a wait short */
```

- The poller owns its wake (`eventfd` / a pipe / `PostQueuedCompletionStatus`),
  so a loop made with a poller as its waiter needs no `lh_os_wake_t` beside it.
- `wait` returns as many events as are ready, up to `capacity`; the cost does
  not grow with the number of sockets registered.

### Stream — `lh/rail/stream.h`

```c
typedef lh_void(lh_rail_stream_read_fn)(lh_rail_stream_t *self, lh_ssize_t size);
typedef lh_void(lh_rail_stream_done_fn)(lh_rail_stream_t *self, lh_ssize_t result);

lh_bool_t lh_rail_stream_try_read_start(lh_rail_stream_t *self, lh_byte_t *buffer,
                                        lh_usize_t capacity, lh_rail_stream_read_cb read);
lh_void   lh_rail_stream_read_stop(lh_rail_stream_t *self);
lh_bool_t lh_rail_stream_try_write(lh_rail_stream_t *self, lh_rail_stream_write_t *request,
                                   const lh_byte_t *data, lh_usize_t size,
                                   lh_rail_stream_done_cb done);
lh_bool_t lh_rail_stream_try_shutdown(lh_rail_stream_t *self, lh_rail_stream_done_cb done);
lh_void   lh_rail_stream_close(lh_rail_stream_t *self, lh_rail_stream_done_cb closed);

lh_bool_t lh_rail_listener_try_start(lh_rail_listener_t *self, lh_rail_loop_t *loop,
                                     const lh_net_socket_addr_t *addr,
                                     lh_rail_listener_accept_cb accept);
```

- The stream and every write request are the caller's records, as a task is
  (`lh/rail` allocates nothing per operation); the request must stay alive
  until `done` runs.
- `read` gets the size that arrived in `buffer`, 0 at the end of the stream, a
  negative error otherwise. `done` gets the bytes written or the error.
- After `close`, the record is the caller's again only once `closed` ran.

### Pool — `lh/rail/pool.h`

```c
lh_bool_t lh_rail_pool_try_start(lh_rail_pool_t *self, lh_uint_t threads,
                                 const lh_rail_context_t *context);
const lh_rail_executor_t *lh_rail_pool_get_executor(const lh_rail_pool_t *self);
lh_bool_t lh_rail_pool_try_stop(lh_rail_pool_t *self, lh_rail_stop_kind_t kind,
                                lh_u32_t timeout_ms);
lh_void   lh_rail_pool_destroy(lh_rail_pool_t *self);
```

The thread executor's laws, with N threads and no order; each thread opens
its own context.

### What a server looks like on these bricks

```c
lh_void
echo_read(lh_rail_stream_t *stream, lh_ssize_t size)
{
    if (size <= 0)
    {
        lh_rail_stream_close(stream, client_closed);
        return;
    }
    client_t *client = client_of(stream);
    lh_rail_stream_try_write(stream, client_get_write_mut(client), client_get_buffer(client),
                             lh_cast_static(lh_usize_t, size), echo_written);
}

lh_void
echo_accept(lh_rail_listener_t *listener, lh_rail_stream_t *stream)
{
    client_t *client = client_create(stream);
    lh_rail_stream_try_read_start(stream, client_get_buffer_mut(client), 4096U, echo_read);
}
```

The same code runs on Windows, Linux and a board with a W5500; only the backend
filled in at start differs.

## 7. Order

Each step is its own change, measured with `bench/` where speed is the point,
verified on Windows, Linux (ASan) and the GCC option matrix, and committed by
the owner before the next.

1. This page, decided.
2. The poller: epoll, kqueue, IOCP, `poll()` fallback; a waiter for
   `lh_rail_loop_try_create_waiting`; the web host and Authum's HTTP server move
   onto it. Bench: 64 / 1 024 / 10 000 idle connections, wake latency.
3. The stream over the OS socket, with the listener; the web host's connection
   step becomes stream callbacks.
4. The pool executor.
5. Asynchronous files on the pool; DNS (`lh/os/net/resolve`) and IPv6 socket
   addresses.
6. The W5500 stream backend on a bare STM32F407 (SPI2, `lh/rail/bare.h`): the
   echo server of section 6 answers from the board.
7. Processes, signals, file watching — each when a user needs it.
