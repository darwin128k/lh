/**
 * @file bool.h
 * @brief Minimal boolean type and constants.
 *
 * Provides ::lh_bool_t, ::lh_bool_false, and ::lh_bool_true
 * for use in contexts where `<stdbool.h>` is unavailable or undesirable.
 */

#ifndef LH_BOOL_H
#define LH_BOOL_H

#include <lh/byte.h>

/**
 * @def lh_bool_false
 * @brief Boolean false value (0).
 */
#define lh_bool_false 0

/**
 * @def lh_bool_true
 * @brief Boolean true value (1).
 */
#define lh_bool_true 1

/**
 * @typedef lh_bool_t
 * @brief Minimal boolean type.
 *
 * Alias for ::lh_byte_t (`unsigned char`),
 * with ::lh_bool_false (0) and ::lh_bool_true (1) as the canonical values.
 */
typedef lh_byte_t lh_bool_t;

/**
 * @def lh_bool_eq(a, b)
 * @brief `1` when @p a and @p b are both true or both false, else `0`.
 *
 * Not `a == b`: any nonzero ::lh_bool_t is true, so `2` and `1` are equal as
 * booleans even though they differ as numbers.
 */
#define lh_bool_eq(a, b) (!(a) == !(b))

#endif /* LH_BOOL_H */