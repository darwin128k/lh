/**
 * @file sw.h
 * @brief Software canvas backend: ::lh_ui_canvas_backend_sw over a
 *        ::lh_ui_pixmap_t.
 *
 * Draws every primitive itself, row by row, into the pixmap: straight runs
 * as one span, anti-aliased edges per pixel with the coverage from
 * `lh/ui/radius.h` (round rects) or the mask (glyphs). Needs no OS: on
 * Windows the pixmap is the DIB section a GDI context presents; on a
 * microcontroller it is the frame buffer.
 *
 * It has a `set_clip` slot, so the canvas sends primitives uncut and this
 * backend cuts each row to its limit (pixmap ∩ clip). `fill_round_rect` and
 * `fill_mask` always draw, so the canvas fallback is never taken.
 * `begin` / `end` are ::lh_null: whoever owns the pixels shows them.
 *
 * The context passed to the canvas is an ::lh_ui_canvas_sw_t.
 */

#ifndef LH_UI_CANVAS_SW_H
#define LH_UI_CANVAS_SW_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw/fields.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_ui_canvas_sw
 * @typedef lh_ui_canvas_sw_t
 * @brief Context of ::lh_ui_canvas_backend_sw.
 */
struct lh_ui_canvas_sw
{
    lh_ui_canvas_sw_fields(lh_ui_pixmap_t, lh_ui_rect_t);
};
typedef struct lh_ui_canvas_sw lh_ui_canvas_sw_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Lifetime ────────────────────────────────────────────────────────────── */

/**
 * @brief No pixmap yet: every write is cut away.
 */
lh_void
lh_ui_canvas_sw_init(lh_ui_canvas_sw_t *self);

/**
 * @brief Draw into @p pixmap (copied as a view, pixels not owned); the limit
 *        becomes its bounds, no clip.
 */
lh_void
lh_ui_canvas_sw_set_pixmap(lh_ui_canvas_sw_t *self, const lh_ui_pixmap_t *pixmap);

/**
 * @brief The pixmap @p self draws into.
 */
const lh_ui_pixmap_t *
lh_ui_canvas_sw_get_pixmap(const lh_ui_canvas_sw_t *self);

/**
 * @brief The rect every write is cut to: pixmap bounds ∩ clip.
 */
lh_ui_rect_t
lh_ui_canvas_sw_get_limit(const lh_ui_canvas_sw_t *self);

/**
 * @brief @p context of a backend call as the software context; asserts it is
 *        not ::lh_null.
 */
lh_ui_canvas_sw_t *
lh_ui_canvas_sw_from(lh_ptr context);

/* ── Cutting to the limit ────────────────────────────────────────────────── */

/**
 * @brief @p x moved right to the first column of the limit, if before it.
 */
lh_s32_t
lh_ui_canvas_sw_cut_x0(const lh_ui_canvas_sw_t *self, lh_s32_t x);

/**
 * @brief @p x moved left to one past the last column of the limit, if after it.
 */
lh_s32_t
lh_ui_canvas_sw_cut_x1(const lh_ui_canvas_sw_t *self, lh_s32_t x);

/**
 * @brief @p y moved down to the first row of the limit, if above it.
 */
lh_s32_t
lh_ui_canvas_sw_cut_y0(const lh_ui_canvas_sw_t *self, lh_s32_t y);

/**
 * @brief @p y moved up to one past the last row of the limit, if below it.
 */
lh_s32_t
lh_ui_canvas_sw_cut_y1(const lh_ui_canvas_sw_t *self, lh_s32_t y);

/* ── Rows ────────────────────────────────────────────────────────────────── */

/**
 * @brief ::lh_ui_pixmap_fill_span of `x0 .. x1 - 1` on row @p y, cut to the
 *        limit across (the row itself is the caller's to cut).
 */
lh_void
lh_ui_canvas_sw_fill_span(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Pixels `x0 .. x1 - 1` of row @p y at their coverage of the rounded
 *        @p rect (::lh_ui_radius_coverage), cut to the limit across.
 */
lh_void
lh_ui_canvas_sw_cover_span(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0,
                           lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief True when row @p y of @p rect crosses a corner of @p radius (top or
 *        bottom band): its ends need coverage.
 */
lh_bool_t
lh_ui_canvas_sw_is_corner_row(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y);

/**
 * @brief Row @p y of a corner band of the rounded @p rect: both corner spans
 *        at their coverage, one span between them.
 */
lh_void
lh_ui_canvas_sw_corner_row(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y,
                           const lh_ui_color_t *color);

/**
 * @brief Row @p y of the rounded @p rect: one span in the straight middle;
 *        in a corner band both corner spans with coverage and the span between.
 *        The same pixels as ::lh_ui_canvas_fill_round_rect_by_rects.
 */
lh_void
lh_ui_canvas_sw_round_row(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y,
                          const lh_ui_color_t *color);

/**
 * @brief Row @p y of @p mask placed with its top-left at (@p x0, @p y0): each
 *        pixel at its mask coverage, cut to the limit across.
 */
lh_void
lh_ui_canvas_sw_mask_row(lh_ui_canvas_sw_t *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0, lh_s32_t y,
                         const lh_ui_color_t *color);

/* ── Backend slots (context: ::lh_ui_canvas_sw_t) ────────────────────────── */

/**
 * @brief Backend `clear`: store @p color in the whole pixmap (the clip does
 *        not apply to clear).
 */
lh_void
lh_ui_canvas_sw_clear(lh_ptr context, const lh_ui_color_t *color);

/**
 * @brief Backend `fill_rect`: @p rect (whole pixels it touches) cut to the
 *        limit; stored when opaque, blended otherwise.
 */
lh_void
lh_ui_canvas_sw_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color);

/**
 * @brief Backend `fill_round_rect`: ::lh_ui_canvas_sw_round_row for each row
 *        of @p rect inside the limit. Always draws.
 */
lh_bool_t
lh_ui_canvas_sw_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                const lh_ui_color_t *color);

/**
 * @brief Backend `set_clip`: the limit becomes pixmap bounds ∩ @p clip, or the
 *        bounds for ::lh_null.
 */
lh_void
lh_ui_canvas_sw_set_clip(lh_ptr context, const lh_ui_rect_t *clip);

/**
 * @brief Backend `fill_mask`: ::lh_ui_canvas_sw_mask_row for each row of
 *        @p mask inside the limit. Always draws.
 */
lh_bool_t
lh_ui_canvas_sw_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                          const lh_ui_color_t *color);

/**
 * @brief The software backend table: clear, fill_rect, fill_round_rect,
 *        set_clip, fill_mask; no begin / end.
 */
extern const lh_ui_canvas_backend_t lh_ui_canvas_backend_sw;

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_SW_H */
