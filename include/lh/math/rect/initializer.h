/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_rect_t.
 */

#ifndef LH_MATH_RECT_INITIALIZER_H
#define LH_MATH_RECT_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_rect_initializer(origin, size)
 * @brief Produces a brace-enclosed initializer for ::lh_math_rect_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_rect_fields.
 *
 * @param origin Initializer of the ::lh_math_point_t @c origin (::lh_math_point_initializer).
 * @param size   Initializer of the ::lh_math_size_t @c size (::lh_math_size_initializer).
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_rect_t rect = lh_math_rect_initializer(
 *     lh_math_point_initializer(10, 20),    // origin
 *     lh_math_size_initializer(640, 480));  // size
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_rect_t
 */
#define lh_math_rect_initializer(origin, size) lh_initializer(origin, size)

#endif /* LH_MATH_RECT_INITIALIZER_H */
