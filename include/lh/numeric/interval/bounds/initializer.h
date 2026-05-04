/**
 * @file initializer.h
 * @brief Initializer macros for full-range interval bounds of numeric types.
 *
 * Provides ::lh_numeric_interval_bounds_initializer_unsigned and
 * ::lh_numeric_interval_bounds_initializer_signed — convenience wrappers
 * around ::lh_interval_bounds_initializer that fill in @c first / @c second
 * (minimum / maximum) automatically from ::lh_numeric_limit_umin / ::lh_numeric_limit_umax
 * and ::lh_numeric_limit_smin / ::lh_numeric_limit_smax respectively.
 * Also provides ::lh_numeric_interval_bounds_initializer for automatic
 * signed/unsigned selection based on the type.
 */

#ifndef LH_NUMERIC_INTERVAL_BOUNDS_INITIALIZER_H
#define LH_NUMERIC_INTERVAL_BOUNDS_INITIALIZER_H

#include <lh/interval/bounds/initializer.h>
#include <lh/util/numeric.h>

/**
 * @def lh_numeric_interval_bounds_initializer_unsigned(T)
 * @brief Create a bounds initializer spanning the full range of an unsigned
 * type.
 *
 * Expands to `lh_interval_bounds_initializer(lh_numeric_limit_umin(T),
 * lh_numeric_limit_umax(T))`, i.e. `{ 0, (T)-1 }`.
 *
 * @param T An unsigned integer type (e.g. `unsigned int`, `uint8_t`).
 *
 * Example usage:
 * @code{.c}
 * struct uint_bounds { lh_interval_bounds_fields(unsigned int); };
 *
 * struct uint_bounds b = lh_initializer_of_struct(
 *     uint_bounds,
 *     lh_numeric_interval_bounds_initializer_unsigned(unsigned int));
 * @endcode
 */
#define lh_numeric_interval_bounds_initializer_unsigned(T)                                         \
    lh_interval_bounds_initializer(lh_numeric_limit_umin(T), lh_numeric_limit_umax(T))

/**
 * @def lh_numeric_interval_bounds_initializer_signed(T)
 * @brief Create a bounds initializer spanning the full range of a signed type.
 *
 * Expands to `lh_interval_bounds_initializer(lh_numeric_limit_smin(T),
 * lh_numeric_limit_smax(T))`, i.e. `{ INT_MIN, INT_MAX }` for `int`.
 *
 * @param T A signed integer type (e.g. `int`, `int32_t`).
 *
 * Example usage:
 * @code{.c}
 * struct int_bounds { lh_interval_bounds_fields(int); };
 *
 * struct int_bounds b = lh_initializer_of_struct(
 *     int_bounds,
 *     lh_numeric_interval_bounds_initializer_signed(int));
 * @endcode
 */
#define lh_numeric_interval_bounds_initializer_signed(T)                                           \
    lh_interval_bounds_initializer(lh_numeric_limit_smin(T), lh_numeric_limit_smax(T))

/**
 * @def lh_numeric_interval_bounds_initializer(T)
 * @brief Create full-range bounds for integer type @p T with automatic
 * signedness handling.
 *
 * Expands to `lh_interval_bounds_initializer(lh_numeric_limit_min(T),
 * lh_numeric_limit_max(T))`.
 *
 * @param T Integer type whose full range should be used.
 */
#define lh_numeric_interval_bounds_initializer(T)                                                  \
    lh_interval_bounds_initializer(lh_numeric_limit_min(T), lh_numeric_limit_max(T))

#endif // LH_NUMERIC_INTERVAL_BOUNDS_INITIALIZER_H
