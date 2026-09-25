/**
 * @file type.h
 * @brief Which transport protocol ::lh_os_system_net_socket_open creates.
 *
 * TCP and UDP are not different kinds of object — both come from the same
 * `socket()` call, distinguished only by the `type`/`protocol` argument
 * passed to it. ::lh_os_net_socket_t reflects that: one struct, one open
 * function, and this constant says which protocol the OS should hand back.
 */

#ifndef LH_OS_SYSTEM_NET_SOCKET_TYPE_H
#define LH_OS_SYSTEM_NET_SOCKET_TYPE_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_system_net_socket_type_t
 * @brief Discriminator for which transport protocol to open.
 *
 * Alias for ::lh_u8_t; canonical values are the `lh_os_system_net_socket_type_*`
 * constants below.
 */
typedef lh_u8_t lh_os_system_net_socket_type_t;

/**
 * @def lh_os_system_net_socket_type_tcp
 * @brief Connection-oriented, byte-stream transport (`SOCK_STREAM`).
 *
 * The only type ::lh_os_net_socket_get_reader / _get_writer / _get_stream
 * are meant for — a stream has no message boundaries, which is exactly
 * what TCP gives and UDP does not.
 */
#define lh_os_system_net_socket_type_tcp 0U

/**
 * @def lh_os_system_net_socket_type_udp
 * @brief Connectionless, message-oriented transport (`SOCK_DGRAM`).
 *
 * ::lh_os_net_socket_open supports this today. Whole datagrams go through
 * ::lh_os_net_socket_sendto / ::lh_os_net_socket_recvfrom (and
 * ::lh_os_net_socket_get_dgram). Treating a UDP socket as an
 * ::lh_io_stream_t would silently drop packet boundaries, so
 * ::lh_os_net_socket_get_stream is not offered for it.
 */
#define lh_os_system_net_socket_type_udp 1U

#endif /* LH_OS_SYSTEM_NET_SOCKET_TYPE_H */
