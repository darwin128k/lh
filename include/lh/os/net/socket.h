/**
 * @file socket.h
 * @brief A blocking IPv4 socket — the first real ::lh_io_stream_t backend
 *        (a socket "today", per that file's doc).
 *
 * One struct, one open function for both transports (see
 * ::lh_os_net_socket_type_t): TCP and UDP are not different kinds of
 * object, just the `type`/`protocol` argument to the same `socket()` call.
 * The same handle is a client after ::lh_os_net_socket_connect or a
 * listening server after ::lh_os_net_socket_bind / ::lh_os_net_socket_listen
 * — matching the OS `socket()`, not a second type.
 *
 * Deliberately narrow otherwise: no non-blocking mode.
 */

#ifndef LH_OS_NET_SOCKET_H
#define LH_OS_NET_SOCKET_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/io/dgram.h>
#include <lh/io/reader.h>
#include <lh/io/stream.h>
#include <lh/io/writer.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/numeric/types.h>
#include <lh/os/net/socket/handle.h>
#include <lh/os/net/socket/type.h>
#include <lh/ptr.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/net/socket.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

/**
 * @struct lh_os_net_socket
 * @typedef lh_os_net_socket_t
 * @brief A single OS socket handle.
 */
struct lh_os_net_socket
{
    lh_os_net_socket_handle_t handle;
};
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
LH_ATTRIBUTE_SYMBOL
void
lh_os_net_socket_init(lh_os_net_socket_t *self);

/**
 * @brief Open @p self as a new IPv4 socket of the given transport type.
 *
 * ::lh_os_net_init must have already succeeded once in this process.
 *
 * @param self Socket object to open; must be in the empty state
 *             (::lh_os_net_socket_init or freshly ::lh_os_net_socket_close'd).
 * @param type ::lh_os_net_socket_type_tcp or ::lh_os_net_socket_type_udp.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_net_socket_open(lh_os_net_socket_t *self, lh_os_net_socket_type_t type);

/**
 * @brief Close @p self's handle (if open) and return it to the empty state.
 *
 * Safe to call on an already-empty socket.
 *
 * @param self Socket object to close.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_net_socket_close(lh_os_net_socket_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the raw handle stored in @p self.
 * @param self Socket to read from.
 * @return Current ::lh_os_net_socket_handle_t
 *         (::LH_OS_NET_SOCKET_HANDLE_INVALID if not open).
 */
LH_ATTRIBUTE_SYMBOL
lh_os_net_socket_handle_t
lh_os_net_socket_get_handle(const lh_os_net_socket_t *self);

/**
 * @brief Test whether @p self currently holds an open handle.
 * @param self Socket to test.
 * @return ::lh_bool_true if open, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_SYMBOL
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
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_net_socket_connect(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr);

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
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_net_socket_bind(lh_os_net_socket_t *self, const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Mark a bound TCP @p self as listening for incoming connections.
 *
 * @param self    Open, bound TCP socket.
 * @param backlog Maximum pending connections the OS should queue.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
LH_ATTRIBUTE_SYMBOL
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
LH_ATTRIBUTE_SYMBOL
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
LH_ATTRIBUTE_SYMBOL
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
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_net_socket_set_reuse_addr(lh_os_net_socket_t *self, lh_bool_t enabled);

/**
 * @brief ::lh_io_writer_write_fn backed by @p context's socket.
 *
 * @param context An ::lh_os_net_socket_t* (passed as the writer's context).
 * @param buf     Source buffer.
 * @param size    Number of bytes from @p buf to send.
 * @return Bytes actually sent, or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_net_socket_send(lh_ptr context, const lh_ptr buf, lh_usize_t size);

/**
 * @brief ::lh_io_reader_read_fn backed by @p context's socket.
 *
 * @param context An ::lh_os_net_socket_t* (passed as the reader's context).
 * @param buf     Destination buffer.
 * @param size    Maximum bytes to read into @p buf.
 * @return Bytes actually received (`0` if the peer closed the connection),
 *         or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_net_socket_recv(lh_ptr context, lh_ptr buf, lh_usize_t size);

/**
 * @brief ::lh_io_dgram_writer_send_fn backed by @p context's socket (`sendto`).
 *
 * Each call is one datagram to @p addr. Not a stream write.
 *
 * @param context An ::lh_os_net_socket_t* (passed as the writer's context).
 * @param buf     Source buffer (the whole datagram).
 * @param size    Number of bytes from @p buf to send.
 * @param addr    Destination IPv4 address.
 * @return Bytes actually sent, or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_net_socket_sendto(lh_ptr context, const lh_ptr buf, lh_usize_t size,
                        const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief ::lh_io_dgram_reader_recv_fn backed by @p context's socket (`recvfrom`).
 *
 * Each call is one datagram; @p addr receives the sender. `0` is an empty
 * datagram, not end-of-stream.
 *
 * @param context An ::lh_os_net_socket_t* (passed as the reader's context).
 * @param buf     Destination buffer.
 * @param size    Maximum bytes to copy into @p buf.
 * @param addr    Receives the sender's IPv4 address.
 * @return Bytes actually received, or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_net_socket_recvfrom(lh_ptr context, lh_ptr buf, lh_usize_t size,
                          lh_net_ip4_socket_addr_t *addr);

/* ── lh_io adapters ──────────────────────────────────────────────────────── */

/**
 * @brief Build an ::lh_io_reader_t that reads from @p self.
 *
 * @param self Socket to read from; must outlive the returned reader.
 * @return An ::lh_io_reader_t wired to ::lh_os_net_socket_recv.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_reader_t
lh_os_net_socket_get_reader(lh_os_net_socket_t *self);

/**
 * @brief Build an ::lh_io_writer_t that writes to @p self.
 *
 * @param self Socket to write to; must outlive the returned writer.
 * @return An ::lh_io_writer_t wired to ::lh_os_net_socket_send.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_writer_t
lh_os_net_socket_get_writer(lh_os_net_socket_t *self);

/**
 * @brief Build an ::lh_io_stream_t (reader + writer) over @p self.
 *
 * This is the point of ::lh_io_stream_t: everything above this call sees a
 * generic duplex stream and does not need to know a socket is behind it.
 *
 * @p self should be a ::lh_os_net_socket_type_tcp socket — a stream has no
 * message boundaries, and treating a UDP socket this way would silently
 * lose the fact that each ::lh_os_net_socket_recv call actually returns one
 * whole datagram (or drops the rest of one that didn't fit in @c buf).
 *
 * @param self Open, connected TCP socket to wrap; must outlive the
 *             returned stream.
 * @return An ::lh_io_stream_t backed by @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_stream_t
lh_os_net_socket_get_stream(lh_os_net_socket_t *self);

/**
 * @brief Build an ::lh_io_dgram_reader_t that receives from @p self.
 *
 * @param self Socket to receive from; must outlive the returned reader.
 * @return An ::lh_io_dgram_reader_t wired to ::lh_os_net_socket_recvfrom.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_dgram_reader_t
lh_os_net_socket_get_dgram_reader(lh_os_net_socket_t *self);

/**
 * @brief Build an ::lh_io_dgram_writer_t that sends through @p self.
 *
 * @param self Socket to send through; must outlive the returned writer.
 * @return An ::lh_io_dgram_writer_t wired to ::lh_os_net_socket_sendto.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_dgram_writer_t
lh_os_net_socket_get_dgram_writer(lh_os_net_socket_t *self);

/**
 * @brief Build an ::lh_io_dgram_t (recvfrom + sendto) over @p self.
 *
 * This is the datagram counterpart of ::lh_os_net_socket_get_stream: everything
 * above this call sees datagrams plus addresses and does not need to know a
 * socket is behind it.
 *
 * @p self should be a ::lh_os_net_socket_type_udp socket — TCP has no
 * message boundaries or per-call destination, which is exactly what a
 * datagram pair is for.
 *
 * @param self Open UDP socket to wrap; must outlive the returned dgram.
 * @return An ::lh_io_dgram_t backed by @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_io_dgram_t
lh_os_net_socket_get_dgram(lh_os_net_socket_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_NET_SOCKET_H */
