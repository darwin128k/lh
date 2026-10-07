/**
 * @file state.h
 * @brief One level of the canvas offset / clip stack: ::lh_ui_canvas_state_t.
 *
 * Kept inside ::lh_ui_canvas_t and changed by ::lh_ui_canvas_push /
 * ::lh_ui_canvas_pop through the functions below. Every rect handed in is in
 * the space before the offset; `clip` and results are in target space.
 */

#ifndef LH_UI_CANVAS_STATE_H
#define LH_UI_CANVAS_STATE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/canvas/state/fields.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

/**
 * @struct lh_ui_canvas_state
 * @typedef lh_ui_canvas_state_t
 * @brief Offset, and the clip when there is one.
 */
struct lh_ui_canvas_state
{
    lh_ui_canvas_state_fields(lh_ui_point_t, lh_ui_rect_t, lh_bool_t, lh_u8_t);
};
typedef struct lh_ui_canvas_state lh_ui_canvas_state_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Offset `(0, 0)`, no clip.
 */
lh_void
lh_ui_canvas_state_init(lh_ui_canvas_state_t *self);

/**
 * @brief Clip of @p self, or ::lh_null when nothing is cut.
 */
const lh_ui_rect_t *
lh_ui_canvas_state_get_clip(const lh_ui_canvas_state_t *self);

/**
 * @brief True when @p a and @p b cut the same way (both off, or same rect
 *        and the same number of rounded cuts).
 */
lh_bool_t
lh_ui_canvas_state_has_same_clip(const lh_ui_canvas_state_t *a, const lh_ui_canvas_state_t *b);

/**
 * @brief @p rect moved by the offset of @p self: target space.
 */
lh_ui_rect_t
lh_ui_canvas_state_to_target(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *rect);

/**
 * @brief Add @p delta to the offset of @p self.
 */
lh_void
lh_ui_canvas_state_move(lh_ui_canvas_state_t *self, lh_ui_point_t delta);

/**
 * @brief Cut the clip of @p self further to @p target (target space).
 */
lh_void
lh_ui_canvas_state_clip_to(lh_ui_canvas_state_t *self, const lh_ui_rect_t *target);

/**
 * @brief @p target cut to the clip of @p self (unchanged when nothing is cut).
 */
lh_ui_rect_t
lh_ui_canvas_state_cut(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *target);

/**
 * @brief True when @p target lies wholly inside the clip (or nothing is cut).
 */
lh_bool_t
lh_ui_canvas_state_contains(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *target);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_STATE_H */
