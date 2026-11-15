/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_pen_t.
 */

#ifndef LH_GL_PEN_INITIALIZER_H
#define LH_GL_PEN_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_pen_initializer(paint, width)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_pen_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_pen_fields.
 *
 * @param paint Initializer of the ::lh_gl_paint_t (::lh_gl_paint_initializer).
 * @param width Width of the stroke.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_pen_t pen = lh_gl_pen_initializer(
 *     lh_gl_paint_initializer(lh_gl_paint_kind_solid,
 *                             lh_gl_color_initializer(0x00, 0x00, 0x00, 0xFF),
 *                             lh_null),  // paint
 *     1);                                // width
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_pen_t
 */
#define lh_gl_pen_initializer(paint, width) lh_initializer(paint, width)

#endif /* LH_GL_PEN_INITIALIZER_H */
