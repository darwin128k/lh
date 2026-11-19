/**
 * @file fn.h
 * @brief Function type ::lh_gx_canvas_fill_rect_fn.
 */

#ifndef LH_GX_CANVAS_FILL_RECT_FN_H
#define LH_GX_CANVAS_FILL_RECT_FN_H

#include <lh/gx/brush.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_canvas_fill_rect_fn
 * @brief Fill the part of @p rect inside @p clip with @p brush.
 *
 * Required. A translucent colour is laid over what is there; an opaque one
 * replaces it. Every other shape the canvas cannot draw comes down to this. A
 * gradient brush colours each pixel as the gradient laid over the brush's box
 * says (`lh/core/pixel/gradient.h`), however much of the box @p rect is; a
 * canvas that draws colours only fills with the brush's colour.
 *
 * @param context The backend's context — where it draws.
 * @param clip    Nothing outside it is touched; screen coordinates.
 * @param rect    The rectangle to fill; screen coordinates.
 * @param brush   What to fill with (::lh_gx_brush_t); the caller's.
 */
typedef lh_void(lh_gx_canvas_fill_rect_fn)(lh_ptr context, const lh_gx_rect_t *clip,
                                           const lh_gx_rect_t *rect, const lh_gx_brush_t *brush);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CANVAS_FILL_RECT_FN_H */
