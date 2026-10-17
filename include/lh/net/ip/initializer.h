/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_net_ip4_t.
 */

#ifndef LH_NET_IP_INITIALIZER_H
#define LH_NET_IP_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_net_ip4_initializer(octet0, octet1, octet2, octet3)
 * @brief Produces a brace-enclosed initializer for ::lh_net_ip4_t.
 *
 * Expands to two nested ::lh_initializer lists: the outer one is the struct,
 * the inner one its @c octets array, in dotted-decimal order.
 *
 * @param octet0 Octet 0 (`192` in `192.168.0.1`).
 * @param octet1 Octet 1 (`168` in `192.168.0.1`).
 * @param octet2 Octet 2 (`0` in `192.168.0.1`).
 * @param octet3 Octet 3 (`1` in `192.168.0.1`).
 *
 * Example usage:
 * @code{.c}
 * static const lh_net_ip4_t ip = lh_net_ip4_initializer(192, 168, 0, 1);  // 192.168.0.1
 * @endcode
 *
 * @see lh_initializer
 * @see lh_net_ip4_t
 */
#define lh_net_ip4_initializer(octet0, octet1, octet2, octet3)                                     \
    lh_initializer(lh_initializer(octet0, octet1, octet2, octet3))

#endif /* LH_NET_IP_INITIALIZER_H */
