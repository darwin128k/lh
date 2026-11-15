/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_radii_t.
 */

#ifndef LH_GL_RADII_INITIALIZER_H
#define LH_GL_RADII_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_radii_initializer(top_left, top_right, bottom_right, bottom_left)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_radii_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_radii_fields.
 *
 * @param top_left     Value for @c top_left.
 * @param top_right    Value for @c top_right.
 * @param bottom_right Value for @c bottom_right.
 * @param bottom_left  Value for @c bottom_left.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_radii_t radii = lh_gl_radii_initializer(6, 6, 0, 0);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_radii_t
 */
#define lh_gl_radii_initializer(top_left, top_right, bottom_right, bottom_left)                    \
    lh_initializer(top_left, top_right, bottom_right, bottom_left)

#endif /* LH_GL_RADII_INITIALIZER_H */
