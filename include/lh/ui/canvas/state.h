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

#include <lh/ui/canvas/state/fields.h>
#include <lh/ui/rect.h>

/**
 * @struct lh_ui_canvas_state
 * @brief Offset, and the clip when there is one.
 */
struct lh_ui_canvas_state
{
    /** Members: ::lh_ui_canvas_state_fields. */
    lh_ui_canvas_state_fields(lh_ui_point_t, lh_ui_rect_t, lh_bool_t, lh_u8_t);
};
/** @copybrief lh_ui_canvas_state */
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
lh_void
lh_ui_canvas_state_to_target(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *rect,
                             lh_ui_rect_t *out);

/**
 * @brief Add @p delta to the offset of @p self.
 */
lh_void
lh_ui_canvas_state_move(lh_ui_canvas_state_t *self, lh_ui_point_t delta);

/**
 * @brief Replace the offset of @p self with @p origin, instead of adding to it.
 *
 * The frame entry points use this to put `(0, 0)` on the buffer origin before
 * anything is pushed, so the first push of a frame lands on the same space
 * whatever came before it.
 */
lh_void
lh_ui_canvas_state_set_origin(lh_ui_canvas_state_t *self, lh_ui_point_t origin);

/**
 * @brief Cut the clip of @p self further to @p target (target space).
 */
lh_void
lh_ui_canvas_state_clip_to(lh_ui_canvas_state_t *self, const lh_ui_rect_t *target);

/**
 * @brief @p target cut to the clip of @p self (unchanged when nothing is cut).
 */
lh_void
lh_ui_canvas_state_cut(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *target,
                       lh_ui_rect_t *out);

/**
 * @brief True when @p target lies wholly inside the clip (or nothing is cut).
 */
lh_bool_t
lh_ui_canvas_state_contains(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *target);

/**
 * @brief The translation applied to what is drawn (level 1).
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_point_t *
lh_ui_canvas_state_get_offset(const lh_ui_canvas_state_t *self);

/**
 * @brief The translation applied to what is drawn, to change in place (level 1).
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_point_t *
lh_ui_canvas_state_get_offset_mut(lh_ui_canvas_state_t *self);

/**
 * @brief Store the translation applied to what is drawn (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 * @param offset The new value.
 */
lh_void
lh_ui_canvas_state_set_offset(lh_ui_canvas_state_t *self, lh_ui_point_t offset);

/**
 * @brief The clip rectangle, to change in place (level 1).
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_rect_t *
lh_ui_canvas_state_get_clip_mut(lh_ui_canvas_state_t *self);

/**
 * @brief Store the clip rectangle (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 * @param clip The new value.
 */
lh_void
lh_ui_canvas_state_set_clip(lh_ui_canvas_state_t *self, lh_ui_rect_t clip);

/**
 * @brief Whether a clip is set (level 1).
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_canvas_state_is_clipped(const lh_ui_canvas_state_t *self);

/**
 * @brief Store whether a clip is set (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 * @param clipped The new value.
 */
lh_void
lh_ui_canvas_state_set_clipped(lh_ui_canvas_state_t *self, lh_bool_t clipped);

/**
 * @brief How many rounded clips the state has (level 1).
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_ui_canvas_state_get_round_count(const lh_ui_canvas_state_t *self);

/**
 * @brief Store how many rounded clips the state has (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The state; checked by ::lh_assert_runtime_ref.
 * @param round_count The new value.
 */
lh_void
lh_ui_canvas_state_set_round_count(lh_ui_canvas_state_t *self, lh_u8_t round_count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_STATE_H */
