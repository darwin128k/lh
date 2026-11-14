/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_rect_t.
 */

#ifndef LH_GL_RECT_INITIALIZER_H
#define LH_GL_RECT_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_rect_initializer(origin, size)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_rect_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_math_rect_fields, the member macro ::lh_gl_rect_t is built from.
 *
 * @param origin Initializer of the ::lh_gl_point_t @c origin (::lh_gl_point_initializer).
 * @param size   Initializer of the ::lh_gl_size_t @c size (::lh_gl_size_initializer).
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_rect_t rect = lh_gl_rect_initializer(
 *     lh_gl_point_initializer(10, 20),    // origin
 *     lh_gl_size_initializer(640, 480));  // size
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_rect_t
 */
#define lh_gl_rect_initializer(origin, size) lh_initializer(origin, size)

#endif /* LH_GL_RECT_INITIALIZER_H */