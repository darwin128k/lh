/**
 * @file mask.h
 * @brief Painting an ::lh_ui_mask_t (a glyph, an icon) on a canvas.
 *
 * ::lh_ui_canvas_fill_mask moves the mask by the offset, drops it when the
 * clip shows none of it, and sends it to the backend `fill_mask` slot when
 * the backend has one and the canvas need not cut it. Otherwise the canvas
 * paints it here, pixel by pixel through `fill_rect` with
 * ::lh_ui_color_with_coverage — the same path the anti-aliased corners take,
 * so every pixel still passes the clip.
 */

#ifndef LH_UI_CANVAS_MASK_H
#define LH_UI_CANVAS_MASK_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

struct lh_ui_canvas;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Paint @p mask in @p color with its top-left pixel at @p origin
 *        (current space).
 */
lh_void
lh_ui_canvas_fill_mask(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, lh_ui_point_t origin,
                       const lh_ui_color_t *color);

/**
 * @brief True when the backend `fill_mask` can take @p target as is: the slot
 *        exists, and the canvas need not cut it.
 */
lh_bool_t
lh_ui_canvas_can_fill_mask(const struct lh_ui_canvas *self, const lh_ui_rect_t *target);

/**
 * @brief Send @p mask over @p target to the backend `fill_mask` when it can
 *        take it (::lh_ui_canvas_can_fill_mask). True only when the slot drew.
 */
lh_bool_t
lh_ui_canvas_try_fill_mask(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, const lh_ui_rect_t *target,
                           const lh_ui_color_t *color);

/**
 * @brief Paint @p mask over @p target (target space, the mask rect already
 *        moved): the backend slot, else (no slot, or it drew nothing)
 *        ::lh_ui_canvas_fill_mask_by_pixels.
 */
lh_void
lh_ui_canvas_fill_target_mask(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, const lh_ui_rect_t *target,
                              const lh_ui_color_t *color);

/**
 * @brief Paint @p mask with its top-left pixel at (@p x0, @p y0) (target
 *        space) through `fill_rect` alone, row by row.
 */
lh_void
lh_ui_canvas_fill_mask_by_pixels(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0,
                                 const lh_ui_color_t *color);

/**
 * @brief Row @p y of ::lh_ui_canvas_fill_mask_by_pixels.
 */
lh_void
lh_ui_canvas_fill_mask_row(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0,
                           lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Pixel (@p x, @p y) of the mask at (@p x0, @p y0): @p color with the
 *        coverage in its alpha; nothing where the coverage is `0`.
 */
lh_void
lh_ui_canvas_fill_mask_pixel(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0,
                             lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_MASK_H */
