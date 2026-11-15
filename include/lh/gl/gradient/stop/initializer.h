/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_gradient_stop_t.
 */

#ifndef LH_GL_GRADIENT_STOP_INITIALIZER_H
#define LH_GL_GRADIENT_STOP_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_gradient_stop_initializer(color, frac)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_gradient_stop_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_gradient_stop_fields.
 *
 * @param color Initializer of the ::lh_gl_color_t (::lh_gl_color_initializer).
 * @param frac  Position of the stop (`0..255`).
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_gradient_stop_t stop = lh_gl_gradient_stop_initializer(
 *     lh_gl_color_initializer(0xFF, 0xFF, 0xFF, 0xFF),  // color
 *     128);                                             // frac
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_gradient_stop_t
 */
#define lh_gl_gradient_stop_initializer(color, frac) lh_initializer(color, frac)

#endif /* LH_GL_GRADIENT_STOP_INITIALIZER_H */
