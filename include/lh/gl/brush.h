/**
 * @file brush.h
 * @brief Fill: ::lh_gl_brush_t — the same type as ::lh_gl_paint_t.
 *
 * A fill needs nothing beyond the paint, so the brush stays an alias. An
 * outline needs a width and is its own type, ::lh_gl_pen_t.
 */

#ifndef LH_GL_BRUSH_H
#define LH_GL_BRUSH_H

#include <lh/gl/paint.h>

/**
 * @typedef lh_gl_brush_t
 * @brief A fill. Alias of ::lh_gl_paint_t.
 */
typedef lh_gl_paint_t lh_gl_brush_t;

/**
 * @def lh_gl_brush_init
 * @brief Fill @p self with the empty brush. Same as ::lh_gl_paint_init.
 */
#define lh_gl_brush_init lh_gl_paint_init

#endif /* LH_GL_BRUSH_H */
