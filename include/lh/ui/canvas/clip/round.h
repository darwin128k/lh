/**
 * @file round.h
 * @brief ::lh_ui_canvas_clip_round_t — one rounded cut.
 */

#ifndef LH_UI_CANVAS_CLIP_ROUND_H
#define LH_UI_CANVAS_CLIP_ROUND_H

#include <lh/ui/canvas/clip/fields.h>
#include <lh/ui/rect.h>

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

/**
 * @brief The rectangle of the rounded clip (level 1).
 *
 * @param self The rounded clip; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_rect_t *
lh_ui_canvas_clip_round_get_rect(const lh_ui_canvas_clip_round_t *self);

/**
 * @brief The rectangle of the rounded clip, to change in place (level 1).
 *
 * @param self The rounded clip; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_rect_t *
lh_ui_canvas_clip_round_get_rect_mut(lh_ui_canvas_clip_round_t *self);

/**
 * @brief Store the rectangle of the rounded clip (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The rounded clip; checked by ::lh_assert_runtime_ref.
 * @param rect The new value.
 */
lh_void
lh_ui_canvas_clip_round_set_rect(lh_ui_canvas_clip_round_t *self, lh_ui_rect_t rect);

/**
 * @brief The corner radius of the rounded clip (level 1).
 *
 * @param self The rounded clip; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_canvas_clip_round_get_radius(const lh_ui_canvas_clip_round_t *self);

/**
 * @brief Store the corner radius of the rounded clip (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The rounded clip; checked by ::lh_assert_runtime_ref.
 * @param radius The new value.
 */
lh_void
lh_ui_canvas_clip_round_set_radius(lh_ui_canvas_clip_round_t *self, lh_ui_scalar_t radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_CLIP_ROUND_H */
