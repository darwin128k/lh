/**
 * @file axis.c
 * @brief Implementation of `lh/gl/axis.h`.
 */

#include <lh/gl/axis.h>

lh_gl_axis_t
lh_gl_axis_get_cross(lh_gl_axis_t axis)
{
    return axis == lh_gl_axis_vertical ? lh_gl_axis_horizontal : lh_gl_axis_vertical;
}

lh_gl_scalar_t
lh_gl_point_get_along(const lh_gl_point_t *self, lh_gl_axis_t axis)
{
    lh_assert_runtime_ref(self);
    return axis == lh_gl_axis_vertical ? lh_gl_point_get_y(self) : lh_gl_point_get_x(self);
}

lh_void
lh_gl_point_set_along(lh_gl_point_t *self, lh_gl_axis_t axis, lh_gl_scalar_t value)
{
    lh_assert_runtime_ref(self);
    if (axis == lh_gl_axis_vertical)
    {
        lh_gl_point_set_y(self, value);
        return;
    }
    lh_gl_point_set_x(self, value);
}

lh_void
lh_gl_point_init_along(lh_gl_point_t *self, lh_gl_axis_t axis, lh_gl_scalar_t along,
                       lh_gl_scalar_t across)
{
    lh_assert_runtime_ref(self);
    lh_gl_point_init(self, across, across);
    lh_gl_point_set_along(self, axis, along);
}

lh_gl_scalar_t
lh_gl_size_get_along(const lh_gl_size_t *self, lh_gl_axis_t axis)
{
    lh_assert_runtime_ref(self);
    return axis == lh_gl_axis_vertical ? lh_gl_size_get_height(self) : lh_gl_size_get_width(self);
}

lh_void
lh_gl_size_set_along(lh_gl_size_t *self, lh_gl_axis_t axis, lh_gl_scalar_t value)
{
    lh_assert_runtime_ref(self);
    if (axis == lh_gl_axis_vertical)
    {
        lh_gl_size_set_height(self, value);
        return;
    }
    lh_gl_size_set_width(self, value);
}

lh_void
lh_gl_rect_init_along(lh_gl_rect_t *self, const lh_gl_rect_t *base, lh_gl_axis_t axis,
                      lh_gl_scalar_t start, lh_gl_scalar_t length)
{
    lh_gl_point_t *origin;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(base);
    lh_ptr_deref(self) = lh_ptr_deref(base);
    origin = lh_gl_rect_get_origin_mut(self);
    lh_gl_point_set_along(origin, axis, lh_gl_point_get_along(origin, axis) + start);
    lh_gl_size_set_along(lh_gl_rect_get_size_mut(self), axis, length);
}
