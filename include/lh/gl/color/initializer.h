/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_color_t.
 */

#ifndef LH_GL_COLOR_INITIALIZER_H
#define LH_GL_COLOR_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_color_initializer(r, g, b, a)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_color_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_color_fields.
 *
 * @param r Value for @c r (::lh_gl_color_channel_t).
 * @param g Value for @c g (::lh_gl_color_channel_t).
 * @param b Value for @c b (::lh_gl_color_channel_t).
 * @param a Value for @c a (::lh_gl_color_channel_t).
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_color_t red = lh_gl_color_initializer(0xFF, 0x00, 0x00, 0xFF);
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_color_t
 */
#define lh_gl_color_initializer(r, g, b, a) lh_initializer(r, g, b, a)

#endif /* LH_GL_COLOR_INITIALIZER_H */
