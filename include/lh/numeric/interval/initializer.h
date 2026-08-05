/**
 * @file initializer.h
 * @brief Initializer macros for full-range intervals of numeric types.
 *
 * Provides ::lh_numeric_interval_initializer_unsigned and
 * ::lh_numeric_interval_initializer_signed — convenience wrappers around
 * ::lh_interval_initializer that combine
 * ::lh_numeric_interval_bounds_initializer_unsigned /
 * ::lh_numeric_interval_bounds_initializer_signed with a caller-supplied flags value.
 * Also provides ::lh_numeric_interval_initializer for automatic
 * signed/unsigned selection based on the type.
 */

#ifndef LH_NUMERIC_INTERVAL_INITIALIZER_H
#define LH_NUMERIC_INTERVAL_INITIALIZER_H

#include <lh/interval/initializer.h>
#include <lh/numeric/interval/bounds/initializer.h>

/**
 * @def lh_numeric_interval_initializer_unsigned(T, flags)
 * @brief Create an interval initializer spanning the full range of an unsigned
 * type.
 *
 * Expands to `lh_interval_initializer(
 * lh_numeric_interval_bounds_initializer_unsigned(T), flags)`.
 *
 * @param T     An unsigned integer type (e.g. `unsigned int`, `uint8_t`).
 * @param flags Interval flags value (e.g. `lh_interval_flags_closed`).
 *
 * Example usage:
 * @code{.c}
 * typedef struct { lh_interval_bounds_fields(unsigned int); } uint_bounds_t;
 * typedef struct { lh_interval_fields(uint_bounds_t); } uint_interval_t;
 *
 * uint_interval_t i = lh_initializer_of_struct(
 *     uint_interval_t,
 *     lh_numeric_interval_initializer_unsigned(unsigned int,
 *                                              lh_interval_flags_closed));
 * @endcode
 */
#define lh_numeric_interval_initializer_unsigned(T, flags)                                         \
    lh_interval_initializer(lh_numeric_interval_bounds_initializer_unsigned(T), flags)

/**
 * @def lh_numeric_interval_initializer_signed(T, flags)
 * @brief Create an interval initializer spanning the full range of a signed
 * type.
 *
 * Expands to `lh_interval_initializer(
 * lh_numeric_interval_bounds_initializer_signed(T), flags)`.
 *
 * @param T     A signed integer type (e.g. `int`, `int32_t`).
 * @param flags Interval flags value (e.g. `lh_interval_flags_closed`).
 *
 * Example usage:
 * @code{.c}
 * typedef struct { lh_interval_bounds_fields(int); } int_bounds_t;
 * typedef struct { lh_interval_fields(int_bounds_t); } int_interval_t;
 *
 * int_interval_t i = lh_initializer_of_struct(
 *     int_interval_t,
 *     lh_numeric_interval_initializer_signed(int,
 *                                            lh_interval_flags_closed));
 * @endcode
 */
#define lh_numeric_interval_initializer_signed(T, flags)                                           \
    lh_interval_initializer(lh_numeric_interval_bounds_initializer_signed(T), flags)

/**
 * @def lh_numeric_interval_initializer(T, flags)
 * @brief Create an interval initializer spanning the full range of integer
 * type @p T with automatic signedness handling.
 *
 * Expands to `lh_interval_initializer(
 * lh_numeric_interval_bounds_initializer(T), flags)`.
 *
 * @param T     Integer type whose full range should be used.
 * @param flags Interval flags value (e.g. `lh_interval_flags_closed`).
 */
#define lh_numeric_interval_initializer(T, flags)                                                  \
    lh_interval_initializer(lh_numeric_interval_bounds_initializer(T), flags)

#endif /* LH_NUMERIC_INTERVAL_INITIALIZER_H */
