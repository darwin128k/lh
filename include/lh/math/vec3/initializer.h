/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_vec3_t.
 */

#ifndef LH_MATH_VEC3_INITIALIZER_H
#define LH_MATH_VEC3_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_vec3_initializer(x, y, z)
 * @brief Produces a brace-enclosed initializer for ::lh_math_vec3_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_vec3_fields.
 *
 * @param x Value for @c x.
 * @param y Value for @c y.
 * @param z Value for @c z.
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_vec3_t v = lh_math_vec3_initializer(1.0f, 2.0f, 3.0f);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_vec3_t
 */
#define lh_math_vec3_initializer(x, y, z) lh_initializer(x, y, z)

#endif /* LH_MATH_VEC3_INITIALIZER_H */
