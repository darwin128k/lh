/**
 * @file fields.h
 * @brief Macro for declaring socket-address member fields (`ip` / `port`).
 */

#ifndef LH_NET_SOCKET_ADDR_FIELDS_H
#define LH_NET_SOCKET_ADDR_FIELDS_H

#include <lh/net/port.h>

/**
 * @def lh_net_socket_addr_fields(ip_type)
 * @brief Expands to the `ip` and `port` members of a socket-address struct.
 *
 * Intended to be embedded inside a struct body so every concrete IP-version
 * socket address (::lh_net_socket_addr_t today; a future IPv6 counterpart
 * with its own extra fields) shares this one declaration shape for the part
 * they do have in common. `port` is always ::lh_net_port_t — only the
 * address type varies by IP version, so only it is a parameter.
 *
 * @param ip_type Type of the `ip` field (e.g. ::lh_net_ip4_t).
 *
 * Example usage:
 * @code{.c}
 * typedef struct lh_net_socket_addr {
 *     lh_net_socket_addr_fields(lh_net_ip4_t);
 * } lh_net_socket_addr_t;
 * @endcode
 */
#define lh_net_socket_addr_fields(ip_type)                                                         \
    ip_type ip;                                                                                    \
    lh_net_port_t port

#endif /* LH_NET_SOCKET_ADDR_FIELDS_H */
