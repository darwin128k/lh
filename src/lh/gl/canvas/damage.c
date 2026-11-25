/**
 * @file damage.c
 * @brief Implementation of `lh/gl/canvas.h`: the damage: what a frame has drawn, and where.
 */

#include <lh/gl/canvas.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_canvas_add_damage(lh_gl_canvas_t *self, const lh_gl_rect_t *rect)
{
    /* A primitive reaches the backend already moved by the frame origin, so the
       rect given here is still in the buffer. Put it back: the damage union is
       in target space, where the caller drew it, and stays that way whichever
       part of the target this frame covers. */
    lh_gl_rect_t drawn;
    lh_gl_rect_offset(rect, lh_gl_point_get_x(lh_gl_canvas_get_frame_at_mut(self)),
                      lh_gl_point_get_y(lh_gl_canvas_get_frame_at_mut(self)), lh_addr_of(drawn));

    lh_assert_runtime_ref(rect);
    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(drawn)));
    if (!lh_gl_canvas_is_has_damage(self))
    {
        lh_gl_canvas_set_damage(self, drawn);
        lh_gl_canvas_set_has_damage(self, lh_bool_true);
        return;
    }
    lh_gl_rect_union(lh_gl_canvas_get_damage_mut(self), lh_addr_of(drawn),
                     lh_gl_canvas_get_damage_mut(self));
}

lh_void
lh_gl_canvas_reset_damage(lh_gl_canvas_t *self)
{
    lh_gl_rect_init_empty(lh_gl_canvas_get_damage_mut(self));
    lh_gl_canvas_set_has_damage(self, lh_bool_false);
}

lh_bool_t
lh_gl_canvas_has_damage(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->has_damage;
}

const lh_gl_rect_t *
lh_gl_canvas_get_damage_v(const lh_gl_canvas_t *self)
{
    return lh_gl_canvas_is_has_damage(self) ? lh_gl_canvas_get_damage(self) : lh_null;
}

lh_void
lh_gl_canvas_damage_in(const lh_gl_rect_t *area, const lh_gl_rect_t *damage, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);

    lh_assert_runtime_ref(area);
    /* No damage is the whole target: nothing was said, so everything still holds. */
    if (lh_null_eq(damage))
    {
        lh_ptr_deref(out) = lh_ptr_deref(area);
        return;
    }
    lh_gl_rect_t part;
    lh_gl_rect_intersection(area, damage, lh_addr_of(part));
    lh_ptr_deref(out) = part;
}
