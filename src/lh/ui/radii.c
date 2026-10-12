/**
 * @file radii.c
 * @brief Implementation of `lh/ui/radii.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/ui/radii.h>
#include <lh/ui/radius.h>
#include <lh/util/addr.h>

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

/* @p radius times @p num / @p den when @p num < @p den, else @p radius: the CSS factor,
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

lh_ui_radii_t
lh_ui_radii_clamp(const lh_ui_radii_t *self, const lh_ui_rect_t *rect)
{
    const lh_ui_size_t *size;
    lh_ui_scalar_t width;
    lh_ui_scalar_t height;
    lh_ui_scalar_t num = lh_ui_scalar(1);
    lh_ui_scalar_t den = lh_ui_scalar(1);
    lh_ui_scalar_t sums[4];
    lh_ui_scalar_t sides[4];
    lh_ui_scalar_t half;
    lh_ui_radii_t out;
    int i;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    size = lh_ui_rect_get_size_as_const(rect);
    width = lh_ui_size_get_width(size);
    height = lh_ui_size_get_height(size);
    out = *self;
    out.top_left = lh_math_max(out.top_left, lh_ui_scalar(0));
    out.top_right = lh_math_max(out.top_right, lh_ui_scalar(0));
    out.bottom_right = lh_math_max(out.bottom_right, lh_ui_scalar(0));
    out.bottom_left = lh_math_max(out.bottom_left, lh_ui_scalar(0));
    /* The side that is most over-full decides one factor for all four corners, so a
       rounded tab stays the same shape, smaller, rather than turning into another. */
    sums[0] = out.top_left + out.top_right;
    sides[0] = width;
    sums[1] = out.bottom_left + out.bottom_right;
    sides[1] = width;
    sums[2] = out.top_left + out.bottom_left;
    sides[2] = height;
    sums[3] = out.top_right + out.bottom_right;
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
    out.top_left = lh_ui_radii_scale(out.top_left, num, den);
    out.top_right = lh_ui_radii_scale(out.top_right, num, den);
    out.bottom_right = lh_ui_radii_scale(out.bottom_right, num, den);
    out.bottom_left = lh_ui_radii_scale(out.bottom_left, num, den);
    /* And none past half the shorter side, where a corner would reach into the
       quarter of the box that belongs to its neighbour. */
    half = lh_ui_radius_get_max(rect);
    out.top_left = lh_math_min(out.top_left, half);
    out.top_right = lh_math_min(out.top_right, half);
    out.bottom_right = lh_math_min(out.bottom_right, half);
    out.bottom_left = lh_math_min(out.bottom_left, half);
    return out;
}

lh_ui_scalar_t
lh_ui_radii_get_at(const lh_ui_radii_t *self, const lh_ui_rect_t *rect, lh_s32_t x, lh_s32_t y)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    /* Pixel centres against the middle, both doubled, so an odd box has no half. */
    const lh_bool_t left =
        (lh_ui_scalar_t)(2 * x + 1) < 2 * lh_ui_point_get_x(origin) + lh_ui_size_get_width(size) ? lh_bool_true
                                                                                                : lh_bool_false;
    const lh_bool_t top =
        (lh_ui_scalar_t)(2 * y + 1) < 2 * lh_ui_point_get_y(origin) + lh_ui_size_get_height(size) ? lh_bool_true
                                                                                                 : lh_bool_false;

    lh_assert_runtime_ref(self);
    if (top)
    {
        return left ? self->top_left : self->top_right;
    }
    return left ? self->bottom_left : self->bottom_right;
}
