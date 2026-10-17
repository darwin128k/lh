/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_math_quat_t.
 */

#ifndef LH_MATH_QUAT_INITIALIZER_H
#define LH_MATH_QUAT_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_math_quat_initializer(x, y, z, w)
 * @brief Produces a brace-enclosed initializer for ::lh_math_quat_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_quat_fields.
 *
 * @param x Value for @c x.
 * @param y Value for @c y.
 * @param z Value for @c z.
 * @param w Value for @c w.
 *
 * Example usage:
 * @code{.c}
 * static const lh_math_quat_t identity = lh_math_quat_initializer(0.0f, 0.0f, 0.0f, 1.0f);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_math_quat_t
 */
#define lh_math_quat_initializer(x, y, z, w) lh_initializer(x, y, z, w)

#endif /* LH_MATH_QUAT_INITIALIZER_H */
