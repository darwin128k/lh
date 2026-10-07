/**
 * @file axis.c
 * @brief Implementation of `lh/ui/axis.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/axis.h>

lh_ui_axis_t
lh_ui_axis_get_cross(lh_ui_axis_t axis)
{
    return axis == lh_ui_axis_vertical ? lh_ui_axis_horizontal : lh_ui_axis_vertical;
}

lh_ui_scalar_t
lh_ui_point_get_along(const lh_ui_point_t *self, lh_ui_axis_t axis)
{
    lh_assert_runtime_ref(self);
    return axis == lh_ui_axis_vertical ? lh_ui_point_get_y(self) : lh_ui_point_get_x(self);
}

lh_void
lh_ui_point_set_along(lh_ui_point_t *self, lh_ui_axis_t axis, lh_ui_scalar_t value)
{
    lh_assert_runtime_ref(self);
    if (axis == lh_ui_axis_vertical)
    {
        lh_ui_point_set_y(self, value);
        return;
    }
    lh_ui_point_set_x(self, value);
}

lh_void
lh_ui_point_init_along(lh_ui_point_t *self, lh_ui_axis_t axis, lh_ui_scalar_t along, lh_ui_scalar_t across)
{
    lh_assert_runtime_ref(self);
    lh_ui_point_init(self, across, across);
    lh_ui_point_set_along(self, axis, along);
}

lh_ui_scalar_t
lh_ui_size_get_along(const lh_ui_size_t *self, lh_ui_axis_t axis)
{
    lh_assert_runtime_ref(self);
    return axis == lh_ui_axis_vertical ? lh_ui_size_get_height(self) : lh_ui_size_get_width(self);
}

lh_void
lh_ui_size_set_along(lh_ui_size_t *self, lh_ui_axis_t axis, lh_ui_scalar_t value)
{
    lh_assert_runtime_ref(self);
    if (axis == lh_ui_axis_vertical)
    {
        lh_ui_size_set_height(self, value);
        return;
    }
    lh_ui_size_set_width(self, value);
}

lh_void
lh_ui_rect_init_along(lh_ui_rect_t *self, const lh_ui_rect_t *base, lh_ui_axis_t axis, lh_ui_scalar_t start,
                      lh_ui_scalar_t length)
{
    lh_ui_point_t *origin;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(base);
    *self = *base;
    origin = lh_ui_rect_get_origin(self);
    lh_ui_point_set_along(origin, axis, lh_ui_point_get_along(origin, axis) + start);
    lh_ui_size_set_along(lh_ui_rect_get_size(self), axis, length);
}
