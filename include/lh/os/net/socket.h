/**
 * @file socket.h
 * @brief A blocking IPv4 socket — the first real ::lh_io_stream_t backend
 *        (a socket "today", per that file's doc).
 *
 * One struct, one open function for both transports (see
 * ::lh_os_system_net_socket_type_t): TCP and UDP are not different kinds of
 * object, just the `type`/`protocol` argument to the same `socket()` call.
 * The same handle is a client after ::lh_os_net_socket_connect or a
 * listening server after ::lh_os_net_socket_bind / ::lh_os_net_socket_listen
 * — matching the OS `socket()`, not a second type.
 *
 * Blocking by default. ::lh_os_net_socket_set_blocking makes it a socket that
 * never waits — send and recv answer ::LH_OS_RESULT_PENDING instead — and a
 * connect is then started and finished in two steps
 * (::lh_os_net_socket_connect_start, ::lh_os_net_socket_connect_wait), so that
 * how long a dead address may hold the caller up is the caller's number, not the
 * operating system's.
 *
 * On failure the reason is in ::lh_os_system_last_error (`lh/os/system.h`)
 * — every failure here is a native call, never one of our own checks.
 */

#ifndef LH_OS_NET_SOCKET_H
#define LH_OS_NET_SOCKET_H

#include <lh/config.h>
#include <lh/io/dgram.h>
#include <lh/io/stream.h>
#include <lh/os/result.h>
#include <lh/os/system/net/socket/handle.h>
#include <lh/os/system/net/socket/type.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/net/socket.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_net_socket
 * @brief A single OS socket handle.
 */
struct lh_os_net_socket
{
    lh_os_system_net_socket_handle_t handle; /**< Native socket handle. */
};
/** @copybrief lh_os_net_socket */
typedef struct lh_os_net_socket lh_os_net_socket_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── construct / lifetime ────────────────────────────────────────────────── */

/**
 * @brief Set @p self to the empty (not-yet-open) state.
 *
 * Pure value initialization — does not touch the OS. Call this (or
 * ::lh_os_net_socket_open directly) before any other operation.
 *
 * @param self Socket object to initialize.
 */
lh_void
lh_os_net_socket_init(lh_os_net_socket_t *self);

/**
 * @brief Open @p self as a new IPv4 socket of the given transport type.
 *
 * ::lh_os_net_init must have already succeeded once in this process.
 *
 * @param self Socket object to open; must be in the empty state
 *             (::lh_os_net_socket_init or freshly ::lh_os_net_socket_close'd).
 * @param type ::lh_os_system_net_socket_type_tcp or ::lh_os_system_net_socket_type_udp.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
lh_bool_t
lh_os_net_socket_open(lh_os_net_socket_t *self, lh_os_system_net_socket_type_t type);

/**
 * @brief Close @p self's handle (if open) and return it to the empty state.
 *
 * Safe to call on an already-empty socket.
 *
 * @param self Socket object to close.
 */
lh_void
lh_os_net_socket_close(lh_os_net_socket_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the raw handle stored in @p self.
 * @param self Socket to read from.
 * @return Current ::lh_os_system_net_socket_handle_t
 *         (::LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID if not open).
 */
lh_os_system_net_socket_handle_t
lh_os_net_socket_get_handle(const lh_os_net_socket_t *self);

/**
 * @brief Test whether @p self currently holds an open handle.
 * @param self Socket to test.
 * @return ::lh_bool_true if open, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_os_net_socket_is_valid(const lh_os_net_socket_t *self);

/* ── operations ──────────────────────────────────────────────────────────── */

/**
 * @brief Connect an open @p self to @p addr (blocking).
 *
 * @param self Open socket (see ::lh_os_net_socket_open).
 * @param addr Remote IPv4 address and port to connect to.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed
 *         (connection refused, unreachable, timed out, ...).
 */
lh_bool_t
lh_os_net_socket_connect(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Make @p self blocking or non-blocking.
 *
 * @param self     Open socket.
 * @param blocking ::lh_bool_false for a socket whose calls never wait.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
lh_bool_t
lh_os_net_socket_set_blocking(lh_os_net_socket_t *self, lh_bool_t blocking);

/**
 * @brief Start connecting a non-blocking @p self to @p addr.
 *
 * @return `0` connected already, ::LH_OS_RESULT_PENDING under way,
 *         ::LH_OS_RESULT_INVALID failed.
 */
lh_ssize_t
lh_os_net_socket_connect_start(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Wait at most @p timeout_ms for the connect under way on @p self.
 *
 * @return `0` connected, ::LH_OS_RESULT_PENDING still under way when the time is
 *         up, ::LH_OS_RESULT_INVALID failed (refused, unreachable, ...).
 */
lh_ssize_t
lh_os_net_socket_connect_wait(lh_os_net_socket_t *self, lh_u32_t timeout_ms);

/**
 * @brief Connect @p self to @p addr and leave it non-blocking, waiting at most
 *        @p timeout_ms for the other end to accept.
 *
 * The three calls above in the order they are always made in, so that what a
 * stream user gets back is a socket that never waits: ::lh_os_net_socket_get_stream
 * on it reads ::LH_IO_PENDING when nothing has arrived and `0` when the peer has
 * closed. A connect left to the operating system waits out its SYN retries, twenty
 * seconds and more, on an address where nothing answers -- a device that is
 * switched off; here that costs @p timeout_ms.
 *
 * @param self       Open TCP socket (::lh_os_net_socket_open).
 * @param addr       Where to connect.
 * @param timeout_ms How long the connect may take.
 *
 * @return ::lh_bool_true when connected; on failure (refused, unreachable, no
 *         answer in time) ::lh_bool_false, and the socket is still open for the
 *         caller to close.
 */
lh_bool_t
lh_os_net_socket_connect_within(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr,
                                lh_u32_t timeout_ms);

/**
 * @brief Bind an open @p self to @p addr (TCP listen socket or UDP local port).
 *
 * Port `0` asks the OS for an ephemeral port; read it back with
 * ::lh_os_net_socket_get_local_addr.
 *
 * @param self Open socket (see ::lh_os_net_socket_open).
 * @param addr Local IPv4 address and port to bind to.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed
 *         (address in use, permission denied, ...).
 */
lh_bool_t
lh_os_net_socket_bind(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Mark a bound TCP @p self as listening for incoming connections.
 *
 * @param self    Open, bound TCP socket.
 * @param backlog Maximum pending connections the OS should queue.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
lh_bool_t
lh_os_net_socket_listen(lh_os_net_socket_t *self, lh_int_t backlog);

/**
 * @brief Accept one pending connection on a listening TCP @p self (blocking).
 *
 * @param self   Listening TCP socket (see ::lh_os_net_socket_listen).
 * @param client Receives the new connected socket; must be in the empty
 *               state (::lh_os_net_socket_init or freshly closed).
 * @param peer   Receives the remote IPv4 address.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
lh_bool_t
lh_os_net_socket_accept(lh_os_net_socket_t *self, lh_os_net_socket_t *client,
                        lh_net_ip4_socket_addr_t *peer);

/**
 * @brief Read the local IPv4 address currently bound to @p self.
 *
 * Works after bind (including port `0`), listen, or connect.
 *
 * @param self Socket whose local address to query.
 * @param out  Receives the local IPv4 address.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
lh_bool_t
lh_os_net_socket_get_local_addr(const lh_os_net_socket_t *self, lh_net_ip4_socket_addr_t *out);

/**
 * @brief Enable or disable `SO_REUSEADDR` on @p self.
 *
 * Typical for a listen socket so a restart can re-bind the same port while
 * the previous instance's sockets are still in `TIME_WAIT`.
 *
 * @param self    Open socket.
 * @param enabled ::lh_bool_true to allow reuse, ::lh_bool_false to clear it.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
lh_bool_t
lh_os_net_socket_set_reuse_addr(lh_os_net_socket_t *self, lh_bool_t enabled);

/**
 * @brief Enable or disable `TCP_NODELAY` on a TCP @p self.
 *
 * A connection that answers every input with one write (a frame, a reply)
 * wants Nagle off: otherwise the tail of the write can wait for a delayed ACK.
 *
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
lh_bool_t
lh_os_net_socket_set_no_delay(lh_os_net_socket_t *self, lh_bool_t enabled);

/**
 * @brief Copy the handles of @p count @p sockets into @p out (at most
 *        ::LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX) and answer @p count.
 */
lh_usize_t
lh_os_net_socket_get_handles(lh_os_system_net_socket_handle_t *out, lh_os_net_socket_t *const *sockets,
                             lh_usize_t count);

/**
 * @brief Wait at most @p timeout_ms until one of @p read can be read (or
 *        accepted from) or one of @p write can be written.
 *
 * What a server loop calls instead of sleeping: it returns as soon as there
 * is something to do. Each list holds at most ::LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX
 * sockets; both empty answers ::LH_OS_RESULT_PENDING at once.
 *
 * @return How many sockets are ready (> 0), ::LH_OS_RESULT_PENDING when the
 *         time ran out, ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_net_socket_wait(lh_os_net_socket_t *const *read, lh_usize_t read_count, lh_os_net_socket_t *const *write,
                      lh_usize_t write_count, lh_u32_t timeout_ms);

/**
 * @brief ::lh_io_writer_write_fn backed by @p context's socket.
 *
 * @param self An ::lh_os_net_socket_t* (passed as the writer's context).
 * @param buf     Source buffer.
 * @param size    Number of bytes from @p buf to send.
 * @return Bytes actually sent, or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_net_socket_send(lh_self_ptr self, const lh_ptr buf, lh_usize_t size);

/**
 * @brief ::lh_io_reader_read_fn backed by @p context's socket.
 *
 * @param self An ::lh_os_net_socket_t* (passed as the reader's context).
 * @param buf     Destination buffer.
 * @param size    Maximum bytes to read into @p buf.
 * @return Bytes actually received (`0` if the peer closed the connection),
 *         or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_net_socket_recv(lh_self_ptr self, lh_ptr buf, lh_usize_t size);

/**
 * @brief How many bytes are waiting in @p self's receive buffer, unread.
 *
 * Settles "nothing has arrived" against "something has and the read was made at
 * the wrong moment", which a non-blocking recv answers the same way
 * (::LH_OS_RESULT_PENDING). Meaningful on a TCP socket (see
 * ::lh_os_system_net_socket_pending for UDP).
 *
 * @param self Open socket.
 * @return Bytes pending (`0` when empty), or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_net_socket_pending(const lh_os_net_socket_t *self);

/**
 * @brief ::lh_io_dgram_writer_send_fn backed by @p context's socket (`sendto`).
 *
 * Each call is one datagram to @p addr. Not a stream write.
 *
 * @param self An ::lh_os_net_socket_t* (passed as the writer's context).
 * @param buf     Source buffer (the whole datagram).
 * @param size    Number of bytes from @p buf to send.
 * @param addr    Destination IPv4 address.
 * @return Bytes actually sent, or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_net_socket_sendto(lh_self_ptr self, const lh_ptr buf, lh_usize_t size,
                        const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief ::lh_io_dgram_reader_recv_fn backed by @p context's socket (`recvfrom`).
 *
 * Each call is one datagram; @p addr receives the sender. `0` is an empty
 * datagram, not end-of-stream.
 *
 * @param self An ::lh_os_net_socket_t* (passed as the reader's context).
 * @param buf     Destination buffer.
 * @param size    Maximum bytes to copy into @p buf.
 * @param addr    Receives the sender's IPv4 address.
 * @return Bytes actually received, or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_net_socket_recvfrom(lh_self_ptr self, lh_ptr buf, lh_usize_t size,
                          lh_net_ip4_socket_addr_t *addr);

/* ── lh_io adapters ──────────────────────────────────────────────────────── */

/**
 * @brief Build an ::lh_io_reader_t that reads from @p self.
 *
 * @param self Socket to read from; must outlive the returned reader.
 * @return An ::lh_io_reader_t wired to ::lh_os_net_socket_recv.
 */
lh_void
lh_os_net_socket_get_reader(lh_os_net_socket_t *self, lh_io_reader_t *out);

/**
 * @brief Build an ::lh_io_writer_t that writes to @p self.
 *
 * @param self Socket to write to; must outlive the returned writer.
 * @return An ::lh_io_writer_t wired to ::lh_os_net_socket_send.
 */
lh_void
lh_os_net_socket_get_writer(lh_os_net_socket_t *self, lh_io_writer_t *out);

/**
 * @brief Build an ::lh_io_stream_t (reader + writer) over @p self.
 *
 * This is the point of ::lh_io_stream_t: everything above this call sees a
 * generic duplex stream and does not need to know a socket is behind it.
 *
 * @p self should be a ::lh_os_system_net_socket_type_tcp socket — a stream has no
 * message boundaries, and treating a UDP socket this way would silently
 * lose the fact that each ::lh_os_net_socket_recv call actually returns one
 * whole datagram (or drops the rest of one that didn't fit in @c buf).
 *
 * @param self Open, connected TCP socket to wrap; must outlive the
 *             returned stream.
 * @return An ::lh_io_stream_t backed by @p self.
 */
lh_void
lh_os_net_socket_get_stream(lh_os_net_socket_t *self, lh_io_stream_t *out);

/**
 * @brief Build an ::lh_io_dgram_reader_t that receives from @p self.
 *
 * @param self Socket to receive from; must outlive the returned reader.
 * @return An ::lh_io_dgram_reader_t wired to ::lh_os_net_socket_recvfrom.
 */
lh_void
lh_os_net_socket_get_dgram_reader(lh_os_net_socket_t *self, lh_io_dgram_reader_t *out);

/**
 * @brief Build an ::lh_io_dgram_writer_t that sends through @p self.
 *
 * @param self Socket to send through; must outlive the returned writer.
 * @return An ::lh_io_dgram_writer_t wired to ::lh_os_net_socket_sendto.
 */
lh_void
lh_os_net_socket_get_dgram_writer(lh_os_net_socket_t *self, lh_io_dgram_writer_t *out);

/**
 * @brief Build an ::lh_io_dgram_t (recvfrom + sendto) over @p self.
 *
 * This is the datagram counterpart of ::lh_os_net_socket_get_stream: everything
 * above this call sees datagrams plus addresses and does not need to know a
 * socket is behind it.
 *
 * @p self should be a ::lh_os_system_net_socket_type_udp socket — TCP has no
 * message boundaries or per-call destination, which is exactly what a
 * datagram pair is for.
 *
 * @param self Open UDP socket to wrap; must outlive the returned dgram.
 * @return An ::lh_io_dgram_t backed by @p self.
 */
lh_void
lh_os_net_socket_get_dgram(lh_os_net_socket_t *self, lh_io_dgram_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_NET_SOCKET_H */
