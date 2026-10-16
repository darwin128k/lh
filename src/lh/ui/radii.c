/**
 * @file radii.c
 * @brief Implementation of `lh/ui/radii.h`.
 */

#include <lh/math.h>
#include <lh/ui/radii.h>
#include <lh/ui/radius.h>

lh_void
lh_ui_radii_init(lh_ui_radii_t *self, lh_ui_scalar_t top_left, lh_ui_scalar_t top_right,
                 lh_ui_scalar_t bottom_right, lh_ui_scalar_t bottom_left)
{
    lh_assert_runtime_ref(self);
    self->top_left = top_left;
    self->top_right = top_right;
    self->bottom_right = bottom_right;
    self->bottom_left = bottom_left;
}

lh_void
lh_ui_radii_init_all(lh_ui_radii_t *self, lh_ui_scalar_t radius)
{
    lh_ui_radii_init(self, radius, radius, radius, radius);
}

lh_bool_t
lh_ui_radii_is_uniform(const lh_ui_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return (self->top_left == self->top_right && self->top_right == self->bottom_right &&
            self->bottom_right == self->bottom_left)
               ? lh_bool_true
               : lh_bool_false;
}

lh_ui_scalar_t
lh_ui_radii_get_min(const lh_ui_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_math_min(lh_math_min(self->top_left, self->top_right),
                       lh_math_min(self->bottom_right, self->bottom_left));
}

/** @p radius times @p num / @p den when @p num < @p den, else @p radius: the CSS factor,
   kept in the scalar type so a build with no FPU rounds down rather than up. */
static lh_ui_scalar_t
lh_ui_radii_scale(lh_ui_scalar_t radius, lh_ui_scalar_t num, lh_ui_scalar_t den)
{
    if (den <= lh_ui_scalar(0) || num >= den)
    {
        return radius;
    }
    return radius * num / den;
}

lh_void
lh_ui_radii_clamp(const lh_ui_radii_t *self, const lh_ui_rect_t *rect, lh_ui_radii_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_ui_size_t *size;
    lh_ui_scalar_t width;
    lh_ui_scalar_t height;
    lh_ui_scalar_t num = lh_ui_scalar(1);
    lh_ui_scalar_t den = lh_ui_scalar(1);
    lh_ui_scalar_t sums[4];
    lh_ui_scalar_t sides[4];
    lh_ui_scalar_t half;
    lh_ui_radii_t result;
    int i;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    size = lh_ui_rect_get_size_as_const(rect);
    width = lh_ui_size_get_width(size);
    height = lh_ui_size_get_height(size);
    result = lh_ptr_deref(self);
    result.top_left = lh_math_max(result.top_left, lh_ui_scalar(0));
    result.top_right = lh_math_max(result.top_right, lh_ui_scalar(0));
    result.bottom_right = lh_math_max(result.bottom_right, lh_ui_scalar(0));
    result.bottom_left = lh_math_max(result.bottom_left, lh_ui_scalar(0));
    /* The side that is most over-full decides one factor for all four corners, so a
       rounded tab stays the same shape, smaller, rather than turning into another. */
    sums[0] = result.top_left + result.top_right;
    sides[0] = width;
    sums[1] = result.bottom_left + result.bottom_right;
    sides[1] = width;
    sums[2] = result.top_left + result.bottom_left;
    sides[2] = height;
    sums[3] = result.top_right + result.bottom_right;
    sides[3] = height;
    for (i = 0; i < 4; ++i)
    {
        /* sides[i] / sums[i] < num / den, compared without dividing. */
        if (sums[i] > sides[i] && sides[i] * den < num * sums[i])
        {
            num = sides[i];
            den = sums[i];
        }
    }
    result.top_left = lh_ui_radii_scale(result.top_left, num, den);
    result.top_right = lh_ui_radii_scale(result.top_right, num, den);
    result.bottom_right = lh_ui_radii_scale(result.bottom_right, num, den);
    result.bottom_left = lh_ui_radii_scale(result.bottom_left, num, den);
    /* And none past half the shorter side, where a corner would reach into the
       quarter of the box that belongs to its neighbour. */
    half = lh_ui_radius_get_max(rect);
    result.top_left = lh_math_min(result.top_left, half);
    result.top_right = lh_math_min(result.top_right, half);
    result.bottom_right = lh_math_min(result.bottom_right, half);
    result.bottom_left = lh_math_min(result.bottom_left, half);
    lh_ptr_deref(out) = result;
}

lh_ui_scalar_t
lh_ui_radii_get_at(const lh_ui_radii_t *self, const lh_ui_rect_t *rect, lh_s32_t x, lh_s32_t y)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    /* Pixel centres against the middle, both doubled, so an odd box has no half. */
    const lh_bool_t left =
        lh_cast_static(lh_ui_scalar_t, (2 * x + 1))< 2 * lh_ui_point_get_x(origin) + lh_ui_size_get_width(size) ? lh_bool_true
                                                                                                : lh_bool_false;
    const lh_bool_t top =
        lh_cast_static(lh_ui_scalar_t, (2 * y + 1))< 2 * lh_ui_point_get_y(origin) + lh_ui_size_get_height(size) ? lh_bool_true
                                                                                                 : lh_bool_false;

    lh_assert_runtime_ref(self);
    if (top)
    {
        return left ? self->top_left : self->top_right;
    }
    return left ? self->bottom_left : self->bottom_right;
}
