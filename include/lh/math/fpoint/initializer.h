/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_fpoint_t.
 */

#ifndef LH_MATH_FPOINT_INITIALIZER_H
#define LH_MATH_FPOINT_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_fpoint_initializer(x, y)
 * @brief Produces a brace-enclosed initializer for ::lh_math_fpoint_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_point_fields.
 *
 * @param x Value for @c x.
 * @param y Value for @c y.
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_fpoint_t point = lh_math_fpoint_initializer(1.5f, 2.0f);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_fpoint_t
 */
#define lh_math_fpoint_initializer(x, y) lh_initializer(x, y)

#endif /* LH_MATH_FPOINT_INITIALIZER_H */
