/**
 * @file insets.c
 * @brief Implementation of `lh/gl/insets.h`.
 */
#include <lh/core/assert.h>

#include <lh/core/math.h>
#include <lh/gl/insets.h>

lh_void
lh_gl_insets_init(lh_gl_insets_t *self, lh_gl_scalar_t left, lh_gl_scalar_t top,
                  lh_gl_scalar_t right, lh_gl_scalar_t bottom)
{
    lh_gl_insets_set_left(self, left);
    lh_gl_insets_set_top(self, top);
    lh_gl_insets_set_right(self, right);
    lh_gl_insets_set_bottom(self, bottom);
}

lh_void
lh_gl_insets_init_all(lh_gl_insets_t *self, lh_gl_scalar_t value)
{
    lh_gl_insets_init(self, value, value, value, value);
}

lh_gl_scalar_t
lh_gl_insets_get_left(const lh_gl_insets_t *self)
{
    lh_assert_runtime_ref(self);
    return self->left;
}

lh_gl_scalar_t
lh_gl_insets_get_top(const lh_gl_insets_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top;
}

lh_gl_scalar_t
lh_gl_insets_get_right(const lh_gl_insets_t *self)
{
    lh_assert_runtime_ref(self);
    return self->right;
}

lh_gl_scalar_t
lh_gl_insets_get_bottom(const lh_gl_insets_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom;
}

lh_gl_scalar_t
lh_gl_insets_get_start(const lh_gl_insets_t *self, lh_gl_axis_t axis)
{
    return axis == lh_gl_axis_vertical ? lh_gl_insets_get_top(self) : lh_gl_insets_get_left(self);
}

lh_gl_scalar_t
lh_gl_insets_get_end(const lh_gl_insets_t *self, lh_gl_axis_t axis)
{
    return axis == lh_gl_axis_vertical ? lh_gl_insets_get_bottom(self)
                                       : lh_gl_insets_get_right(self);
}

lh_bool_t
lh_gl_insets_is_zero(const lh_gl_insets_t *self)
{
    return lh_gl_insets_get_left(self) == lh_gl_scalar(0) &&
                   lh_gl_insets_get_top(self) == lh_gl_scalar(0) &&
                   lh_gl_insets_get_right(self) == lh_gl_scalar(0) &&
                   lh_gl_insets_get_bottom(self) == lh_gl_scalar(0)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_gl_insets_shrink(const lh_gl_insets_t *self, const lh_gl_rect_t *rect, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    lh_gl_rect_t inner;

    lh_gl_rect_init(lh_addr_of(inner), lh_gl_point_get_x(origin) + lh_gl_insets_get_left(self),
                    lh_gl_point_get_y(origin) + lh_gl_insets_get_top(self),
                    lh_gl_scalar_slack(lh_gl_size_get_width(size) - lh_gl_insets_get_left(self),
                                       lh_gl_insets_get_right(self)),
                    lh_gl_scalar_slack(lh_gl_size_get_height(size) - lh_gl_insets_get_top(self),
                                       lh_gl_insets_get_bottom(self)));
    lh_ptr_deref(out) = inner;
}

lh_void
lh_gl_insets_set_left(lh_gl_insets_t *self, lh_gl_scalar_t left)
{
    lh_assert_runtime_ref(self);
    self->left = left;
}

lh_void
lh_gl_insets_set_top(lh_gl_insets_t *self, lh_gl_scalar_t top)
{
    lh_assert_runtime_ref(self);
    self->top = top;
}

lh_void
lh_gl_insets_set_right(lh_gl_insets_t *self, lh_gl_scalar_t right)
{
    lh_assert_runtime_ref(self);
    self->right = right;
}

lh_void
lh_gl_insets_set_bottom(lh_gl_insets_t *self, lh_gl_scalar_t bottom)
{
    lh_assert_runtime_ref(self);
    self->bottom = bottom;
}
