/**
 * @file canvas.h
 * @brief Where an entity tree draws: ::lh_ui_canvas_t, a backend plus context.
 *
 * Callers use begin / clear / fill_rect / end; the bound
 * ::lh_ui_canvas_backend_t does the pixels. ::lh_ui_entity_draw hands the
 * canvas to every class draw event. No style cascade, no draw-task queue.
 *
 * A small fixed stack (::LH_LIBRARY_OPTION_UI_CANVAS_DEPTH) holds an offset
 * and a clip: ::lh_ui_canvas_push / ::lh_ui_canvas_pop. The canvas adds the
 * offset to every primitive and, for a backend without `set_clip`, cuts it
 * to the clip; backends never see the offset.
 */

#ifndef LH_UI_CANVAS_H
#define LH_UI_CANVAS_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ptr.h>
#include <lh/ui/canvas/backend.h>
#include <lh/ui/canvas/fields.h>
#include <lh/ui/canvas/state.h>
#include <lh/ui/color.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/void.h>

/**
 * @struct lh_ui_canvas
 * @typedef lh_ui_canvas_t
 * @brief Active draw target: backend, context, offset / clip stack, damage.
 */
struct lh_ui_canvas
{
    lh_ui_canvas_fields(lh_ui_canvas_backend_t, lh_ptr, lh_ui_canvas_state_t, lh_u8_t, lh_ui_size_t,
                        lh_ui_rect_t, lh_bool_t);
};
typedef struct lh_ui_canvas lh_ui_canvas_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Bind @p self to @p backend and @p context. Neither is owned.
 *
 * ::lh_null @p backend makes every call a no-op until
 * ::lh_ui_canvas_set_backend. ::lh_ui_canvas_backend_null says the same
 * thing explicitly. Starts with offset `(0, 0)`, no clip, nothing pushed.
 */
lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend, lh_ptr context);

/**
 * @brief Drop the backend and context of @p self.
 */
lh_void
lh_ui_canvas_deinit(lh_ui_canvas_t *self);

/**
 * @brief Replace the backend of @p self. Not owned.
 */
lh_void
lh_ui_canvas_set_backend(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend);

/**
 * @brief Backend of @p self, or ::lh_null.
 */
const lh_ui_canvas_backend_t *
lh_ui_canvas_get_backend(const lh_ui_canvas_t *self);

/**
 * @brief Replace the context of @p self.
 */
lh_void
lh_ui_canvas_set_context(lh_ui_canvas_t *self, lh_ptr context);

/**
 * @brief Context of @p self.
 */
lh_ptr
lh_ui_canvas_get_context(const lh_ui_canvas_t *self);

/**
 * @brief Target size of @p self (for `clear` damage). Zero until set.
 */
lh_void
lh_ui_canvas_set_size(lh_ui_canvas_t *self, lh_ui_size_t size);

/**
 * @brief Target size of @p self.
 */
lh_ui_size_t
lh_ui_canvas_get_size(const lh_ui_canvas_t *self);

/**
 * @brief Union @p rect (target space) into the accumulated damage of @p self.
 *
 * Empty @p rect is ignored. The one place damage is recorded.
 */
lh_void
lh_ui_canvas_add_damage(lh_ui_canvas_t *self, const lh_ui_rect_t *rect);

/**
 * @brief Drop the accumulated damage of @p self.
 */
lh_void
lh_ui_canvas_reset_damage(lh_ui_canvas_t *self);

/**
 * @brief True when @p self has accumulated damage.
 */
lh_bool_t
lh_ui_canvas_has_damage(const lh_ui_canvas_t *self);

/**
 * @brief Accumulated damage of @p self, or ::lh_null when none.
 */
const lh_ui_rect_t *
lh_ui_canvas_get_damage(const lh_ui_canvas_t *self);

/**
 * @brief Save the offset and clip of @p self, then move and cut later draws.
 *
 * The new offset is the old one plus @p offset_delta. @p clip_rect is in the
 * current (pre-push) space, like every rect handed to a primitive; it is
 * moved by the old offset and intersected with the old clip. ::lh_null
 * @p clip_rect keeps the old clip. When the clip changes, the backend
 * `set_clip` slot (if any) gets the new one.
 *
 * More than ::LH_LIBRARY_OPTION_UI_CANVAS_DEPTH pushes without a pop fail a
 * runtime assertion.
 */
lh_void
lh_ui_canvas_push(lh_ui_canvas_t *self, lh_ui_point_t offset_delta, const lh_ui_rect_t *clip_rect);

/**
 * @brief Restore the offset and clip saved by the matching ::lh_ui_canvas_push.
 *
 * Popping with nothing pushed fails a runtime assertion.
 */
lh_void
lh_ui_canvas_pop(lh_ui_canvas_t *self);

/**
 * @brief Offset @p self adds to every primitive (sum of the pushed deltas).
 */
lh_ui_point_t
lh_ui_canvas_get_offset(const lh_ui_canvas_t *self);

/**
 * @brief Clip of @p self in target space, or ::lh_null when nothing is cut.
 */
const lh_ui_rect_t *
lh_ui_canvas_get_clip(const lh_ui_canvas_t *self);

/**
 * @brief True when @p self has a clip and its backend has no `set_clip`, so
 *        the canvas cuts primitives itself.
 */
lh_bool_t
lh_ui_canvas_is_cutting(const lh_ui_canvas_t *self);

/**
 * @brief Hand the current clip to the backend `set_clip` (if it has one).
 */
lh_void
lh_ui_canvas_send_clip(lh_ui_canvas_t *self);

/**
 * @brief ::lh_ui_canvas_send_clip when the clip differs from @p before.
 */
lh_void
lh_ui_canvas_sync_clip(lh_ui_canvas_t *self, const lh_ui_canvas_state_t *before);

/**
 * @brief Save the current offset and clip on the stack (first half of push).
 */
lh_void
lh_ui_canvas_save(lh_ui_canvas_t *self);

/**
 * @brief Cut the current clip to @p clip_rect (pre-offset space);
 *        ::lh_null leaves it.
 */
lh_void
lh_ui_canvas_clip_to(lh_ui_canvas_t *self, const lh_ui_rect_t *clip_rect);

/**
 * @brief Start a frame.
 */
lh_void
lh_ui_canvas_begin(lh_ui_canvas_t *self);

/**
 * @brief Finish a frame.
 */
lh_void
lh_ui_canvas_end(lh_ui_canvas_t *self);

/**
 * @brief Fill the whole target with @p color. Ignores the offset and clip.
 */
lh_void
lh_ui_canvas_clear(lh_ui_canvas_t *self, const lh_ui_color_t *color);

/**
 * @brief Fill a rect already in target space: cut to the clip when the canvas
 *        cuts, nothing sent when nothing is left. The one place that sends
 *        `fill_rect`.
 */
lh_void
lh_ui_canvas_fill_target_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *target, const lh_ui_color_t *color);

/**
 * @brief True when some of @p target (target space) is left inside the clip;
 *        always true when nothing is cut. Lets a caller skip work early.
 */
lh_bool_t
lh_ui_canvas_shows(const lh_ui_canvas_t *self, const lh_ui_rect_t *target);

/**
 * @brief ::lh_ui_canvas_shows for @p rect in the current (offset) space.
 */
lh_bool_t
lh_ui_canvas_shows_rect(const lh_ui_canvas_t *self, const lh_ui_rect_t *rect);

/**
 * @brief Fill @p rect with the solid @p color.
 *
 * @p rect is moved by the offset; without a backend `set_clip` it is cut to
 * the clip here, and nothing is sent when nothing is left.
 */
lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_color_t *color);

/**
 * @brief Fill @p rect with @p color, corners rounded by @p radius.
 *
 * The one place a radius is normalized: it is clamped with
 * ::lh_ui_radius_clamp, `0` falls back to ::lh_ui_canvas_fill_rect, and a
 * backend without `fill_round_rect` gets the anti-aliased shape drawn here
 * through `fill_rect` (edge pixels carry the coverage in their alpha).
 * @p rect is moved by the offset. Without a backend `set_clip`, a shape that
 * sticks out of the clip goes through that fallback, which is cut pixel-exact
 * like ::lh_ui_canvas_fill_rect.
 */
lh_void
lh_ui_canvas_fill_round_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                             const lh_ui_color_t *color);

/**
 * @brief True when the backend `fill_round_rect` can take @p target as is:
 *        the slot exists, and the canvas need not cut it.
 */
lh_bool_t
lh_ui_canvas_can_fill_round(const lh_ui_canvas_t *self, const lh_ui_rect_t *target);

/**
 * @brief Send a rounded fill of @p target to the backend `fill_round_rect`
 *        when it can take it (::lh_ui_canvas_can_fill_round). True only when
 *        the slot drew.
 */
lh_bool_t
lh_ui_canvas_try_fill_round(lh_ui_canvas_t *self, const lh_ui_rect_t *target, lh_ui_scalar_t radius,
                            const lh_ui_color_t *color);

/**
 * @brief Rounded fill of a target-space rect with an already clamped radius:
 *        `0` → ::lh_ui_canvas_fill_target_rect, else the backend slot, else
 *        (no slot, or it drew nothing) ::lh_ui_canvas_fill_round_rect_by_rects.
 */
lh_void
lh_ui_canvas_fill_target_round_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *target, lh_ui_scalar_t radius,
                                    const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_H */
