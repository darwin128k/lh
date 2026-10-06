/**
 * @file canvas.c
 * @brief Implementation of `lh/ui/canvas.h` and the null backend.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/radius.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

/* ── Null backend ────────────────────────────────────────────────────────── */

const lh_ui_canvas_backend_t lh_ui_canvas_backend_null = {lh_null, lh_null, lh_null, lh_null,
                                                            lh_null, lh_null, lh_null};

/* ── Lifetime ────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
    self->context = context;
    lh_ui_canvas_state_init(lh_addr_of(self->state));
    self->depth = 0U;
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

/* ── Offset and clip ─────────────────────────────────────────────────────── */

lh_bool_t
lh_ui_canvas_is_cutting(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(!self->state.clipped, lh_bool_false);
    return lh_null_eq(self->backend) || lh_null_eq(self->backend->set_clip) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_canvas_send_clip(lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->set_clip));
    self->backend->set_clip(self->context, lh_ui_canvas_state_get_clip(lh_addr_of(self->state)));
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
lh_ui_canvas_push(lh_ui_canvas_t *self, lh_ui_point_t offset_delta, const lh_ui_rect_t *clip_rect)
{
    lh_ui_canvas_save(self);
    lh_ui_canvas_clip_to(self, clip_rect);
    lh_ui_canvas_state_move(lh_addr_of(self->state), offset_delta);
    lh_ui_canvas_sync_clip(self, lh_addr_of(self->saved[self->depth - 1U]));
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
    self->backend->begin(self->context);
}

lh_void
lh_ui_canvas_end(lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->end));
    self->backend->end(self->context);
}

lh_void
lh_ui_canvas_clear(lh_ui_canvas_t *self, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->clear));
    self->backend->clear(self->context, color);
}

/* ── Fills ───────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_fill_target_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *target, const lh_ui_color_t *color)
{
    lh_ui_rect_t cut;
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->fill_rect));
    cut = lh_ui_canvas_is_cutting(self) ? lh_ui_canvas_state_cut(lh_addr_of(self->state), target) : *target;
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(cut)));
    self->backend->fill_rect(self->context, lh_addr_of(cut), color);
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
lh_ui_canvas_can_fill_round(const lh_ui_canvas_t *self, const lh_ui_rect_t *target)
{
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->fill_round_rect), lh_bool_false);
    return !lh_ui_canvas_is_cutting(self) || lh_ui_canvas_state_contains(lh_addr_of(self->state), target)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_canvas_fill_target_round_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *target, lh_ui_scalar_t radius,
                                    const lh_ui_color_t *color)
{
    if (radius <= lh_ui_scalar(0))
    {
        lh_ui_canvas_fill_target_rect(self, target, color);
    }
    else if (lh_ui_canvas_can_fill_round(self, target))
    {
        self->backend->fill_round_rect(self->context, target, radius, color);
    }
    else
    {
        lh_ui_canvas_fill_round_rect_by_rects(self, target, radius, color);
    }
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
