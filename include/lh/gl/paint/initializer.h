/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_paint_t.
 */

#ifndef LH_GL_PAINT_INITIALIZER_H
#define LH_GL_PAINT_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_paint_initializer(kind, color, gradient)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_paint_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_paint_fields.
 *
 * @param kind     Paint kind (::lh_gl_paint_kind_t).
 * @param color    Initializer of the ::lh_gl_color_t (::lh_gl_color_initializer).
 * @param gradient `const lh_gl_gradient_t *`, or ::lh_null.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_paint_t paint = lh_gl_paint_initializer(
 *     lh_gl_paint_kind_solid,                           // kind
 *     lh_gl_color_initializer(0x20, 0x40, 0x80, 0xFF),  // color
 *     lh_null);                                         // gradient
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_paint_t
 * @see lh_gl_paint_init
 */
#define lh_gl_paint_initializer(kind, color, gradient) lh_initializer(kind, color, gradient)

#endif /* LH_GL_PAINT_INITIALIZER_H */
