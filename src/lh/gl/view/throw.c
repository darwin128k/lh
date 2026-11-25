/**
 * @file throw.c
 * @brief Implementation of `lh/gl/view.h`: the throw: a released drag glides on and slows down.
 */

#include <lh/gl/view.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_view_is_slow(lh_gl_point_t velocity, lh_gl_scalar_t limit)
{
    return lh_math_abs(lh_gl_point_get_x(lh_addr_of(velocity))) < limit &&
                   lh_math_abs(lh_gl_point_get_y(lh_addr_of(velocity))) < limit
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_gl_view_stop_throw(lh_gl_view_t *self)
{
    lh_gl_point_init(lh_gl_view_get_velocity_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_return_if(lh_null_eq(lh_gl_view_get_throwing(self)));
    lh_gl_view_set_throwing(self, lh_null);
    lh_gl_view_end_scroll(self);
}

lh_bool_t
lh_gl_view_is_gliding(const lh_gl_view_t *self)
{
    return lh_null_ne(lh_gl_view_get_throwing(self)) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_gl_view_slow_down(lh_gl_point_t velocity, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_point_t slower;

    lh_gl_point_init(lh_addr_of(slower),
                     lh_gl_point_get_x(lh_addr_of(velocity)) * LH_GL_VIEW_THROW_KEEP / 100,
                     lh_gl_point_get_y(lh_addr_of(velocity)) * LH_GL_VIEW_THROW_KEEP / 100);
    lh_ptr_deref(out) = slower;
}

lh_void
lh_gl_view_start_throw(lh_gl_view_t *self, lh_gl_container_t *box)
{
    if (lh_gl_view_is_slow(lh_ptr_deref(lh_gl_view_get_velocity(self)), LH_GL_VIEW_THROW_MIN))
    {
        lh_gl_view_end_scroll(self);
        return;
    }
    lh_gl_view_set_throwing(self, box);
}
