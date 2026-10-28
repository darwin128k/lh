/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_math_slice_t.
 */

#ifndef LH_MATH_SLICE_INITIALIZER_H
#define LH_MATH_SLICE_INITIALIZER_H

#include <lh/null.h>

/**
 * @def lh_math_slice_initializer(first, last)
 * @brief Produces a brace-enclosed initializer for ::lh_math_slice_t.
 *
 * Expands to ::lh_interval_bounds_initializer with @p first as @c first and
 * @p last as @c second. The slice is closed: <tt>[first, last]</tt>, both ends
 * are elements of it.
 *
 * @param first Pointer to the first element, or ::lh_null.
 * @param last  Pointer to the last element, or ::lh_null.
 *
 * Example usage:
 * @code{.c}
 * static const lh_float_t data[3] = {1.0f, 2.0f, 3.0f};
 * const lh_math_slice_t slice = lh_math_slice_initializer(&data[0], &data[2]);
 * @endcode
 *
 * @see lh_math_slice_t
 * @see lh_interval_bounds_initializer
 */
#define lh_math_slice_initializer(first, last) lh_interval_bounds_initializer(first, last)

/**
 * @def lh_math_slice_empty_initializer()
 * @brief Initializer of an empty slice: both bounds ::lh_null.
 *
 * Expands to ::lh_math_slice_initializer(::lh_null, ::lh_null).
 *
 * @see lh_math_slice_initializer
 */
#define lh_math_slice_empty_initializer() lh_math_slice_initializer(lh_null, lh_null)

#endif /* LH_MATH_SLICE_INITIALIZER_H */
