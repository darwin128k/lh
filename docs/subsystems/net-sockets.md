# Non-blocking server sockets

Initialize networking with `lh_os_net_init` before opening sockets. Existing
`open`, `bind`, `listen`, `accept`, `send` and `recv` operations retain their
contracts. `lh_os_net_socket_open_nonblocking` and
`lh_os_net_socket_accept_nonblocking` additionally return sockets that never
wait on I/O. POSIX sockets close on exec; Windows handles are non-inheritable.

Start with an empty socket object. Open it with `open_nonblocking`, bind an
IPv4 address, then listen. Port zero selects an ephemeral port, which can be
read with `lh_os_net_socket_get_local_addr`. Once the listener is readable,
accept into another empty socket. The listening socket must be non-blocking;
the accept function configures the new socket, not the listener.

An accept with no connection ready returns false without modifying the peer
address, destination socket or captured native error. An interrupted accept
behaves the same way. Other native failures are captured in
`lh_os_system_last_error`. A failed setup closes its newly acquired handle.
Send and receive distinguish `LH_OS_RESULT_PENDING`, zero-byte EOF, and
`LH_OS_RESULT_INVALID`; callers must preserve that distinction in their loops.

Linux applies `SOCK_NONBLOCK | SOCK_CLOEXEC` atomically in `socket` and `accept4`.
Other POSIX platforms configure the descriptor before returning it. Windows
clears handle inheritance and applies `FIONBIO` before returning the socket.
Those fallback configurations are not atomic with concurrent process creation;
applications requiring that guarantee must coordinate child-process creation.
Close every socket before calling `lh_os_net_deinit`.

Raw-handle counterparts live in `lh/os/system/net/socket.h`. They use the same
native backend and do not expose native address structures in public headers.
These APIs contain no HTTP routing, rate limits, resource ownership or host
lifecycle rules.

## Per-socket readiness and wake signals

`lh_os_net_socket_poll` takes an array of `lh_os_net_socket_poll_t`: a borrowed
handle, read/write interests, and an output readiness mask. At most
`LH_OS_NET_SOCKET_POLL_MAX` (512) entries are accepted. Invalid handles are
ignored. Every call clears the output masks, including timeout and interrupted
waits. The return value counts ready entries, not individual event bits. Empty
or all-invalid lists return pending immediately. A zero timeout only checks.

POSIX uses poll. Windows uses select with counted handle arrays sized for the
full batch, including more than the default 64 handles. EOF is readable;
receive must still distinguish data, pending and EOF. Native errors/exceptional
conditions and hangup flags are additional hints, not substitutes for I/O results.
Windows reports EOF as readability rather than a separate hangup flag.

`lh_os_wake_t` owns a non-blocking socket pair. Initialize, open, then borrow its
read endpoint into the poll array. Another thread calls `lh_os_wake_signal`.
Signals coalesce when buffers fill and never wait for a reader. A consumer can
reset by draining available data; in-flight notifications can produce later
readiness, so always recheck the caller-owned predicate. Close after producers
and the waiting thread have stopped. No memory allocation occurs in the portable
wake implementation. POSIX creates a local socket pair; Windows creates and
verifies a loopback connection within the caller's per-phase setup timeout.
Windows networking must be initialized first. Send/receive retry interrupted
native calls so an interruption cannot silently discard a wake signal.

## Explicit-offset file transfer

`lh_os_system_net_socket_send_file` sends up to the requested amount from a
readable regular file. Its unsigned 64-bit offset advances only by bytes actually
sent; it is unchanged on pending/error/EOF. Zero-size requests return zero without
I/O. Partial sends are normal. Source offsets must fit signed 64-bit file positions.

Linux uses sendfile. Other backends use a bounded read/send buffer. A socket can
be non-blocking, but reading file data can still wait for disk. The Windows
fallback can change the file's current position, so coordinate source-file access.
The Linux caller must block or ignore SIGPIPE (the LH thread API has a flag for
that), as with native sendfile. These operations provide no file access policy,
HTTP routing, bandwidth budget or module ownership.
