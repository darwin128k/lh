/**
 * @file fn.h
 * @brief Function type ::lh_gx_canvas_fill_mask_fn.
 */

#ifndef LH_GX_CANVAS_FILL_MASK_FN_H
#define LH_GX_CANVAS_FILL_MASK_FN_H

#include <lh/gx/color.h>
#include <lh/gx/pixmap.h>
#include <lh/gx/rect.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_canvas_fill_mask_fn
 * @brief Lay @p color through @p mask, its top-left corner at @p at, inside @p clip.
 *
 * Optional. Each pixel of the mask (format `a8`) is a coverage that scales
 * the colour's alpha: a glyph, an anti-aliased edge, a rounded corner. Without
 * this slot the drawing above the canvas falls back to ::lh_gx_canvas_fill_rect_fn
 * one pixel at a time — correct, and slow.
 *
 * @param context The backend's context — where it draws.
 * @param clip    Nothing outside it is touched; screen coordinates.
 * @param mask    The coverage, format `a8`; the caller's.
 * @param at      Where the mask's top-left pixel lands; screen coordinates.
 * @param color   The colour, `0xAARRGGBB`.
 */
typedef lh_void(lh_gx_canvas_fill_mask_fn)(lh_ptr context, const lh_gx_rect_t *clip,
                                           const lh_gx_pixmap_t *mask, lh_gx_point_t at,
                                           lh_gx_color_t color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CANVAS_FILL_MASK_FN_H */
