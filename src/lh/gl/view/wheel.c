/**
 * @file wheel.c
 * @brief Implementation of `lh/gl/view.h`: the wheel: how far a notch scrolls, and the notch
 * itself.
 */

#include <lh/gl/view.h>

#include <lh/core/math.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_view_wheel(lh_gl_view_t *self, lh_gl_point_t point, lh_gl_scalar_t dx, lh_gl_scalar_t dy)
{
    lh_gl_view_stop_throw(self);
    lh_bool_t changed = lh_gl_view_scroll_by(
        self, lh_gl_scrollbar_find_scrolled(lh_gl_view_hit_test(self, point)), dx, dy);
    lh_gl_view_end_scroll(self);
    return changed;
}

lh_gl_scalar_t
lh_gl_view_wheel_distance(lh_s32_t notch_120, lh_s32_t step)
{
    return lh_type_cast(lh_gl_scalar_t, lh_math_neg(notch_120) * step / LH_INPUT_WHEEL_NOTCH);
}

lh_s32_t
lh_gl_view_get_wheel_step(const lh_gl_view_t *self)
{
    lh_input_wheel_lines_fn *fn = lh_gl_view_get_wheel_lines(self);

    lh_return_if(lh_null_eq(fn), LH_GL_VIEW_WHEEL_STEP);
    return lh_input_wheel_step_of(fn(lh_gl_view_get_wheel_lines_context(self)));
}

lh_void
lh_gl_view_install_wheel_lines(lh_gl_view_t *self, lh_input_wheel_lines_fn *fn, lh_ptr context)
{
    lh_gl_view_set_wheel_lines(self, fn);
    lh_gl_view_set_wheel_lines_context(self, context);
}

lh_bool_t
lh_gl_view_input_wheel(lh_gl_view_t *self, lh_input_event_cptr event)
{
    const lh_s32_t step = lh_gl_view_get_wheel_step(self);

    return lh_gl_view_wheel(self, lh_gl_view_point_of(event),
                            lh_gl_view_wheel_distance(lh_input_event_get_dx(event), step),
                            lh_gl_view_wheel_distance(lh_input_event_get_dy(event), step));
}
