/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_mb_link_counters_t.
 */

#ifndef LH_NET_MODBUS_LINK_INITIALIZER_H
#define LH_NET_MODBUS_LINK_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_mb_link_counters_initializer(sent, done, failed, refused, bytes, us_busy)
 * @brief Produces a brace-enclosed initializer for ::lh_mb_link_counters_t.
 *
 * Expands to ::lh_initializer with the arguments in struct order.
 *
 * @param sent    Requests written to the stream.
 * @param done    Answers parsed in full.
 * @param failed  Timeouts and stream errors.
 * @param refused Answers that were Modbus exceptions.
 * @param bytes   Request and answer bytes both ways.
 * @param us_busy Microseconds requests spent in flight.
 *
 * Example usage:
 * @code{.c}
 * lh_mb_link_counters_t counters = lh_mb_link_counters_initializer(0, 0, 0, 0, 0, 0);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_mb_link_counters_t
 */
#define lh_mb_link_counters_initializer(sent, done, failed, refused, bytes, us_busy)               \
    lh_initializer(sent, done, failed, refused, bytes, us_busy)

#endif /* LH_NET_MODBUS_LINK_INITIALIZER_H */
