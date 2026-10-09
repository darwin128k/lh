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
#include <lh/size.h>
#include <lh/ui/canvas/backend.h>
#include <lh/ui/canvas/clip.h>
#include <lh/ui/canvas/fields.h>
#include <lh/ui/canvas/state.h>
#include <lh/ui/color.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/rects.h>
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
                        lh_ui_point_t, lh_ui_rect_t, lh_bool_t, lh_ui_canvas_clip_round_t, lh_u8_t,
                        lh_usize_t, lh_ui_rects_t);
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
 * @brief @p damage clipped to @p area; empty where the two do not meet.
 *
 * ::lh_null @p damage is the whole target and answers @p area. Whether a piece of
 * the target still holds what was drawn there is the one question every frame asks
 * of every part of itself, so it is asked once here, beside the damage the canvas
 * keeps, and not again by whoever walks the target.
 */
lh_ui_rect_t
lh_ui_canvas_damage_in(const lh_ui_rect_t *area, const lh_ui_rect_t *damage);

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
 * @brief ::lh_ui_canvas_push that also cuts later draws to @p clip_rect with
 *        rounded corners of @p radius (clamped to it, ::lh_ui_radius_clamp),
 *        on top of every rounded cut already pushed.
 *
 * Radius `0` or a ::lh_null @p clip_rect is a plain ::lh_ui_canvas_push.
 */
lh_void
lh_ui_canvas_push_round(lh_ui_canvas_t *self, lh_ui_point_t offset_delta, const lh_ui_rect_t *clip_rect,
                        lh_ui_scalar_t radius);

/**
 * @brief Add the rounded cut @p clip_rect (pre-offset space) with @p radius to
 *        the current state; nothing for ::lh_null or radius `<= 0`.
 */
lh_void
lh_ui_canvas_add_round(lh_ui_canvas_t *self, const lh_ui_rect_t *clip_rect, lh_ui_scalar_t radius);

/**
 * @brief The current clip of @p self as @p out (rect and active rounds), or
 *        ::lh_null when nothing is cut.
 */
const lh_ui_canvas_clip_t *
lh_ui_canvas_describe_clip(const lh_ui_canvas_t *self, lh_ui_canvas_clip_t *out);

/**
 * @brief How many rounded cuts are active.
 */
lh_u32_t
lh_ui_canvas_get_round_count(const lh_ui_canvas_t *self);

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
 * @brief Start a frame that covers @p area only, so the backend can keep a
 *        buffer no larger than that.
 *
 * @p area is in target space, on whole pixels. Every primitive, clip and
 * rounded cut the backend sees afterwards is already moved so that its
 * `(0, 0)` is the top-left of @p area; `end` presents it back there. Damage
 * (:lh_ui_canvas_get_damage) stays in target space either way.
 *
 * Falls back to ::lh_ui_canvas_begin when the backend has no
 * ::lh_ui_canvas_begin_area_fn: the frame is then the whole target and the
 * drawing is unchanged.
 */
lh_void
lh_ui_canvas_begin_area(lh_ui_canvas_t *self, const lh_ui_rect_t *area);

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
 * @brief Send the one-row box `x0 .. x1 - 1` of row @p y to the backend
 *        `fill_rect` as is (no offset, no cut); nothing when empty.
 */
lh_void
lh_ui_canvas_send_box(lh_ui_canvas_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Send pixel (@p x, @p y) with @p color at its @p clip coverage
 *        (::lh_ui_canvas_clip_coverage); nothing where it is `0`.
 */
lh_void
lh_ui_canvas_send_clip_pixel(lh_ui_canvas_t *self, const lh_ui_canvas_clip_t *clip, lh_s32_t x, lh_s32_t y,
                             const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_canvas_send_clip_pixel for `x0 .. x1 - 1` of row @p y.
 */
lh_void
lh_ui_canvas_send_clip_pixels(lh_ui_canvas_t *self, const lh_ui_canvas_clip_t *clip, lh_s32_t x0, lh_s32_t x1,
                              lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Row @p y of `x0 .. x1 - 1` under the rounded cuts of @p clip: the
 *        middle as one box, the ends per pixel (::lh_ui_canvas_clip_split_row).
 */
lh_void
lh_ui_canvas_send_clip_row(lh_ui_canvas_t *self, const lh_ui_canvas_clip_t *clip, lh_s32_t x0, lh_s32_t x1,
                           lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_canvas_send_clip_row for every row of @p cut (already cut to
 *        the clip rect, target space).
 */
lh_void
lh_ui_canvas_send_clip_rows(lh_ui_canvas_t *self, const lh_ui_rect_t *cut, const lh_ui_color_t *color);

/**
 * @brief Send @p cut (cut to the clip rect) when the canvas cuts itself: one
 *        `fill_rect`, or rows under the rounded cuts when there are any.
 */
lh_void
lh_ui_canvas_send_cut(lh_ui_canvas_t *self, const lh_ui_rect_t *cut, const lh_ui_color_t *color);

/**
 * @brief True when the clip leaves @p target whole: no clip at all, or @p target
 *        inside a plain (unrounded) clip rect.
 *
 * The bar for a slot that *reads* its target rather than drawing it —
 * ::lh_ui_canvas_blur and ::lh_ui_canvas_glass, which need the pixels they are
 * about to work on to be in the buffer. It says nothing about who does the
 * clipping: a backend with a `set_clip` slot is not exempt from it, because a
 * clip that cuts the target means those pixels are not in the buffer at all.
 * A slot that draws a shape may rely on the backend's own clip instead; see
 * ::lh_ui_canvas_can_fill_round.
 */
lh_bool_t
lh_ui_canvas_can_send_whole(const lh_ui_canvas_t *self, const lh_ui_rect_t *target);

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

/* ── Effects ─────────────────────────────────────────────────────────────── */

/**
 * @brief Paint @p shadow behind @p rect (corners @p radius), anti-aliased.
 *
 * The three effects are here rather than on the pixmap because a shadow is
 * something the frame draws *into*, and the canvas is what a frame draws
 * through; ::lh/ui/shadow.h holds the description and the geometry, and this is
 * the call that puts it down. @p rect is moved by the offset like any other
 * primitive, and the shadow may reach past it by a spread.
 *
 * @return True when something was painted. A backend with no `shadow` slot gets
 *         the shadow drawn here, pixel by pixel through `fill_rect` with the
 *         coverage in the alpha — the same way a mask is drawn without its slot.
 *         The shadow is the box and ::lh_ui_shadow_t alone, so a frame drawn in
 *         strips paints it strip by strip and the picture does not change; a
 *         shadow that is nowhere near the clip paints nothing and says so.
 */
lh_bool_t
lh_ui_canvas_shadow(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                    const lh_ui_shadow_t *shadow);

/**
 * @brief The pixel-by-pixel shadow ::lh_ui_canvas_shadow falls back on.
 *
 * @p bounds is the shadow's own box (the box @p target grown by the outset), and
 * only the part of it the clip lets through is touched. Public because the canvas
 * hands it to a backend that has no `shadow` slot and wants the same pixels.
 */
lh_void
lh_ui_canvas_shadow_fallback(lh_ui_canvas_t *self, const lh_ui_rect_t *bounds, const lh_ui_rect_t *target,
                             lh_ui_scalar_t radius, const lh_ui_shadow_t *shadow);

/**
 * @brief Soften what is already on @p rect of the target, @p blur_radius out.
 *
 * Reads the target, so this is the one primitive with no fallback: a backend
 * without a `blur` slot cannot do it, and the canvas returns ::lh_bool_false
 * rather than drawing something that only looks blurred. It is also the one that
 * needs a buffer of its own (::lh_ui_canvas_set_scratch) and the one that needs
 * @p rect whole: the pixels it is about to blur have to be in the buffer, and in a
 * frame drawn in strips they are not there across the strip line.
 *
 * @return True when the target was blurred.
 */
lh_bool_t
lh_ui_canvas_blur(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius);

/**
 * @brief A glass panel over @p rect: blur what is behind it by @p blur_radius,
 *        then lay @p tint over it inside corners rounded by @p corner.
 *
 * @return True when the panel was drawn; false without a `glass` slot, and when
 *         there is nothing to draw.
 */
lh_bool_t
lh_ui_canvas_glass(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t corner,
                   lh_ui_scalar_t blur_radius, const lh_ui_color_t *tint);

/**
 * @brief Memory the effects that read pixels may use as a second buffer.
 *
 * Not owned, never allocated and never freed: a frame is not the place to reach
 * for a heap, and an app with no allocator under it would get a fault instead of
 * a "no". @p bytes is how much of @p scratch there is; a blur needs
 * ::lh_ui_blur_scratch_size of the rect it is given and is not drawn when it was
 * given less. With no scratch at all (the default), ::lh_ui_canvas_blur and
 * ::lh_ui_canvas_glass return ::lh_bool_false.
 */
lh_void
lh_ui_canvas_set_scratch(lh_ui_canvas_t *self, lh_u8_t *scratch, lh_usize_t bytes);

/**
 * @brief Scratch of @p self, or null when it has none.
 */
lh_u8_t *
lh_ui_canvas_get_scratch(const lh_ui_canvas_t *self);

/**
 * @brief Bytes of scratch @p self was given.
 */
lh_usize_t
lh_ui_canvas_get_scratch_bytes(const lh_ui_canvas_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_H */
