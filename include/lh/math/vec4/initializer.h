/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_vec4_t.
 */

#ifndef LH_MATH_VEC4_INITIALIZER_H
#define LH_MATH_VEC4_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_vec4_initializer(x, y, z, w)
 * @brief Produces a brace-enclosed initializer for ::lh_math_vec4_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_vec4_fields.
 *
 * @param x Value for @c x.
 * @param y Value for @c y.
 * @param z Value for @c z.
 * @param w Value for @c w.
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_vec4_t v = lh_math_vec4_initializer(1.0f, 2.0f, 3.0f, 4.0f);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_vec4_t
 */
#define lh_math_vec4_initializer(x, y, z, w) lh_initializer(x, y, z, w)

#endif /* LH_MATH_VEC4_INITIALIZER_H */
