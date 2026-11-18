/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_paint_t.
 */

#ifndef LH_GX_PAINT_FIELDS_H
#define LH_GX_PAINT_FIELDS_H

#include <lh/core/ptr.h>

/**
 * @def lh_gx_paint_fields(canvas_type, rect_type)
 * @brief The canvas and its context, the clip, and the entity's place on the screen.
 *
 * `canvas` and `canvas_context` are where the drawing goes; `clip` is the part
 * of the screen this paint may touch (the damaged area cut by every owner on the
 * way down); `place` is the rectangle of the entity being painted, in screen
 * coordinates. None of it is owned: a paint lives on the renderer's stack for
 * one call of a class's `paint`.
 *
 * @param canvas_type Type of the canvas table (::lh_gx_canvas_t).
 * @param rect_type   Type of the rectangles (::lh_gx_rect_t).
 */
#define lh_gx_paint_fields(canvas_type, rect_type)                                                 \
    const canvas_type *canvas;                                                                     \
    lh_ptr canvas_context;                                                                         \
    rect_type clip;                                                                                \
    rect_type place

#endif /* LH_GX_PAINT_FIELDS_H */
