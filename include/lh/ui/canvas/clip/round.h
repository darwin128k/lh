/**
 * @file round.h
 * @brief ::lh_ui_canvas_clip_round_t — one rounded cut.
 */

#ifndef LH_UI_CANVAS_CLIP_ROUND_H
#define LH_UI_CANVAS_CLIP_ROUND_H

#include <lh/ui/canvas/clip/fields.h>
#include <lh/ui/rect.h>
#include <lh/compiler/extern/c.h>

/**
 * @struct lh_ui_canvas_clip_round
 * @brief One rounded cut.
 */
struct lh_ui_canvas_clip_round
{
    /** Members: ::lh_ui_canvas_clip_round_fields. */
    lh_ui_canvas_clip_round_fields(lh_ui_rect_t, lh_ui_scalar_t);
};
/** @copybrief lh_ui_canvas_clip_round */
typedef struct lh_ui_canvas_clip_round lh_ui_canvas_clip_round_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p rect (target space) with @p radius clamped to it
 *        (::lh_ui_radius_clamp).
 */
lh_void
lh_ui_canvas_clip_round_init(lh_ui_canvas_clip_round_t *self, const lh_ui_rect_t *rect,
                             lh_ui_scalar_t radius);

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
lh_ui_canvas_clip_round_narrow_row(const lh_ui_canvas_clip_round_t *self, lh_s32_t y, lh_s32_t *lo,
                                   lh_s32_t *hi);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_CLIP_ROUND_H */
