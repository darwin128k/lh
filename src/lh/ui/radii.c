/**
 * @file radii.c
 * @brief Implementation of `lh/ui/radii.h`.
 */
#include <lh/assert.h>

#include <lh/math.h>
#include <lh/ui/radii.h>
#include <lh/ui/radius.h>

lh_void
lh_ui_radii_init(lh_ui_radii_t *self, lh_ui_scalar_t top_left, lh_ui_scalar_t top_right,
                 lh_ui_scalar_t bottom_right, lh_ui_scalar_t bottom_left)
{
    lh_ui_radii_set_top_left(self, top_left);
    lh_ui_radii_set_top_right(self, top_right);
    lh_ui_radii_set_bottom_right(self, bottom_right);
    lh_ui_radii_set_bottom_left(self, bottom_left);
}

lh_void
lh_ui_radii_init_all(lh_ui_radii_t *self, lh_ui_scalar_t radius)
{
    lh_ui_radii_init(self, radius, radius, radius, radius);
}

lh_bool_t
lh_ui_radii_is_uniform(const lh_ui_radii_t *self)
{
    return (lh_ui_radii_get_top_left(self) == lh_ui_radii_get_top_right(self) &&
            lh_ui_radii_get_top_right(self) == lh_ui_radii_get_bottom_right(self) &&
            lh_ui_radii_get_bottom_right(self) == lh_ui_radii_get_bottom_left(self))
               ? lh_bool_true
               : lh_bool_false;
}

lh_ui_scalar_t
lh_ui_radii_get_min(const lh_ui_radii_t *self)
{
    return lh_math_min(
        lh_math_min(lh_ui_radii_get_top_left(self), lh_ui_radii_get_top_right(self)),
        lh_math_min(lh_ui_radii_get_bottom_right(self), lh_ui_radii_get_bottom_left(self)));
}

lh_ui_scalar_t
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
    lh_ui_scalar_t num = lh_ui_scalar(1);
    lh_ui_scalar_t den = lh_ui_scalar(1);

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);
    lh_ui_scalar_t width = lh_ui_size_get_width(size);
    lh_ui_scalar_t height = lh_ui_size_get_height(size);
    lh_ui_radii_t result = lh_ptr_deref(self);
    lh_ui_radii_set_top_left(
        lh_addr_of(result),
        lh_math_max(lh_ui_radii_get_top_left(lh_addr_of(result)), lh_ui_scalar(0)));
    lh_ui_radii_set_top_right(
        lh_addr_of(result),
        lh_math_max(lh_ui_radii_get_top_right(lh_addr_of(result)), lh_ui_scalar(0)));
    lh_ui_radii_set_bottom_right(
        lh_addr_of(result),
        lh_math_max(lh_ui_radii_get_bottom_right(lh_addr_of(result)), lh_ui_scalar(0)));
    lh_ui_radii_set_bottom_left(
        lh_addr_of(result),
        lh_math_max(lh_ui_radii_get_bottom_left(lh_addr_of(result)), lh_ui_scalar(0)));
    lh_ui_scalar_t sums[4];
    /* The side that is most over-full decides one factor for all four corners, so a
       rounded tab stays the same shape, smaller, rather than turning into another. */
    sums[0] = lh_ui_radii_get_top_left(lh_addr_of(result)) +
              lh_ui_radii_get_top_right(lh_addr_of(result));
    lh_ui_scalar_t sides[4];
    sides[0] = width;
    sums[1] = lh_ui_radii_get_bottom_left(lh_addr_of(result)) +
              lh_ui_radii_get_bottom_right(lh_addr_of(result));
    sides[1] = width;
    sums[2] = lh_ui_radii_get_top_left(lh_addr_of(result)) +
              lh_ui_radii_get_bottom_left(lh_addr_of(result));
    sides[2] = height;
    sums[3] = lh_ui_radii_get_top_right(lh_addr_of(result)) +
              lh_ui_radii_get_bottom_right(lh_addr_of(result));
    sides[3] = height;
    for (int i = 0; i < 4; ++i)
    {
        /* sides[i] / sums[i] < num / den, compared without dividing. */
        if (sums[i] > sides[i] && sides[i] * den < num * sums[i])
        {
            num = sides[i];
            den = sums[i];
        }
    }
    lh_ui_radii_set_top_left(
        lh_addr_of(result),
        lh_ui_radii_scale(lh_ui_radii_get_top_left(lh_addr_of(result)), num, den));
    lh_ui_radii_set_top_right(
        lh_addr_of(result),
        lh_ui_radii_scale(lh_ui_radii_get_top_right(lh_addr_of(result)), num, den));
    lh_ui_radii_set_bottom_right(
        lh_addr_of(result),
        lh_ui_radii_scale(lh_ui_radii_get_bottom_right(lh_addr_of(result)), num, den));
    lh_ui_radii_set_bottom_left(
        lh_addr_of(result),
        lh_ui_radii_scale(lh_ui_radii_get_bottom_left(lh_addr_of(result)), num, den));
    /* And none past half the shorter side, where a corner would reach into the
       quarter of the box that belongs to its neighbour. */
    lh_ui_scalar_t half = lh_ui_radius_get_max(rect);
    lh_ui_radii_set_top_left(lh_addr_of(result),
                             lh_math_min(lh_ui_radii_get_top_left(lh_addr_of(result)), half));
    lh_ui_radii_set_top_right(lh_addr_of(result),
                              lh_math_min(lh_ui_radii_get_top_right(lh_addr_of(result)), half));
    lh_ui_radii_set_bottom_right(
        lh_addr_of(result), lh_math_min(lh_ui_radii_get_bottom_right(lh_addr_of(result)), half));
    lh_ui_radii_set_bottom_left(lh_addr_of(result),
                                lh_math_min(lh_ui_radii_get_bottom_left(lh_addr_of(result)), half));
    lh_ptr_deref(out) = result;
}

lh_ui_scalar_t
lh_ui_radii_get_at(const lh_ui_radii_t *self, const lh_ui_rect_t *rect, lh_s32_t x, lh_s32_t y)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);
    /* Pixel centres against the middle, both doubled, so an odd box has no half. */
    const lh_bool_t left = lh_cast_static(lh_ui_scalar_t, (2 * x + 1)) <
                                   2 * lh_ui_point_get_x(origin) + lh_ui_size_get_width(size)
                               ? lh_bool_true
                               : lh_bool_false;
    const lh_bool_t top = lh_cast_static(lh_ui_scalar_t, (2 * y + 1)) <
                                  2 * lh_ui_point_get_y(origin) + lh_ui_size_get_height(size)
                              ? lh_bool_true
                              : lh_bool_false;

    if (top)
    {
        return left ? lh_ui_radii_get_top_left(self) : lh_ui_radii_get_top_right(self);
    }
    return left ? lh_ui_radii_get_bottom_left(self) : lh_ui_radii_get_bottom_right(self);
}

lh_ui_scalar_t
lh_ui_radii_get_top_left(const lh_ui_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top_left;
}

lh_void
lh_ui_radii_set_top_left(lh_ui_radii_t *self, lh_ui_scalar_t top_left)
{
    lh_assert_runtime_ref(self);
    self->top_left = top_left;
}

lh_ui_scalar_t
lh_ui_radii_get_top_right(const lh_ui_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top_right;
}

lh_void
lh_ui_radii_set_top_right(lh_ui_radii_t *self, lh_ui_scalar_t top_right)
{
    lh_assert_runtime_ref(self);
    self->top_right = top_right;
}

lh_ui_scalar_t
lh_ui_radii_get_bottom_right(const lh_ui_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom_right;
}

lh_void
lh_ui_radii_set_bottom_right(lh_ui_radii_t *self, lh_ui_scalar_t bottom_right)
{
    lh_assert_runtime_ref(self);
    self->bottom_right = bottom_right;
}

lh_ui_scalar_t
lh_ui_radii_get_bottom_left(const lh_ui_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom_left;
}

lh_void
lh_ui_radii_set_bottom_left(lh_ui_radii_t *self, lh_ui_scalar_t bottom_left)
{
    lh_assert_runtime_ref(self);
    self->bottom_left = bottom_left;
}
