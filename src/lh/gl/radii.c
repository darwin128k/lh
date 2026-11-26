/**
 * @file radii.c
 * @brief Implementation of `lh/gl/radii.h`.
 */

#include <lh/gl/radii.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/gl/radius.h>

lh_gl_scalar_t
lh_gl_radii_scale(lh_gl_scalar_t radius, lh_gl_scalar_t num, lh_gl_scalar_t den)
{
    if (den <= lh_gl_scalar(0) || num >= den)
    {
        return radius;
    }
    return radius * num / den;
}

lh_gl_scalar_t
lh_gl_radii_get_top_left(const lh_gl_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top_left;
}

lh_void
lh_gl_radii_set_top_left(lh_gl_radii_t *self, lh_gl_scalar_t top_left)
{
    lh_assert_runtime_ref(self);
    self->top_left = top_left;
}

lh_gl_scalar_t
lh_gl_radii_get_top_right(const lh_gl_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top_right;
}

lh_void
lh_gl_radii_set_top_right(lh_gl_radii_t *self, lh_gl_scalar_t top_right)
{
    lh_assert_runtime_ref(self);
    self->top_right = top_right;
}

lh_gl_scalar_t
lh_gl_radii_get_bottom_right(const lh_gl_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom_right;
}

lh_void
lh_gl_radii_set_bottom_right(lh_gl_radii_t *self, lh_gl_scalar_t bottom_right)
{
    lh_assert_runtime_ref(self);
    self->bottom_right = bottom_right;
}

lh_gl_scalar_t
lh_gl_radii_get_bottom_left(const lh_gl_radii_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bottom_left;
}

lh_void
lh_gl_radii_set_bottom_left(lh_gl_radii_t *self, lh_gl_scalar_t bottom_left)
{
    lh_assert_runtime_ref(self);
    self->bottom_left = bottom_left;
}

lh_void
lh_gl_radii_tighten(lh_gl_scalar_t side, lh_gl_scalar_t sum, lh_gl_scalar_t *num,
                    lh_gl_scalar_t *den)
{
    /* side / sum < num / den, compared without dividing. */
    if (sum > side && side * lh_ptr_deref(den) < lh_ptr_deref(num) * sum)
    {
        lh_ptr_deref(num) = side;
        lh_ptr_deref(den) = sum;
    }
}

lh_void
lh_gl_radii_init(lh_gl_radii_t *self, lh_gl_scalar_t top_left, lh_gl_scalar_t top_right,
                 lh_gl_scalar_t bottom_right, lh_gl_scalar_t bottom_left)
{
    lh_gl_radii_set_top_left(self, top_left);
    lh_gl_radii_set_top_right(self, top_right);
    lh_gl_radii_set_bottom_right(self, bottom_right);
    lh_gl_radii_set_bottom_left(self, bottom_left);
}

lh_bool_t
lh_gl_radii_is_uniform(const lh_gl_radii_t *self)
{
    return (lh_gl_radii_get_top_left(self) == lh_gl_radii_get_top_right(self) &&
            lh_gl_radii_get_top_right(self) == lh_gl_radii_get_bottom_right(self) &&
            lh_gl_radii_get_bottom_right(self) == lh_gl_radii_get_bottom_left(self))
               ? lh_bool_true
               : lh_bool_false;
}

lh_gl_scalar_t
lh_gl_radii_get_min(const lh_gl_radii_t *self)
{
    return lh_math_min(
        lh_math_min(lh_gl_radii_get_top_left(self), lh_gl_radii_get_top_right(self)),
        lh_math_min(lh_gl_radii_get_bottom_right(self), lh_gl_radii_get_bottom_left(self)));
}

lh_gl_scalar_t
lh_gl_radii_get_max(const lh_gl_radii_t *self)
{
    return lh_math_max(
        lh_math_max(lh_gl_radii_get_top_left(self), lh_gl_radii_get_top_right(self)),
        lh_math_max(lh_gl_radii_get_bottom_right(self), lh_gl_radii_get_bottom_left(self)));
}

lh_void
lh_gl_radii_get_fit(const lh_gl_radii_t *self, const lh_gl_size_t *size, lh_gl_scalar_t *num,
                    lh_gl_scalar_t *den)
{
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);
    lh_gl_scalar_t top_left = lh_gl_radii_get_top_left(self);
    lh_gl_scalar_t top_right = lh_gl_radii_get_top_right(self);
    lh_gl_scalar_t bottom_right = lh_gl_radii_get_bottom_right(self);
    lh_gl_scalar_t bottom_left = lh_gl_radii_get_bottom_left(self);

    lh_ptr_deref(num) = lh_gl_scalar(1);
    lh_ptr_deref(den) = lh_gl_scalar(1);
    lh_gl_radii_tighten(width, top_left + top_right, num, den);
    lh_gl_radii_tighten(width, bottom_left + bottom_right, num, den);
    lh_gl_radii_tighten(height, top_left + bottom_left, num, den);
    lh_gl_radii_tighten(height, top_right + bottom_right, num, den);
}

lh_gl_scalar_t
lh_gl_radii_get_at(const lh_gl_radii_t *self, const lh_gl_rect_t *rect, lh_s32_t x, lh_s32_t y)
{
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    /* Pixel centres against the middle, both doubled, so an odd box has no half. */
    const lh_bool_t left = lh_cast_static(lh_gl_scalar_t, (2 * x + 1)) <
                                   2 * lh_gl_point_get_x(origin) + lh_gl_size_get_width(size)
                               ? lh_bool_true
                               : lh_bool_false;
    const lh_bool_t top = lh_cast_static(lh_gl_scalar_t, (2 * y + 1)) <
                                  2 * lh_gl_point_get_y(origin) + lh_gl_size_get_height(size)
                              ? lh_bool_true
                              : lh_bool_false;

    if (top)
    {
        return left ? lh_gl_radii_get_top_left(self) : lh_gl_radii_get_top_right(self);
    }
    return left ? lh_gl_radii_get_bottom_left(self) : lh_gl_radii_get_bottom_right(self);
}

lh_void
lh_gl_radii_scale_all(const lh_gl_radii_t *self, lh_gl_scalar_t num, lh_gl_scalar_t den,
                      lh_gl_radii_t *out)
{
    lh_gl_radii_init(out, lh_gl_radii_scale(lh_gl_radii_get_top_left(self), num, den),
                     lh_gl_radii_scale(lh_gl_radii_get_top_right(self), num, den),
                     lh_gl_radii_scale(lh_gl_radii_get_bottom_right(self), num, den),
                     lh_gl_radii_scale(lh_gl_radii_get_bottom_left(self), num, den));
}

lh_void
lh_gl_radii_limit(const lh_gl_radii_t *self, lh_gl_scalar_t max, lh_gl_radii_t *out)
{
    lh_gl_radii_init(out, lh_math_min(lh_gl_radii_get_top_left(self), max),
                     lh_math_min(lh_gl_radii_get_top_right(self), max),
                     lh_math_min(lh_gl_radii_get_bottom_right(self), max),
                     lh_math_min(lh_gl_radii_get_bottom_left(self), max));
}

lh_void
lh_gl_radii_inset(const lh_gl_radii_t *self, lh_gl_scalar_t by, lh_gl_radii_t *out)
{
    lh_gl_radii_init(out, lh_math_max(lh_gl_radii_get_top_left(self) - by, lh_gl_scalar(0)),
                     lh_math_max(lh_gl_radii_get_top_right(self) - by, lh_gl_scalar(0)),
                     lh_math_max(lh_gl_radii_get_bottom_right(self) - by, lh_gl_scalar(0)),
                     lh_math_max(lh_gl_radii_get_bottom_left(self) - by, lh_gl_scalar(0)));
}

lh_void
lh_gl_radii_init_all(lh_gl_radii_t *self, lh_gl_scalar_t radius)
{
    lh_gl_radii_init(self, radius, radius, radius, radius);
}

lh_void
lh_gl_radii_clamp(const lh_gl_radii_t *self, const lh_gl_rect_t *rect, lh_gl_radii_t *out)
{
    lh_gl_radii_t result;

    /* None below 0 first: a negative corner would make a side look less full. */
    lh_gl_radii_inset(self, lh_gl_scalar(0), lh_addr_of(result));
    /* The side that is most over-full decides one factor for all four corners, so a
       rounded tab stays the same shape, smaller, rather than turning into another. */
    lh_gl_scalar_t num;
    lh_gl_scalar_t den;
    lh_gl_radii_get_fit(lh_addr_of(result), lh_gl_rect_get_size(rect), lh_addr_of(num),
                        lh_addr_of(den));
    lh_gl_radii_scale_all(lh_addr_of(result), num, den, lh_addr_of(result));
    /* And none past half the shorter side, where a corner would reach into the
       quarter of the box that belongs to its neighbour. */
    lh_gl_radii_limit(lh_addr_of(result), lh_gl_radius_get_max(rect), out);
}
