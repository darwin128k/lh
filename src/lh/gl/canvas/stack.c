/**
 * @file stack.c
 * @brief Implementation of `lh/gl/canvas.h`: the state stack: save, push and pop, the clip and its
 * rounded corners.
 */

#include <lh/gl/canvas.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_canvas_is_cutting(const lh_gl_canvas_t *self)
{
    lh_return_if(!lh_gl_canvas_state_is_clipped(lh_gl_canvas_get_state(self)), lh_bool_false);
    return lh_null_eq(lh_gl_canvas_get_backend(self)) ||
                   lh_null_eq(lh_gl_canvas_backend_get_set_clip_fn(lh_gl_canvas_get_backend(self)))
               ? lh_bool_true
               : lh_bool_false;
}

const lh_gl_canvas_clip_t *
lh_gl_canvas_describe_clip(const lh_gl_canvas_t *self, lh_gl_canvas_clip_t *out)
{
    lh_return_if(!lh_gl_canvas_state_is_clipped(lh_gl_canvas_get_state(self)), lh_null);
    lh_gl_canvas_clip_init(out, lh_gl_canvas_state_get_clip(lh_gl_canvas_get_state(self)),
                           lh_gl_canvas_get_rounds(self),
                           lh_gl_canvas_state_get_round_count(lh_gl_canvas_get_state(self)));
    return out;
}

lh_u32_t
lh_gl_canvas_get_round_count(const lh_gl_canvas_t *self)
{
    return lh_gl_canvas_state_get_round_count(lh_gl_canvas_get_state(self));
}

lh_void
lh_gl_canvas_save(lh_gl_canvas_t *self)
{
    lh_assert_runtime_if(lh_gl_canvas_get_depth(self) >= LH_LIBRARY_OPTION_GL_CANVAS_DEPTH,
                         lh_runtime_error_code_overflow);
    lh_gl_canvas_get_saved_mut(self)[lh_gl_canvas_get_depth(self)] =
        lh_ptr_deref(lh_gl_canvas_get_state(self));
    lh_gl_canvas_set_depth(self, lh_gl_canvas_get_depth(self) + 1);
}

lh_void
lh_gl_canvas_clip_to(lh_gl_canvas_t *self, const lh_gl_rect_t *clip_rect)
{
    lh_return_if(lh_null_eq(clip_rect));
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state_mut(self), clip_rect, lh_addr_of(target));
    lh_gl_canvas_state_clip_to(lh_gl_canvas_get_state_mut(self), lh_addr_of(target));
}

lh_void
lh_gl_canvas_add_round(lh_gl_canvas_t *self, const lh_gl_rect_t *clip_rect, lh_gl_scalar_t radius)
{
    lh_return_if(lh_null_eq(clip_rect) || radius <= lh_gl_scalar(0));
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state_mut(self), clip_rect, lh_addr_of(target));
    lh_gl_canvas_clip_round_init(
        lh_gl_canvas_get_rounds_mut(self) +
            lh_gl_canvas_state_get_round_count(lh_gl_canvas_get_state(self)),
        lh_addr_of(target), radius);
    lh_gl_canvas_state_set_round_count(
        lh_gl_canvas_get_state_mut(self),
        lh_gl_canvas_state_get_round_count(lh_gl_canvas_get_state_mut(self)) + 1);
}

lh_void
lh_gl_canvas_get_offset(const lh_gl_canvas_t *self, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_canvas_state_get_offset(lh_gl_canvas_get_state(self)));
}

const lh_gl_rect_t *
lh_gl_canvas_get_clip(const lh_gl_canvas_t *self)
{
    return lh_gl_canvas_state_get_clip_v(lh_gl_canvas_get_state(self));
}

lh_void
lh_gl_canvas_sync_clip(lh_gl_canvas_t *self, const lh_gl_canvas_state_t *before)
{
    lh_return_if(lh_gl_canvas_state_has_same_clip(before, lh_gl_canvas_get_state_mut(self)));
    lh_gl_canvas_send_clip(self);
}

lh_void
lh_gl_canvas_push_round(lh_gl_canvas_t *self, lh_gl_point_t offset_delta,
                        const lh_gl_rect_t *clip_rect, lh_gl_scalar_t radius)
{
    lh_gl_canvas_save(self);
    lh_gl_canvas_add_round(self, clip_rect, radius);
    lh_gl_canvas_clip_to(self, clip_rect);
    lh_gl_canvas_state_move(lh_gl_canvas_get_state_mut(self), offset_delta);
    lh_gl_canvas_sync_clip(
        self, lh_addr_of(lh_gl_canvas_get_saved(self)[lh_gl_canvas_get_depth(self) - 1U]));
}

lh_void
lh_gl_canvas_pop(lh_gl_canvas_t *self)
{
    lh_assert_runtime_if(lh_gl_canvas_get_depth(self) == 0U, lh_runtime_error_code_underflow);
    lh_gl_canvas_state_t dropped = lh_ptr_deref(lh_gl_canvas_get_state(self));
    lh_gl_canvas_set_depth(self, lh_cast_static(lh_u8_t, (lh_gl_canvas_get_depth(self) - 1U)));
    lh_gl_canvas_set_state(self, lh_gl_canvas_get_saved(self)[lh_gl_canvas_get_depth(self)]);
    lh_gl_canvas_sync_clip(self, lh_addr_of(dropped));
}

lh_void
lh_gl_canvas_push(lh_gl_canvas_t *self, lh_gl_point_t offset_delta, const lh_gl_rect_t *clip_rect)
{
    lh_gl_canvas_push_round(self, offset_delta, clip_rect, lh_gl_scalar(0));
}
