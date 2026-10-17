/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_net_ip4_socket_addr_t.
 */

#ifndef LH_NET_SOCKET_ADDR_IP4_INITIALIZER_H
#define LH_NET_SOCKET_ADDR_IP4_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_net_ip4_socket_addr_initializer(ip, port)
 * @brief Produces a brace-enclosed initializer for ::lh_net_ip4_socket_addr_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_net_socket_addr_fields.
 *
 * @param ip   Initializer of the ::lh_net_ip4_t (::lh_net_ip4_initializer).
 * @param port Port (::lh_net_port_t).
 *
 * Example usage:
 * @code{.c}
 * static const lh_net_ip4_socket_addr_t addr = lh_net_ip4_socket_addr_initializer(
 *     lh_net_ip4_initializer(127, 0, 0, 1),  // ip
 *     502);                                  // port
 * @endcode
 *
 * @see lh_initializer
 * @see lh_net_ip4_socket_addr_t
 */
#define lh_net_ip4_socket_addr_initializer(ip, port) lh_initializer(ip, port)

#endif /* LH_NET_SOCKET_ADDR_IP4_INITIALIZER_H */
