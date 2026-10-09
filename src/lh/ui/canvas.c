/**
 * @file canvas.c
 * @brief Implementation of `lh/ui/canvas.h` and the null backend.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/radius.h>
#include <lh/ui/rects.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

/* ── Null backend ────────────────────────────────────────────────────────── */

const lh_ui_canvas_backend_t lh_ui_canvas_backend_null = {
    lh_null, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null, lh_null};

/* ── Lifetime ────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
    self->context = context;
    lh_ui_canvas_state_init(lh_addr_of(self->state));
    self->depth = 0U;
    lh_ui_size_init(lh_addr_of(self->size), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_point_init(lh_addr_of(self->frame_at), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_rect_init_empty(lh_addr_of(self->damage));
    self->has_damage = lh_bool_false;
    lh_ui_rects_init(lh_addr_of(self->drawn));
    self->scratch = lh_null;
    self->scratch_bytes = 0U;
}

lh_void
lh_ui_canvas_set_scratch(lh_ui_canvas_t *self, lh_u8_t *scratch, lh_usize_t bytes)
{
    lh_assert_runtime_ref(self);
    self->scratch = scratch;
    self->scratch_bytes = lh_null_eq(scratch) ? 0U : bytes;
}

lh_u8_t *
lh_ui_canvas_get_scratch(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch;
}

lh_usize_t
lh_ui_canvas_get_scratch_bytes(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch_bytes;
}

lh_void
lh_ui_canvas_deinit(lh_ui_canvas_t *self)
{
    lh_ui_canvas_init(self, lh_null, lh_null);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_set_backend(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
}

const lh_ui_canvas_backend_t *
lh_ui_canvas_get_backend(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->backend;
}

lh_void
lh_ui_canvas_set_context(lh_ui_canvas_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_ptr
lh_ui_canvas_get_context(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_ui_canvas_set_size(lh_ui_canvas_t *self, lh_ui_size_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

lh_ui_size_t
lh_ui_canvas_get_size(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_void
lh_ui_canvas_add_damage(lh_ui_canvas_t *self, const lh_ui_rect_t *rect)
{
    /* A primitive reaches the backend already moved by the frame origin, so the
       rect given here is still in the buffer. Put it back: the damage union is
       in target space, where the caller drew it, and stays that way whichever
       part of the target this frame covers. */
    const lh_ui_rect_t drawn = lh_ui_rect_offset(rect, lh_ui_point_get_x(lh_addr_of(self->frame_at)),
                                                 lh_ui_point_get_y(lh_addr_of(self->frame_at)));

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(drawn)));
    if (!self->has_damage)
    {
        self->damage = drawn;
        self->has_damage = lh_bool_true;
        return;
    }
    self->damage = lh_ui_rect_union(lh_addr_of(self->damage), lh_addr_of(drawn));
}

lh_void
lh_ui_canvas_reset_damage(lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_rect_init_empty(lh_addr_of(self->damage));
    self->has_damage = lh_bool_false;
}

lh_bool_t
lh_ui_canvas_has_damage(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->has_damage;
}

const lh_ui_rect_t *
lh_ui_canvas_get_damage(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->has_damage ? lh_addr_of(self->damage) : lh_null;
}

lh_ui_rect_t
lh_ui_canvas_damage_in(const lh_ui_rect_t *area, const lh_ui_rect_t *damage)
{
    lh_ui_rect_t part;

    lh_assert_runtime_ref(area);
    /* No damage is the whole target: nothing was said, so everything still holds. */
    if (lh_null_eq(damage))
    {
        return *area;
    }
    part = lh_ui_rect_intersection(area, damage);
    return part;
}

/* ── Offset and clip ─────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_canvas_is_cutting(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(!self->state.clipped, lh_bool_false);
    return lh_null_eq(self->backend) || lh_null_eq(self->backend->set_clip) ? lh_bool_true : lh_bool_false;
}

const lh_ui_canvas_clip_t *
lh_ui_canvas_describe_clip(const lh_ui_canvas_t *self, lh_ui_canvas_clip_t *out)
{
    lh_assert_runtime_ref(self);
    lh_return_if(!self->state.clipped, lh_null);
    lh_ui_canvas_clip_init(out, lh_addr_of(self->state.clip), self->rounds, self->state.round_count);
    return out;
}

lh_u32_t
lh_ui_canvas_get_round_count(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->state.round_count;
}

lh_void
lh_ui_canvas_send_clip(lh_ui_canvas_t *self)
{
    lh_ui_canvas_clip_t clip;
    const lh_ui_rect_t *rect;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->set_clip));
    /* The clip is the region the frame is about to draw into, so it is what the
       frame draws into, and this is the one place that knows both. Recorded as the
       rect the backend is handed — in the buffer's own space, so ::lh_ui_canvas_end
       presents exactly the pixels the backend was cut to — and as its own entry,
       because two cuts standing apart are two entries and their gap belongs to
       neither. A frame that sets no clip leaves the list empty, and empty means
       "everything", the way a frame with no damage draws all of it. */
    rect = lh_ui_canvas_get_clip(self);
    self->backend->set_clip(self->context, lh_ui_canvas_describe_clip(self, lh_addr_of(clip)));
    if (!lh_null_eq(rect))
    {
        lh_ui_rects_add(lh_addr_of(self->drawn), rect);
    }
}

lh_void
lh_ui_canvas_sync_clip(lh_ui_canvas_t *self, const lh_ui_canvas_state_t *before)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_ui_canvas_state_has_same_clip(before, lh_addr_of(self->state)));
    lh_ui_canvas_send_clip(self);
}

lh_void
lh_ui_canvas_save(lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(self->depth >= LH_LIBRARY_OPTION_UI_CANVAS_DEPTH, lh_runtime_error_code_overflow);
    self->saved[self->depth] = self->state;
    ++self->depth;
}

lh_void
lh_ui_canvas_clip_to(lh_ui_canvas_t *self, const lh_ui_rect_t *clip_rect)
{
    lh_ui_rect_t target;
    lh_return_if(lh_null_eq(clip_rect));
    target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), clip_rect);
    lh_ui_canvas_state_clip_to(lh_addr_of(self->state), lh_addr_of(target));
}

lh_void
lh_ui_canvas_add_round(lh_ui_canvas_t *self, const lh_ui_rect_t *clip_rect, lh_ui_scalar_t radius)
{
    lh_ui_rect_t target;

    lh_return_if(lh_null_eq(clip_rect) || radius <= lh_ui_scalar(0));
    target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), clip_rect);
    lh_ui_canvas_clip_round_init(self->rounds + self->state.round_count, lh_addr_of(target), radius);
    ++self->state.round_count;
}

lh_void
lh_ui_canvas_push_round(lh_ui_canvas_t *self, lh_ui_point_t offset_delta, const lh_ui_rect_t *clip_rect,
                        lh_ui_scalar_t radius)
{
    lh_ui_canvas_save(self);
    lh_ui_canvas_add_round(self, clip_rect, radius);
    lh_ui_canvas_clip_to(self, clip_rect);
    lh_ui_canvas_state_move(lh_addr_of(self->state), offset_delta);
    lh_ui_canvas_sync_clip(self, lh_addr_of(self->saved[self->depth - 1U]));
}

lh_void
lh_ui_canvas_push(lh_ui_canvas_t *self, lh_ui_point_t offset_delta, const lh_ui_rect_t *clip_rect)
{
    lh_ui_canvas_push_round(self, offset_delta, clip_rect, lh_ui_scalar(0));
}

lh_void
lh_ui_canvas_pop(lh_ui_canvas_t *self)
{
    lh_ui_canvas_state_t dropped;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(self->depth == 0U, lh_runtime_error_code_underflow);
    dropped = self->state;
    self->state = self->saved[--self->depth];
    lh_ui_canvas_sync_clip(self, lh_addr_of(dropped));
}

lh_ui_point_t
lh_ui_canvas_get_offset(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->state.offset;
}

const lh_ui_rect_t *
lh_ui_canvas_get_clip(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_canvas_state_get_clip(lh_addr_of(self->state));
}

/* ── Frame ───────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_begin(lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->begin));
    lh_ui_point_init(lh_addr_of(self->frame_at), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_canvas_state_set_origin(lh_addr_of(self->state), self->frame_at);
    lh_ui_rects_init(lh_addr_of(self->drawn));
    self->backend->begin(self->context);
}

lh_void
lh_ui_canvas_begin_area(lh_ui_canvas_t *self, const lh_ui_rect_t *area)
{
    const lh_ui_point_t *at;
    lh_ui_point_t origin;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(area);
    /* Without the slot the frame is the whole target, which is exactly what
       begin draws: the area is then only a slice of a full frame, and the
       picture is the same, just no smaller buffer and no faster. */
    if (lh_null_eq(self->backend) || lh_null_eq(self->backend->begin_area))
    {
        (void)lh_ui_canvas_begin(self);
        return;
    }
    at = lh_ui_rect_get_origin_as_const(area);
    self->frame_at = *at;
    /* (0, 0) of the buffer is the area corner, so the space the backend draws
       in is the target space moved by minus the area origin. */
    lh_ui_point_init(lh_addr_of(origin), lh_math_neg(lh_ui_point_get_x(at)), lh_math_neg(lh_ui_point_get_y(at)));
    lh_ui_canvas_state_set_origin(lh_addr_of(self->state), origin);
    lh_ui_rects_init(lh_addr_of(self->drawn));
    self->backend->begin_area(self->context, area);
}

lh_void
lh_ui_canvas_end(lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    /* The frame is over, so the buffer's origin is over with it — even when there
       was no `end` slot to present through. A damage recorded between frames is a
       window rectangle, not a slice of the strip that happened to be drawn last,
       and it would be moved by that origin (::lh_ui_canvas_add_damage): 568 rows
       down for an 800x600 target in 32-row strips, which is a repaint of nothing.
     */
    lh_ui_point_init(lh_addr_of(self->frame_at), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->end));
    self->backend->end(self->context, lh_addr_of(self->drawn));
}

lh_void
lh_ui_canvas_clear(lh_ui_canvas_t *self, const lh_ui_color_t *color)
{
    lh_ui_rect_t area;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->clear));
    self->backend->clear(self->context, color);
    lh_ui_rect_init(lh_addr_of(area), lh_ui_scalar(0), lh_ui_scalar(0),
                    lh_ui_size_get_width(lh_addr_of(self->size)),
                    lh_ui_size_get_height(lh_addr_of(self->size)));
    lh_ui_canvas_add_damage(self, lh_addr_of(area));
}

/* ── Fills ───────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_send_box(lh_ui_canvas_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_ui_rect_t box;

    lh_return_if(x1 <= x0);
    lh_ui_rect_init(lh_addr_of(box), x0, y, x1 - x0, 1);
    self->backend->fill_rect(self->context, lh_addr_of(box), color);
}

lh_void
lh_ui_canvas_send_clip_pixel(lh_ui_canvas_t *self, const lh_ui_canvas_clip_t *clip, lh_s32_t x, lh_s32_t y,
                             const lh_ui_color_t *color)
{
    const lh_byte_t kept = lh_ui_canvas_clip_coverage(clip, x, y);
    const lh_ui_color_t edge = lh_ui_color_with_coverage(color, kept);

    lh_return_if(kept == 0U);
    lh_ui_canvas_send_box(self, x, x + 1, y, kept == 255U ? color : lh_addr_of(edge));
}

lh_void
lh_ui_canvas_send_clip_pixels(lh_ui_canvas_t *self, const lh_ui_canvas_clip_t *clip, lh_s32_t x0, lh_s32_t x1,
                              lh_s32_t y, const lh_ui_color_t *color)
{
    for (; x0 < x1; ++x0)
    {
        lh_ui_canvas_send_clip_pixel(self, clip, x0, y, color);
    }
}

lh_void
lh_ui_canvas_send_clip_row(lh_ui_canvas_t *self, const lh_ui_canvas_clip_t *clip, lh_s32_t x0, lh_s32_t x1,
                           lh_s32_t y, const lh_ui_color_t *color)
{
    lh_s32_t mid0;
    lh_s32_t mid1;

    lh_ui_canvas_clip_split_row(clip, x0, x1, y, lh_addr_of(mid0), lh_addr_of(mid1));
    lh_ui_canvas_send_clip_pixels(self, clip, x0, mid0, y, color);
    lh_ui_canvas_send_box(self, mid0, mid1, y, color);
    lh_ui_canvas_send_clip_pixels(self, clip, mid1, x1, y, color);
}

lh_void
lh_ui_canvas_send_clip_rows(lh_ui_canvas_t *self, const lh_ui_rect_t *cut, const lh_ui_color_t *color)
{
    lh_ui_canvas_clip_t clip;
    const lh_s32_t y1 = lh_ui_canvas_round_bottom(cut);
    lh_s32_t y;

    (void)lh_ui_canvas_describe_clip(self, lh_addr_of(clip));
    for (y = lh_ui_canvas_round_top(cut); y < y1; ++y)
    {
        lh_ui_canvas_send_clip_row(self, lh_addr_of(clip), lh_ui_canvas_round_left(cut),
                                   lh_ui_canvas_round_right(cut), y, color);
    }
}

lh_void
lh_ui_canvas_send_cut(lh_ui_canvas_t *self, const lh_ui_rect_t *cut, const lh_ui_color_t *color)
{
    if (self->state.round_count == 0U)
    {
        self->backend->fill_rect(self->context, cut, color);
        return;
    }
    lh_ui_canvas_send_clip_rows(self, cut, color);
}

lh_void
lh_ui_canvas_fill_target_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *target, const lh_ui_color_t *color)
{
    lh_ui_rect_t cut;

    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->fill_rect));
    cut = lh_ui_canvas_state_cut(lh_addr_of(self->state), target);
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(cut)));
    lh_ui_canvas_add_damage(self, lh_addr_of(cut));
    if (lh_ui_canvas_is_cutting(self))
    {
        lh_ui_canvas_send_cut(self, lh_addr_of(cut), color);
        return;
    }
    self->backend->fill_rect(self->context, target, color);
}

lh_bool_t
lh_ui_canvas_shows(const lh_ui_canvas_t *self, const lh_ui_rect_t *target)
{
    const lh_ui_rect_t cut = lh_ui_canvas_state_cut(lh_addr_of(self->state), target);

    return lh_ui_rect_is_empty(lh_addr_of(cut)) ? lh_bool_false : lh_bool_true;
}

lh_bool_t
lh_ui_canvas_shows_rect(const lh_ui_canvas_t *self, const lh_ui_rect_t *rect)
{
    const lh_ui_rect_t target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), rect);

    return lh_ui_canvas_shows(self, lh_addr_of(target));
}

lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    lh_ui_rect_t target;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(color);
    target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), rect);
    lh_ui_canvas_fill_target_rect(self, lh_addr_of(target), color);
}

lh_bool_t
lh_ui_canvas_can_send_whole(const lh_ui_canvas_t *self, const lh_ui_rect_t *target)
{
    /* The bar for a slot that *reads* its target: the pixels it is about to work
       on have to be there. No clip at all, or the target inside a plain
       (unrounded) one. A rounded cut is refused as well, because a backend that
       clips knows it and a software one does not. */
    lh_return_if(!self->state.clipped, lh_bool_true);
    return self->state.round_count == 0U && lh_ui_canvas_state_contains(lh_addr_of(self->state), target)
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_ui_canvas_can_fill_round(const lh_ui_canvas_t *self, const lh_ui_rect_t *target)
{
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->fill_round_rect), lh_bool_false);
    /* A rounded box is a shape and not a reading: it is drawn whole and cut by
       whoever clips, so a backend that clips may have it even when the clip
       cuts it. Keeping that shortcut here and not in ::lh_ui_canvas_can_send_whole
       is the whole difference between the two kinds of slot. */
    lh_return_if(!lh_ui_canvas_is_cutting(self), lh_bool_true);
    return lh_ui_canvas_can_send_whole(self, target);
}

lh_bool_t
lh_ui_canvas_try_fill_round(lh_ui_canvas_t *self, const lh_ui_rect_t *target, lh_ui_scalar_t radius,
                            const lh_ui_color_t *color)
{
    lh_return_if(!lh_ui_canvas_can_fill_round(self, target), lh_bool_false);
    return self->backend->fill_round_rect(self->context, target, radius, color);
}

lh_void
lh_ui_canvas_fill_target_round_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *target, lh_ui_scalar_t radius,
                                    const lh_ui_color_t *color)
{
    lh_ui_rect_t cut;

    if (radius <= lh_ui_scalar(0))
    {
        lh_ui_canvas_fill_target_rect(self, target, color);
        return;
    }
    cut = lh_ui_canvas_state_cut(lh_addr_of(self->state), target);
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(cut)));
    lh_ui_canvas_add_damage(self, lh_addr_of(cut));
    lh_return_if(lh_ui_canvas_try_fill_round(self, target, radius, color));
    lh_ui_canvas_fill_round_rect_by_rects(self, target, radius, color);
}

lh_void
lh_ui_canvas_fill_round_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                             const lh_ui_color_t *color)
{
    lh_ui_rect_t target;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(color);
    target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), rect);
    lh_ui_canvas_fill_target_round_rect(self, lh_addr_of(target), lh_ui_radius_clamp(rect, radius), color);
}

lh_void
lh_ui_canvas_shadow_fallback(lh_ui_canvas_t *self, const lh_ui_rect_t *bounds, const lh_ui_rect_t *target,
                             lh_ui_scalar_t radius, const lh_ui_shadow_t *shadow)
{
    const lh_ui_point_t *cut_origin;
    const lh_ui_size_t *cut_size;
    lh_ui_rect_t cut;
    lh_s32_t x1;
    lh_s32_t y1;
    lh_s32_t y;

    /* Only what the clip lets through is worth a pixel each, and a frame drawn in
       strips clips it once per strip. */
    cut = lh_ui_canvas_state_cut(lh_addr_of(self->state), bounds);
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(cut)));
    cut_origin = lh_ui_rect_get_origin_as_const(lh_addr_of(cut));
    cut_size = lh_ui_rect_get_size_as_const(lh_addr_of(cut));
    x1 = lh_ui_point_get_x(cut_origin) + lh_ui_size_get_width(cut_size);
    y1 = lh_ui_point_get_y(cut_origin) + lh_ui_size_get_height(cut_size);

    for (y = lh_ui_point_get_y(cut_origin); y < y1; ++y)
    {
        lh_s32_t x;

        for (x = lh_ui_point_get_x(cut_origin); x < x1; ++x)
        {
            const lh_byte_t alpha = lh_ui_shadow_alpha_at(shadow, lh_ui_scalar(x), lh_ui_scalar(y), target, radius);
            lh_ui_rect_t pixel;
            lh_ui_color_t color;

            if (alpha == 0)
            {
                continue;
            }
            color = lh_ui_shadow_get_color(shadow);
            lh_ui_color_set_a(&color, alpha);
            lh_ui_rect_init(lh_addr_of(pixel), lh_ui_scalar(x), lh_ui_scalar(y), lh_ui_scalar(1), lh_ui_scalar(1));
            lh_ui_canvas_fill_target_rect(self, lh_addr_of(pixel), &color);
        }
    }
}

lh_bool_t
lh_ui_canvas_shadow(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                    const lh_ui_shadow_t *shadow)
{
    lh_ui_rect_t bounds;
    lh_ui_rect_t target;
    lh_ui_scalar_t outset;
    lh_ui_scalar_t corner;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(shadow);
    target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), rect);
    outset = lh_ui_shadow_get_outset(shadow, &target);
    lh_return_if(outset <= 0, lh_bool_false);
    corner = lh_ui_radius_clamp(&target, radius);
    bounds = lh_ui_rect_inset(lh_addr_of(target), lh_ui_scalar(-outset), lh_ui_scalar(-outset));
    lh_ui_canvas_add_damage(self, lh_addr_of(bounds));
    /* No `can_send_whole` here, unlike a rounded fill: a shadow reads no pixels, it
       is the box and the shadow alone, so every area that clips it draws its own
       slice and the picture is the same whether the frame is drawn whole or in
       strips. The effect that does need the whole area in one buffer is the blur. */
    lh_return_if(!lh_ui_canvas_shows(self, lh_addr_of(bounds)), lh_bool_false);

    if (!lh_null_eq(self->backend) && !lh_null_eq(self->backend->shadow) &&
        self->backend->shadow(self->context, &target, corner, shadow))
    {
        return lh_bool_true;
    }
    lh_ui_canvas_shadow_fallback(self, lh_addr_of(bounds), lh_addr_of(target), corner, shadow);
    return lh_bool_true;
}

lh_bool_t
lh_ui_canvas_blur(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius)
{
    lh_ui_rect_t target;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_return_if(blur_radius <= 0, lh_bool_false);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->blur), lh_bool_false);
    target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), rect);
    lh_return_if(!lh_ui_canvas_shows(self, lh_addr_of(target)), lh_bool_false);
    /* A blur reads the pixels it is about to blur, so unlike a shadow it cannot
       be cut to the area: in a frame drawn in strips the pixels across the line
       are not in the buffer at all, and blurring what is there would leave a
       visible seam every strip. It says it cannot be drawn instead. */
    lh_return_if(!lh_ui_canvas_can_send_whole(self, lh_addr_of(target)), lh_bool_false);
    lh_ui_canvas_add_damage(self, lh_addr_of(target));
    return self->backend->blur(self->context, lh_addr_of(target), blur_radius, self->scratch, self->scratch_bytes);
}

lh_bool_t
lh_ui_canvas_glass(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t corner,
                   lh_ui_scalar_t blur_radius, const lh_ui_color_t *tint)
{
    lh_ui_rect_t target;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(tint);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->glass), lh_bool_false);
    target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), rect);
    lh_return_if(!lh_ui_canvas_shows(self, lh_addr_of(target)), lh_bool_false);
    /* Glass is a blur plus a tint, so it needs the whole rect in the buffer for
       the same reason, and the tint is not put down without it: a tint on its own
       is a fill, and a fill that only looks like glass is worse than none. */
    lh_return_if(!lh_ui_canvas_can_send_whole(self, lh_addr_of(target)), lh_bool_false);
    lh_ui_canvas_add_damage(self, lh_addr_of(target));
    return self->backend->glass(self->context, lh_addr_of(target), lh_ui_radius_clamp(lh_addr_of(target), corner),
                                blur_radius, tint, self->scratch, self->scratch_bytes);
}
