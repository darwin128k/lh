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

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/numeric/types.h>
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
LH_ATTRIBUTE_SYMBOL
lh_os_system_net_socket_handle_t
lh_os_system_net_socket_open(lh_os_system_net_socket_type_t type);

/**
 * @brief Release @p handle. The close result is not reported.
 *
 * @param handle Open handle; must not be ::LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_os_system_net_socket_close(lh_os_system_net_socket_handle_t handle);

/**
 * @brief Connect @p handle to @p addr (blocking).
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_system_net_socket_connect(lh_os_system_net_socket_handle_t handle, const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Bind @p handle to the local @p addr.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_system_net_socket_bind(lh_os_system_net_socket_handle_t handle, const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Mark a bound TCP @p handle as listening.
 * @param backlog Maximum pending connections to queue; must not be negative.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
LH_ATTRIBUTE_SYMBOL
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
LH_ATTRIBUTE_SYMBOL
lh_os_system_net_socket_handle_t
lh_os_system_net_socket_accept(lh_os_system_net_socket_handle_t handle, lh_net_ip4_socket_addr_t *peer);

/**
 * @brief Read the local IPv4 address bound to @p handle into @p out.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_system_net_socket_get_local_addr(lh_os_system_net_socket_handle_t handle, lh_net_ip4_socket_addr_t *out);

/**
 * @brief Enable or disable `SO_REUSEADDR` on @p handle.
 * @return ::lh_bool_true on success, ::lh_bool_false if the native call failed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_os_system_net_socket_set_reuse_addr(lh_os_system_net_socket_handle_t handle, lh_bool_t enabled);

/**
 * @brief Send up to @p size bytes from @p buf on a connected @p handle.
 *
 * One native call; may send fewer bytes than asked (also when @p size
 * exceeds what the native call accepts at once).
 *
 * @return Bytes actually sent, or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_system_net_socket_send(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size);

/**
 * @brief Receive up to @p size bytes into @p buf from a connected @p handle.
 *
 * One native call; may receive fewer bytes than asked.
 *
 * @return Bytes actually received (`0` if the peer closed the connection),
 *         or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_system_net_socket_recv(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size);

/**
 * @brief Send one datagram of @p size bytes from @p buf to @p addr.
 *
 * A @p size the native call cannot take at once is clamped, which makes the
 * datagram oversized for UDP — the native call then fails instead of
 * sending a truncated one.
 *
 * @return Bytes actually sent, or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_system_net_socket_sendto(lh_os_system_net_socket_handle_t handle, const lh_ptr buf, lh_usize_t size,
                               const lh_net_ip4_socket_addr_t *addr);

/**
 * @brief Receive one datagram into @p buf; @p addr receives the sender.
 *
 * `0` is an empty datagram, not end-of-stream. @p addr is written only on
 * success.
 *
 * @return Bytes actually received, or a negative value on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_ssize_t
lh_os_system_net_socket_recvfrom(lh_os_system_net_socket_handle_t handle, lh_ptr buf, lh_usize_t size,
                                 lh_net_ip4_socket_addr_t *addr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_NET_SOCKET_H */
