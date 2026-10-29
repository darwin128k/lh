/**
 * @file state.c
 * @brief Implementation of `lh/ui/canvas/state.h`.
 */
#include <lh/assert.h>

#include <lh/ui/canvas/state.h>
#include <lh/util/return.h>

lh_void
lh_ui_canvas_state_init(lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_point_init(lh_ui_canvas_state_get_offset_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_rect_init_empty(lh_ui_canvas_state_get_clip_mut(self));
    lh_ui_canvas_state_set_clipped(self, lh_bool_false);
    lh_ui_canvas_state_set_round_count(self, 0U);
}

const lh_ui_rect_t *
lh_ui_canvas_state_get_clip_v(const lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_canvas_state_is_clipped(self) ? lh_ui_canvas_state_get_clip(self) : lh_null;
}

lh_bool_t
lh_ui_canvas_state_has_same_clip(const lh_ui_canvas_state_t *a, const lh_ui_canvas_state_t *b)
{
    lh_return_if(lh_ui_canvas_state_is_clipped(a) != lh_ui_canvas_state_is_clipped(b) || lh_ui_canvas_state_get_round_count(a) != lh_ui_canvas_state_get_round_count(b), lh_bool_false);
    return !lh_ui_canvas_state_is_clipped(a) || lh_ui_rect_eq(lh_ui_canvas_state_get_clip(a), lh_ui_canvas_state_get_clip(b)) ? lh_bool_true
                                                                                  : lh_bool_false;
}

lh_void
lh_ui_canvas_state_to_target(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *rect,
                             lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(self);
    lh_ui_rect_offset(rect, lh_ui_point_get_x(lh_ui_canvas_state_get_offset(self)),
                      lh_ui_point_get_y(lh_ui_canvas_state_get_offset(self)), out);
}

lh_void
lh_ui_canvas_state_move(lh_ui_canvas_state_t *self, lh_ui_point_t delta)
{
    lh_assert_runtime_ref(self);
    lh_ui_point_offset(lh_ui_canvas_state_get_offset_mut(self), lh_ui_point_get_x(lh_addr_of(delta)),
                       lh_ui_point_get_y(lh_addr_of(delta)), lh_ui_canvas_state_get_offset_mut(self));
}

lh_void
lh_ui_canvas_state_set_origin(lh_ui_canvas_state_t *self, lh_ui_point_t origin)
{
    lh_assert_runtime_ref(self);
    self->offset = origin;
}

lh_void
lh_ui_canvas_state_cut(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *target,
                       lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    if (lh_ui_canvas_state_is_clipped(self))
    {
        lh_ui_rect_intersection(lh_ui_canvas_state_get_clip(self), target, out);
    }
    else
    {
        lh_ptr_deref(out) = lh_ptr_deref(target);
    }
}

lh_void
lh_ui_canvas_state_clip_to(lh_ui_canvas_state_t *self, const lh_ui_rect_t *target)
{
    lh_ui_canvas_state_cut(self, target, lh_ui_canvas_state_get_clip_mut(self));
    lh_ui_canvas_state_set_clipped(self, lh_bool_true);
}

lh_bool_t
lh_ui_canvas_state_contains(const lh_ui_canvas_state_t *self, const lh_ui_rect_t *target)
{
    lh_ui_rect_t cut;
    lh_ui_canvas_state_cut(self, target, lh_addr_of(cut));
    return lh_ui_rect_eq(lh_addr_of(cut), target);
}

const lh_ui_point_t *
lh_ui_canvas_state_get_offset(const lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->offset);
}

lh_ui_point_t *
lh_ui_canvas_state_get_offset_mut(lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->offset);
}

lh_void
lh_ui_canvas_state_set_offset(lh_ui_canvas_state_t *self, lh_ui_point_t offset)
{
    lh_assert_runtime_ref(self);
    self->offset = offset;
}

lh_ui_rect_t *
lh_ui_canvas_state_get_clip_mut(lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

lh_void
lh_ui_canvas_state_set_clip(lh_ui_canvas_state_t *self, lh_ui_rect_t clip)
{
    lh_assert_runtime_ref(self);
    self->clip = clip;
}

lh_bool_t
lh_ui_canvas_state_is_clipped(const lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clipped;
}

lh_void
lh_ui_canvas_state_set_clipped(lh_ui_canvas_state_t *self, lh_bool_t clipped)
{
    lh_assert_runtime_ref(self);
    self->clipped = clipped;
}

lh_u8_t
lh_ui_canvas_state_get_round_count(const lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return self->round_count;
}

lh_void
lh_ui_canvas_state_set_round_count(lh_ui_canvas_state_t *self, lh_u8_t round_count)
{
    lh_assert_runtime_ref(self);
    self->round_count = round_count;
}

const lh_ui_rect_t *
lh_ui_canvas_state_get_clip(const lh_ui_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}
