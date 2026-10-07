/**
 * @file round.h
 * @brief A rounded, anti-aliased fill built from `fill_rect` alone.
 *
 * What ::lh_ui_canvas_fill_round_rect uses when the backend has no
 * `fill_round_rect` (or the shape sticks out of a clip the canvas cuts by
 * itself). Straight parts go out as boxes, corner pixels one by one with
 * ::lh_ui_radius_coverage in their alpha; every pixel is sent once. All rects
 * and pixel indices here are in target space (offset already applied); each
 * box still passes the canvas clip.
 */

#ifndef LH_UI_CANVAS_ROUND_H
#define LH_UI_CANVAS_ROUND_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/color.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

struct lh_ui_canvas;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief First pixel column @p rect touches (floor of its left edge).
 */
lh_s32_t
lh_ui_canvas_round_left(const lh_ui_rect_t *rect);

/**
 * @brief First pixel row @p rect touches (floor of its top edge).
 */
lh_s32_t
lh_ui_canvas_round_top(const lh_ui_rect_t *rect);

/**
 * @brief One past the last pixel column (ceil of the right edge).
 */
lh_s32_t
lh_ui_canvas_round_right(const lh_ui_rect_t *rect);

/**
 * @brief One past the last pixel row (ceil of the bottom edge).
 */
lh_s32_t
lh_ui_canvas_round_bottom(const lh_ui_rect_t *rect);

/**
 * @brief End of the near corner zone on a pixel span `a .. b`: `min(a + zone, b)`.
 */
lh_s32_t
lh_ui_canvas_round_near_end(lh_s32_t a, lh_s32_t b, lh_s32_t zone);

/**
 * @brief Start of the far corner zone on `a .. b`, never before the near end.
 */
lh_s32_t
lh_ui_canvas_round_far_start(lh_s32_t a, lh_s32_t b, lh_s32_t zone);

/**
 * @brief True when row @p y of @p rect crosses a corner of @p radius (top or
 *        bottom band): its ends need coverage.
 */
lh_bool_t
lh_ui_canvas_round_is_corner_row(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y);

/**
 * @brief True when row @p y lies between the top and bottom pixel rows of @p rect.
 */
lh_bool_t
lh_ui_canvas_round_has_row(const lh_ui_rect_t *rect, lh_s32_t y);

/**
 * @brief The pixels `*x0 .. *x1 - 1` of row @p y wholly inside the rounded
 *        @p rect: the whole row in the straight middle, between the corner
 *        zones in a corner band, none outside the rect's rows. Every pixel in
 *        it has ::lh_ui_radius_coverage `255`.
 */
lh_void
lh_ui_canvas_round_full_span(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y, lh_s32_t *x0, lh_s32_t *x1);

/**
 * @brief Fill the pixel box (@p x, @p y, @p w, @p h); an empty box sends nothing.
 */
lh_void
lh_ui_canvas_fill_pixels(struct lh_ui_canvas *self, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                         const lh_ui_color_t *color);

/**
 * @brief Fill pixel (@p x, @p y) with @p color at its coverage of the rounded
 *        @p rect; a pixel the shape misses sends nothing.
 */
lh_void
lh_ui_canvas_fill_coverage_pixel(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                 lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_canvas_fill_coverage_pixel for pixels `x0 .. x1 - 1` of row @p y.
 */
lh_void
lh_ui_canvas_fill_coverage_span(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Both corner spans of row @p y (the straight middle is not sent).
 */
lh_void
lh_ui_canvas_fill_coverage_row(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                               lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Rows `y0 .. y1 - 1` of a corner band: the straight middle as one box,
 *        the corners pixel by pixel.
 */
lh_void
lh_ui_canvas_fill_round_band(struct lh_ui_canvas *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                             lh_s32_t y0, lh_s32_t y1, const lh_ui_color_t *color);

/**
 * @brief The whole rounded fill of @p rect: top band, middle box, bottom band.
 *
 * @p radius must already be clamped (::lh_ui_radius_clamp).
 */
lh_void
lh_ui_canvas_fill_round_rect_by_rects(struct lh_ui_canvas *self, const lh_ui_rect_t *rect,
                                      lh_ui_scalar_t radius, const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_ROUND_H */
