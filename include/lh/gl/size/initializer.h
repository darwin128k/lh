/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_size_t.
 */

#ifndef LH_GL_SIZE_INITIALIZER_H
#define LH_GL_SIZE_INITIALIZER_H

#include <lh/gl/point/initializer.h>

/**
 * @def lh_gl_size_initializer(width, height)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_size_t.
 *
 * A size is a point, so this is ::lh_gl_point_initializer with the width in the
 * place of `x` and the height in the place of `y`.
 *
 * @param width Value for the width.
 * @param height Value for the height.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_size_t size = lh_gl_size_initializer(640, 480);
 * @endcode
 *
 * @see lh_gl_point_initializer
 * @see lh_gl_size_t
 */
#define lh_gl_size_initializer(width, height) lh_gl_point_initializer(width, height)

#endif /* LH_GL_SIZE_INITIALIZER_H */