/**
 * @file fn.h
 * @brief Function type ::lh_gx_canvas_fill_rect_fn.
 */

#ifndef LH_GX_CANVAS_FILL_RECT_FN_H
#define LH_GX_CANVAS_FILL_RECT_FN_H

#include <lh/gx/color.h>
#include <lh/gx/rect.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_canvas_fill_rect_fn
 * @brief Fill the part of @p rect inside @p clip with @p color.
 *
 * Required. A translucent colour is laid over what is there; an opaque one
 * replaces it. Every other shape the canvas cannot draw comes down to this.
 *
 * @param context The backend's context — where it draws.
 * @param clip    Nothing outside it is touched; screen coordinates.
 * @param rect    The rectangle to fill; screen coordinates.
 * @param color   The colour, `0xAARRGGBB`.
 */
typedef lh_void(lh_gx_canvas_fill_rect_fn)(lh_ptr context, const lh_gx_rect_t *clip,
                                           const lh_gx_rect_t *rect, lh_gx_color_t color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CANVAS_FILL_RECT_FN_H */
