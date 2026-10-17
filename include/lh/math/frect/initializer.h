/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_frect_t.
 */

#ifndef LH_MATH_FRECT_INITIALIZER_H
#define LH_MATH_FRECT_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_frect_initializer(origin, size)
 * @brief Produces a brace-enclosed initializer for ::lh_math_frect_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_rect_fields.
 *
 * @param origin Initializer of the ::lh_math_fpoint_t @c origin (::lh_math_fpoint_initializer).
 * @param size   Initializer of the ::lh_math_fsize_t @c size (::lh_math_fsize_initializer).
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_frect_t rect = lh_math_frect_initializer(
 *     lh_math_fpoint_initializer(1.0f, 2.0f),  // origin
 *     lh_math_fsize_initializer(3.0f, 4.0f));  // size
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_frect_t
 */
#define lh_math_frect_initializer(origin, size) lh_initializer(origin, size)

#endif /* LH_MATH_FRECT_INITIALIZER_H */
