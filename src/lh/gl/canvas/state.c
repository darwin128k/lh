/**
 * @file state.c
 * @brief Implementation of `lh/gl/canvas/state.h`.
 */
#include <lh/core/assert.h>

#include <lh/gl/canvas/state.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_canvas_state_init(lh_gl_canvas_state_t *self)
{
    lh_gl_point_init(lh_gl_canvas_state_get_offset_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_gl_rect_init_empty(lh_gl_canvas_state_get_clip_mut(self));
    lh_gl_canvas_state_set_clipped(self, lh_bool_false);
    lh_gl_canvas_state_set_round_count(self, 0U);
}

const lh_gl_rect_t *
lh_gl_canvas_state_get_clip_v(const lh_gl_canvas_state_t *self)
{
    return lh_gl_canvas_state_is_clipped(self) ? lh_gl_canvas_state_get_clip(self) : lh_null;
}

lh_bool_t
lh_gl_canvas_state_has_same_clip(const lh_gl_canvas_state_t *a, const lh_gl_canvas_state_t *b)
{
    lh_return_if(lh_gl_canvas_state_is_clipped(a) != lh_gl_canvas_state_is_clipped(b) ||
                     lh_gl_canvas_state_get_round_count(a) != lh_gl_canvas_state_get_round_count(b),
                 lh_bool_false);
    return !lh_gl_canvas_state_is_clipped(a) ||
                   lh_gl_rect_eq(lh_gl_canvas_state_get_clip(a), lh_gl_canvas_state_get_clip(b))
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_gl_canvas_state_to_target(const lh_gl_canvas_state_t *self, const lh_gl_rect_t *rect,
                             lh_gl_rect_t *out)
{
    lh_gl_rect_offset(rect, lh_gl_point_get_x(lh_gl_canvas_state_get_offset(self)),
                      lh_gl_point_get_y(lh_gl_canvas_state_get_offset(self)), out);
}

lh_void
lh_gl_canvas_state_move(lh_gl_canvas_state_t *self, lh_gl_point_t delta)
{
    lh_gl_point_offset(lh_gl_canvas_state_get_offset_mut(self),
                       lh_gl_point_get_x(lh_addr_of(delta)), lh_gl_point_get_y(lh_addr_of(delta)),
                       lh_gl_canvas_state_get_offset_mut(self));
}

lh_void
lh_gl_canvas_state_set_origin(lh_gl_canvas_state_t *self, lh_gl_point_t origin)
{
    lh_assert_runtime_ref(self);
    self->offset = origin;
}

lh_void
lh_gl_canvas_state_cut(const lh_gl_canvas_state_t *self, const lh_gl_rect_t *target,
                       lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    if (lh_gl_canvas_state_is_clipped(self))
    {
        lh_gl_rect_intersection(lh_gl_canvas_state_get_clip(self), target, out);
    }
    else
    {
        lh_ptr_deref(out) = lh_ptr_deref(target);
    }
}

lh_void
lh_gl_canvas_state_clip_to(lh_gl_canvas_state_t *self, const lh_gl_rect_t *target)
{
    lh_gl_canvas_state_cut(self, target, lh_gl_canvas_state_get_clip_mut(self));
    lh_gl_canvas_state_set_clipped(self, lh_bool_true);
}

lh_bool_t
lh_gl_canvas_state_contains(const lh_gl_canvas_state_t *self, const lh_gl_rect_t *target)
{
    lh_gl_rect_t cut;
    lh_gl_canvas_state_cut(self, target, lh_addr_of(cut));
    return lh_gl_rect_eq(lh_addr_of(cut), target);
}

const lh_gl_point_t *
lh_gl_canvas_state_get_offset(const lh_gl_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->offset);
}

lh_gl_point_t *
lh_gl_canvas_state_get_offset_mut(lh_gl_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->offset);
}

lh_void
lh_gl_canvas_state_set_offset(lh_gl_canvas_state_t *self, lh_gl_point_t offset)
{
    lh_assert_runtime_ref(self);
    self->offset = offset;
}

lh_gl_rect_t *
lh_gl_canvas_state_get_clip_mut(lh_gl_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

lh_void
lh_gl_canvas_state_set_clip(lh_gl_canvas_state_t *self, lh_gl_rect_t clip)
{
    lh_assert_runtime_ref(self);
    self->clip = clip;
}

lh_bool_t
lh_gl_canvas_state_is_clipped(const lh_gl_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clipped;
}

lh_void
lh_gl_canvas_state_set_clipped(lh_gl_canvas_state_t *self, lh_bool_t clipped)
{
    lh_assert_runtime_ref(self);
    self->clipped = clipped;
}

lh_u8_t
lh_gl_canvas_state_get_round_count(const lh_gl_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return self->round_count;
}

lh_void
lh_gl_canvas_state_set_round_count(lh_gl_canvas_state_t *self, lh_u8_t round_count)
{
    lh_assert_runtime_ref(self);
    self->round_count = round_count;
}

const lh_gl_rect_t *
lh_gl_canvas_state_get_clip(const lh_gl_canvas_state_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}
