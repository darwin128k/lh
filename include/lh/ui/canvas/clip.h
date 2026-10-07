/**
 * @file clip.h
 * @brief What a primitive is cut to: ::lh_ui_canvas_clip_t.
 *
 * A clip is a rect (the intersection of every pushed clip rect) and, on top
 * of it, the rounded rects of the clipping parents with a corner radius
 * (::lh_ui_canvas_clip_round_t). A pixel inside the rect is kept at the
 * product of its coverage in each rounded rect (::lh_ui_canvas_clip_coverage),
 * the same anti-aliased coverage a rounded fill uses (`lh/ui/radius.h`).
 *
 * The canvas builds one for the backend `set_clip` slot, and cuts with it
 * itself when the backend has no such slot. A row is split once
 * (::lh_ui_canvas_clip_split_row): pixels in the middle are wholly inside
 * every round, only the ends need per-pixel coverage.
 */

#ifndef LH_UI_CANVAS_CLIP_H
#define LH_UI_CANVAS_CLIP_H

#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/canvas/clip/fields.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

/**
 * @struct lh_ui_canvas_clip_round
 * @typedef lh_ui_canvas_clip_round_t
 * @brief One rounded cut.
 */
struct lh_ui_canvas_clip_round
{
    lh_ui_canvas_clip_round_fields(lh_ui_rect_t, lh_ui_scalar_t);
};
typedef struct lh_ui_canvas_clip_round lh_ui_canvas_clip_round_t;

/**
 * @struct lh_ui_canvas_clip
 * @typedef lh_ui_canvas_clip_t
 * @brief A clip rect and the rounded cuts on it.
 */
struct lh_ui_canvas_clip
{
    lh_ui_canvas_clip_fields(lh_ui_rect_t, lh_ui_canvas_clip_round_t, lh_u32_t);
};
typedef struct lh_ui_canvas_clip lh_ui_canvas_clip_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── One rounded cut ─────────────────────────────────────────────────────── */

/**
 * @brief @p rect (target space) with @p radius clamped to it
 *        (::lh_ui_radius_clamp).
 */
lh_void
lh_ui_canvas_clip_round_init(lh_ui_canvas_clip_round_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius);

/**
 * @brief Coverage `0..255` of pixel (@p x, @p y) by @p self
 *        (::lh_ui_radius_coverage).
 */
lh_byte_t
lh_ui_canvas_clip_round_coverage(const lh_ui_canvas_clip_round_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Narrow `*lo .. *hi` to the pixels of row @p y wholly inside @p self
 *        (::lh_ui_canvas_round_full_span).
 */
lh_void
lh_ui_canvas_clip_round_narrow_row(const lh_ui_canvas_clip_round_t *self, lh_s32_t y, lh_s32_t *lo, lh_s32_t *hi);

/* ── The clip ────────────────────────────────────────────────────────────── */

/**
 * @brief @p rect, and @p count rounded cuts at @p rounds (not copied).
 */
lh_void
lh_ui_canvas_clip_init(lh_ui_canvas_clip_t *self, const lh_ui_rect_t *rect, const lh_ui_canvas_clip_round_t *rounds,
                       lh_u32_t count);

/**
 * @brief No clip at all: an empty rect that is never read, no rounds.
 */
lh_void
lh_ui_canvas_clip_init_empty(lh_ui_canvas_clip_t *self);

/**
 * @brief The clip rect of @p self (target space).
 */
const lh_ui_rect_t *
lh_ui_canvas_clip_get_rect(const lh_ui_canvas_clip_t *self);

/**
 * @brief How many rounded cuts @p self has; `0` means a plain rect clip.
 */
lh_u32_t
lh_ui_canvas_clip_get_round_count(const lh_ui_canvas_clip_t *self);

/**
 * @brief Rounded cut @p index of @p self.
 */
const lh_ui_canvas_clip_round_t *
lh_ui_canvas_clip_get_round(const lh_ui_canvas_clip_t *self, lh_u32_t index);

/**
 * @brief Coverage `0..255` of pixel (@p x, @p y) by every rounded cut of
 *        @p self: the product (::lh_ui_radius_scale), `255` without rounds.
 *        The clip rect itself is the caller's to apply.
 */
lh_byte_t
lh_ui_canvas_clip_coverage(const lh_ui_canvas_clip_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Cut row @p y of span `x0 .. x1 - 1` into ends and middle: the middle
 *        `*mid0 .. *mid1 - 1` is wholly inside every rounded cut (coverage
 *        `255`), the ends `x0 .. *mid0 - 1` and `*mid1 .. x1 - 1` need
 *        ::lh_ui_canvas_clip_coverage per pixel. `x0 <= *mid0 <= *mid1 <= x1`.
 */
lh_void
lh_ui_canvas_clip_split_row(const lh_ui_canvas_clip_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_s32_t *mid0,
                            lh_s32_t *mid1);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_CLIP_H */
