/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_mb_want_t and ::lh_mb_read_t.
 */

#ifndef LH_NET_MODBUS_INITIALIZER_H
#define LH_NET_MODBUS_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_mb_want_initializer(address, registers)
 * @brief Produces a brace-enclosed initializer for ::lh_mb_want_t.
 *
 * Expands to ::lh_initializer with the arguments in struct order.
 *
 * @param address   First register wanted.
 * @param registers How many from there.
 *
 * Example usage:
 * @code{.c}
 * static const lh_mb_want_t want = lh_mb_want_initializer(100, 4);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_mb_want_t
 */
#define lh_mb_want_initializer(address, registers) lh_initializer(address, registers)

/**
 * @def lh_mb_read_initializer(start, count, first, last)
 * @brief Produces a brace-enclosed initializer for ::lh_mb_read_t.
 *
 * Expands to ::lh_initializer with the arguments in struct order.
 *
 * @param start First register the request asks for.
 * @param count How many registers it asks for.
 * @param first Index into the wants this one starts at.
 * @param last  Index of the last want it covers.
 *
 * Example usage:
 * @code{.c}
 * static const lh_mb_read_t read = lh_mb_read_initializer(100, 8, 0, 1);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_mb_read_t
 */
#define lh_mb_read_initializer(start, count, first, last) lh_initializer(start, count, first, last)

#endif /* LH_NET_MODBUS_INITIALIZER_H */
