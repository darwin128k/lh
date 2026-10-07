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
 * backend cuts each row: to its limit (pixmap ∩ clip rect), then to the
 * rounded cuts of the clip (::lh_ui_canvas_clip_split_row — the middle as a
 * span, the ends at ::lh_ui_canvas_clip_coverage). Coverage is applied in
 * the same order as the canvas fallback, so both give the same pixels.
 * `fill_round_rect` and `fill_mask` always draw. `begin` / `end` are
 * ::lh_null: whoever owns the pixels shows them.
 *
 * The context passed to the canvas is an ::lh_ui_canvas_sw_t.
 */

#ifndef LH_UI_CANVAS_SW_H
#define LH_UI_CANVAS_SW_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/clip.h>
#include <lh/ui/canvas/sw/fields.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/radius.h>
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
    lh_ui_canvas_sw_fields(lh_ui_pixmap_t, lh_ui_rect_t, lh_ui_canvas_clip_t);
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
 * @brief The rect every write is cut to: pixmap bounds ∩ clip rect.
 */
lh_ui_rect_t
lh_ui_canvas_sw_get_limit(const lh_ui_canvas_sw_t *self);

/**
 * @brief True when the clip of @p self has rounded cuts.
 */
lh_bool_t
lh_ui_canvas_sw_is_rounded(const lh_ui_canvas_sw_t *self);

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

/* ── Pixels and rows ─────────────────────────────────────────────────────── */

/**
 * @brief Alpha a shape edge pixel is painted with: @p alpha at the shape
 *        @p coverage, then at the clip coverage when the clip is rounded —
 *        the order the canvas fallback rounds in.
 */
lh_byte_t
lh_ui_canvas_sw_edge_alpha(const lh_ui_canvas_sw_t *self, lh_byte_t alpha, lh_byte_t coverage, lh_s32_t x, lh_s32_t y);

/**
 * @brief Paint `x0 .. x1 - 1` of row @p y (at most ::LH_UI_PIXMAP_RUN, cut
 *        already) with @p color: @p coverage holds each pixel's shape
 *        coverage and is turned into its alpha in place
 *        (::lh_ui_canvas_sw_edge_alpha), then one row kernel blends the run.
 */
lh_void
lh_ui_canvas_sw_blend_run(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color,
                          lh_byte_t *coverage);

/**
 * @brief Paint `x0 .. x1 - 1` of row @p y with @p color at its clip coverage
 *        only (the ends of a span under rounded cuts).
 */
lh_void
lh_ui_canvas_sw_clip_pixels(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                            const lh_ui_color_t *color);

/**
 * @brief Paint `x0 .. x1 - 1` of row @p y, cut to the limit across (the row
 *        itself is the caller's to cut): one ::lh_ui_pixmap_fill_span, or
 *        under rounded cuts its middle as a span and its ends per pixel.
 */
lh_void
lh_ui_canvas_sw_fill_span(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_canvas_sw_fill_span on rows `y0 .. y1 - 1`.
 */
lh_void
lh_ui_canvas_sw_fill_rows(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                          const lh_ui_color_t *color);

/**
 * @brief One run (at most ::LH_UI_PIXMAP_RUN, cut already) of
 *        ::lh_ui_canvas_sw_cover_span.
 */
lh_void
lh_ui_canvas_sw_cover_run(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0,
                          lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Pixels `x0 .. x1 - 1` of row @p y at their coverage of the rounded
 *        @p rect (::lh_ui_radius_coverage), cut to the limit across, in runs.
 */
lh_void
lh_ui_canvas_sw_cover_span(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0,
                           lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief One run (at most ::LH_UI_PIXMAP_RUN, cut already) of
 *        ::lh_ui_canvas_sw_cover_span, against an already prepared
 *        ::lh_ui_radius_run.
 */
lh_void
lh_ui_canvas_sw_cover_run_row(lh_ui_canvas_sw_t *self, const struct lh_ui_radius_run *run, lh_s32_t x0, lh_s32_t x1,
                              lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_canvas_sw_cover_span against an already prepared
 *        ::lh_ui_radius_run, so a run of rows pays for the rect's fixed-point
 *        edges once instead of once per row.
 */
lh_void
lh_ui_canvas_sw_cover_span_run(lh_ui_canvas_sw_t *self, const struct lh_ui_radius_run *run, lh_s32_t x0, lh_s32_t x1,
                               lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Rows `y0 .. y1 - 1` of the rounded @p rect: the span wholly inside it
 *        painted, the corner pixels either side at their coverage.
 *
 * A band, not a row, because that is the shape of the work: the arc's square
 * root and both corner spans change every row, but the rect's fixed-point edges
 * do not, and the corner rows are always several of them together. The same
 * pixels as ::lh_ui_canvas_sw_round_row called per row.
 */
lh_void
lh_ui_canvas_sw_round_band(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y0,
                           lh_s32_t y1, const lh_ui_color_t *color);

/**
 * @brief Row @p y of the rounded @p rect: the span wholly inside it
 *        (::lh_ui_canvas_round_full_span) painted, the corner pixels either
 *        side at their coverage. The same pixels as
 *        ::lh_ui_canvas_fill_round_rect_by_rects.
 */
lh_void
lh_ui_canvas_sw_round_row(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y,
                          const lh_ui_color_t *color);

/**
 * @brief One run of ::lh_ui_canvas_sw_mask_row: pixmap `x0 .. x1 - 1` of row
 *        @p y from mask pixel (@p mx, @p my) on.
 */
lh_void
lh_ui_canvas_sw_mask_run(lh_ui_canvas_sw_t *self, const lh_ui_mask_t *mask, lh_s32_t mx, lh_s32_t my, lh_s32_t x0,
                         lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

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
 *        limit; one box without rounded cuts, rows under them.
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
 * @brief Backend `set_clip`: the limit becomes pixmap bounds ∩ the clip rect
 *        and the rounded cuts are kept; ::lh_null clears both.
 */
lh_void
lh_ui_canvas_sw_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip);

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
