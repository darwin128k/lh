/**
 * @file socket.h
 * @brief Kernel socket primitives on a raw ::lh_os_system_net_socket_handle_t.
 *
 * The only place that talks to Winsock (Windows) or BSD sockets (POSIX).
 * Every function has one contract on every platform; the backend is picked
 * by CMake (`src/lh/os/system/win` or `src/lh/os/system/posix`), not by
 * `#if` at the call site.
 *
 * A socket handle is its own type, not a file handle: on Windows a
 * `SOCKET` is not a `HANDLE` for `ReadFile` / `CloseHandle`.
 *
 * Addresses cross this boundary as ::lh_net_ip4_socket_addr_t; the native
 * `sockaddr_in` never leaves the backend. Holds no state: a handle in, a
 * result out.
 *
 * On failure the native reason is in ::lh_os_system_last_error.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_NET_SOCKET_H
#define LH_OS_SYSTEM_NET_SOCKET_H

#include <lh/void.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/os/result.h>
#include <lh/os/system/net/socket/handle.h>
#include <lh/os/system/net/socket/type.h>
#include <lh/ptr.h>
#include <lh/size.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/net/socket.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create a new IPv4 socket of the given transport type.
 *
 * @param type ::lh_os_system_net_socket_type_tcp or
 *             ::lh_os_system_net_socket_type_udp.
 * @return Open handle, or ::LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID on failure.
 */
lh_os_system_net_socket_handle_t
lh_os_system_net_socket_open(lh_os_system_net_socket_type_t type);

/**
 * @brief Release @p handle. The close result is not reported.
 *
 * @param handle Open handle; must not be ::LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID.
 */
lh_void
lh_os_system_net_socket_close(lh_os_system_net_socket_handle_t handle);

/**
 * @brief Connect @p handle to @p addr (blocking).
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
lh_bool_t
lh_os_system_net_socket_connect(lh_os_system_net_socket_handle_t handle, const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Put @p handle into blocking or non-blocking mode.
 *
 * A non-blocking handle never waits: ::lh_os_system_net_socket_send and
 * ::lh_os_system_net_socket_recv answer ::LH_OS_RESULT_PENDING instead, and a
 * connect is started with ::lh_os_system_net_socket_connect_start.
 *
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
lh_bool_t
lh_os_system_net_socket_set_blocking(lh_os_system_net_socket_handle_t handle, lh_bool_t blocking);

/**
 * @brief Start connecting a non-blocking @p handle to @p addr.
 *
 * @return `0` if it connected at once (loopback can), ::LH_OS_RESULT_PENDING if
 *         the connect is under way (finish it with
 *         ::lh_os_system_net_socket_connect_wait), ::LH_OS_RESULT_INVALID if it
 *         failed.
 */
lh_ssize_t
lh_os_system_net_socket_connect_start(lh_os_system_net_socket_handle_t handle,
                                      const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Wait at most @p timeout_ms for a connect started by
 *        ::lh_os_system_net_socket_connect_start to finish.
 *
 * @param handle     Socket a connect was started on.
 * @param timeout_ms `0` only looks.
 *
 * @return `0` once connected, ::LH_OS_RESULT_PENDING if it is still under way
 *         when the time is up, ::LH_OS_RESULT_INVALID if it failed — refused,
 *         unreachable — with the reason in ::lh_os_system_last_error.
 */
lh_ssize_t
lh_os_system_net_socket_connect_wait(lh_os_system_net_socket_handle_t handle, lh_u32_t timeout_ms);

/**
 * @brief Bind @p handle to the local @p addr.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
lh_bool_t
lh_os_system_net_socket_bind(lh_os_system_net_socket_handle_t handle, const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Mark a bound TCP @p handle as listening.
 * @param handle  Bound TCP socket.
 * @param backlog Maximum pending connections to queue; must not be negative.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
lh_bool_t
lh_os_system_net_socket_listen(lh_os_system_net_socket_handle_t handle, lh_int_t backlog);

/**
 * @brief Accept one pending connection on a listening @p handle (blocking).
 *
 * @param handle Listening TCP handle.
 * @param peer   Receives the remote IPv4 address on success.
 * @return The connected handle, or ::LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID
 *         on failure.
 */
lh_os_system_net_socket_handle_t
lh_os_system_net_socket_accept(lh_os_system_net_socket_handle_t handle, lh_net_ip4_socket_addr_t *peer);

/**
 * @brief Read the local IPv4 address bound to @p handle into @p out.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
lh_bool_t
lh_os_system_net_socket_get_local_addr(lh_os_system_net_socket_handle_t handle, lh_net_ip4_socket_addr_t *out);

/**
 * @brief Enable or disable `SO_REUSEADDR` on @p handle.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
lh_bool_t
lh_os_system_net_socket_set_reuse_addr(lh_os_system_net_socket_handle_t handle, lh_bool_t enabled);

/**
 * @brief Enable or disable `TCP_NODELAY` (Nagle off) on a TCP @p handle.
 *
 * With Nagle on, the tail of a write smaller than a segment waits for the
 * peer's ACK of the one before it, and a delayed ACK makes that a wait of
 * tens of milliseconds. A connection that answers each input with a frame
 * wants it off.
 *
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
lh_bool_t
lh_os_system_net_socket_set_no_delay(lh_os_system_net_socket_handle_t handle, lh_bool_t enabled);

/** How many handles one ::lh_os_system_net_socket_wait watches in each set at most. */
#define LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX 64U

/**
 * @brief Wait at most @p timeout_ms until any of @p read is readable (or, for a
 *        listening handle, has a connection to accept) or any of @p write is
 *        writable.
 *
 * Each set holds at most ::LH_OS_SYSTEM_NET_SOCKET_WAIT_MAX handles. With both
 * sets empty it answers ::LH_OS_RESULT_PENDING at once instead of sleeping.
 *
 * @param read       Handles to watch for reading; may be ::lh_null when @p read_count is `0`.
 * @param read_count How many handles @p read holds.
 * @param write      Handles to watch for writing; may be ::lh_null when @p write_count is `0`.
 * @param write_count How many handles @p write holds.
 * @param timeout_ms `0` only looks.
 * @return How many handles are ready (> 0), ::LH_OS_RESULT_PENDING when the
 *         time ran out first, ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_system_net_socket_wait(const lh_os_system_net_socket_handle_t *read, lh_usize_t read_count,
                             const lh_os_system_net_socket_handle_t *write, lh_usize_t write_count,
                             lh_u32_t timeout_ms);

/**
 * @brief Send up to @p size bytes from @p buf on a connected @p handle.
 *
 * One native call; may send fewer bytes than asked (also when @p size
 * exceeds what the native call accepts at once). Never raises SIGPIPE: a peer
 * that has gone is a failed send, not a dead process.
 *
 * @return Bytes actually sent, ::LH_OS_RESULT_PENDING when a non-blocking
 *         handle's buffer is full, or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_system_net_socket_send(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size);

/**
 * @brief Receive up to @p size bytes into @p buf from a connected @p handle.
 *
 * One native call; may receive fewer bytes than asked.
 *
 * @return Bytes actually received (`0` if the peer closed the connection),
 *         ::LH_OS_RESULT_PENDING when a non-blocking handle has nothing yet, or
 *         ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_system_net_socket_recv(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size);

/**
 * @brief Send one datagram of @p size bytes from @p buf to @p addr.
 *
 * A @p size the native call cannot take at once is clamped, which makes the
 * datagram oversized for UDP — the native call then fails instead of
 * sending a truncated one.
 *
 * @return Bytes actually sent, or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_system_net_socket_sendto(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size,
                               const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief How many bytes are sitting in @p handle's receive buffer, without reading them.
 *
 * `FIONREAD` (`ioctlsocket` on Windows, `ioctl` on POSIX). On a stream that is what
 * a `recv` made right now could return; on a datagram socket Windows answers the
 * size of everything queued and POSIX the size of the next datagram, so do not
 * size a buffer from it on UDP.
 *
 * @return Bytes pending (`0` when empty), or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_system_net_socket_pending(lh_os_system_net_socket_handle_t handle);

/**
 * @brief Receive one datagram into @p buf; @p addr receives the sender.
 *
 * `0` is an empty datagram, not end-of-stream. @p addr is written only on
 * success.
 *
 * @return Bytes actually received, or ::LH_OS_RESULT_INVALID on failure.
 */
lh_ssize_t
lh_os_system_net_socket_recvfrom(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size,
                                 lh_net_ip4_socket_addr_t *addr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_NET_SOCKET_H */
