/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_point_t.
 */

#ifndef LH_GL_POINT_INITIALIZER_H
#define LH_GL_POINT_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_point_initializer(x, y)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_point_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_point_fields, the member macro ::lh_gl_point_t is built from.
 *
 * @param x Value for @c x.
 * @param y Value for @c y.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_point_t point = lh_gl_point_initializer(3, 4);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_point_t
 */
#define lh_gl_point_initializer(x, y) lh_initializer(x, y)

#endif /* LH_GL_POINT_INITIALIZER_H */