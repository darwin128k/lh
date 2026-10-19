/**
 * @file kind.h
 * @brief What an ::lh_value_t currently holds.
 */

#ifndef LH_VALUE_KIND_H
#define LH_VALUE_KIND_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_value_kind_t
 * @brief Discriminator of ::lh_value_t. Alias for ::lh_u8_t; values are the
 *        `lh_value_kind_*` constants below.
 */
typedef lh_u8_t lh_value_kind_t;

/**
 * @def lh_value_kind_none
 * @brief No value. What ::lh_value_init leaves.
 */
#define lh_value_kind_none 0U

/**
 * @def lh_value_kind_bool
 * @brief ::lh_bool_t.
 */
#define lh_value_kind_bool 1U

/**
 * @def lh_value_kind_sint
 * @brief Signed integer (::lh_sllong_t).
 */
#define lh_value_kind_sint 2U

/**
 * @def lh_value_kind_uint
 * @brief Unsigned integer (::lh_ullong_t).
 */
#define lh_value_kind_uint 3U

/**
 * @def lh_value_kind_real
 * @brief Floating point (::lh_real_t). Never held when ::LH_REAL_ENABLED
 *        is `0`; the number stays reserved.
 */
#define lh_value_kind_real 4U

/**
 * @def lh_value_kind_str
 * @brief Text, as an ::lh_str_view_t: the value does not own it.
 */
#define lh_value_kind_str 5U

/**
 * @def lh_value_kind_ptr
 * @brief An opaque pointer (::lh_ptr) to anything; the value does not own it.
 */
#define lh_value_kind_ptr 6U

#endif /* LH_VALUE_KIND_H */
