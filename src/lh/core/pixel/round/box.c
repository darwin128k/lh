/**
 * @file box.c
 * @brief Implementation of `lh/core/pixel/round/box.h`.
 */

#include <lh/core/pixel/round/box.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>

lh_s64_t
lh_pixel_round_box_get_lo_x(const lh_pixel_round_box_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lo_x;
}

lh_void
lh_pixel_round_box_set_lo_x(lh_pixel_round_box_t *self, lh_s64_t lo_x)
{
    lh_assert_runtime_ref(self);
    self->lo_x = lo_x;
}

lh_s64_t
lh_pixel_round_box_get_lo_y(const lh_pixel_round_box_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lo_y;
}

lh_void
lh_pixel_round_box_set_lo_y(lh_pixel_round_box_t *self, lh_s64_t lo_y)
{
    lh_assert_runtime_ref(self);
    self->lo_y = lo_y;
}

lh_s64_t
lh_pixel_round_box_get_hi_x(const lh_pixel_round_box_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hi_x;
}

lh_void
lh_pixel_round_box_set_hi_x(lh_pixel_round_box_t *self, lh_s64_t hi_x)
{
    lh_assert_runtime_ref(self);
    self->hi_x = hi_x;
}

lh_s64_t
lh_pixel_round_box_get_hi_y(const lh_pixel_round_box_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hi_y;
}

lh_void
lh_pixel_round_box_set_hi_y(lh_pixel_round_box_t *self, lh_s64_t hi_y)
{
    lh_assert_runtime_ref(self);
    self->hi_y = hi_y;
}

lh_s64_t
lh_pixel_round_box_get_r(const lh_pixel_round_box_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_void
lh_pixel_round_box_set_r(lh_pixel_round_box_t *self, lh_s64_t r)
{
    lh_assert_runtime_ref(self);
    self->r = r;
}

lh_void
lh_pixel_round_box_init(lh_pixel_round_box_t *self, lh_s64_t lo_x, lh_s64_t lo_y, lh_s64_t hi_x,
                        lh_s64_t hi_y, lh_s64_t r)
{
    lh_pixel_round_box_set_lo_x(self, lo_x);
    lh_pixel_round_box_set_lo_y(self, lo_y);
    lh_pixel_round_box_set_hi_x(self, hi_x);
    lh_pixel_round_box_set_hi_y(self, hi_y);
    lh_pixel_round_box_set_r(self, r);
}

lh_s32_t
lh_pixel_round_box_get_left(const lh_pixel_round_box_t *self)
{
    return lh_cast_static(
        lh_s32_t, lh_math_floor_div(lh_pixel_round_box_get_lo_x(self), LH_PIXEL_ROUND_SUBPIXEL));
}

lh_s32_t
lh_pixel_round_box_get_top(const lh_pixel_round_box_t *self)
{
    return lh_cast_static(
        lh_s32_t, lh_math_floor_div(lh_pixel_round_box_get_lo_y(self), LH_PIXEL_ROUND_SUBPIXEL));
}

lh_s32_t
lh_pixel_round_box_get_right(const lh_pixel_round_box_t *self)
{
    return lh_cast_static(
        lh_s32_t, lh_math_neg(lh_math_floor_div(lh_math_neg(lh_pixel_round_box_get_hi_x(self)),
                                                LH_PIXEL_ROUND_SUBPIXEL)));
}

lh_s32_t
lh_pixel_round_box_get_bottom(const lh_pixel_round_box_t *self)
{
    return lh_cast_static(
        lh_s32_t, lh_math_neg(lh_math_floor_div(lh_math_neg(lh_pixel_round_box_get_hi_y(self)),
                                                LH_PIXEL_ROUND_SUBPIXEL)));
}

lh_byte_t
lh_pixel_round_box_get_coverage(const lh_pixel_round_box_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_pixel_round_coverage(
        lh_pixel_round_box_get_lo_x(self), lh_pixel_round_box_get_lo_y(self),
        lh_pixel_round_box_get_hi_x(self), lh_pixel_round_box_get_hi_y(self),
        lh_pixel_round_box_get_r(self), x, y);
}

lh_void
lh_pixel_round_box_get_full_span(const lh_pixel_round_box_t *self, lh_s32_t y, lh_s32_t *out_x0,
                                 lh_s32_t *out_x1)
{
    lh_assert_runtime_ref(out_x0);
    const lh_s64_t half = LH_PIXEL_ROUND_SUBPIXEL / 2;
    const lh_s64_t lo = lh_pixel_round_box_get_lo_x(self);
    const lh_s64_t hi = lh_pixel_round_box_get_hi_x(self);
    const lh_s64_t r = lh_pixel_round_box_get_r(self);
    /* The columns whose centre is inside the box: from the first centre at or past
       its left edge to the first at or past its right one. */
    const lh_s32_t first = lh_cast_static(
        lh_s32_t, lh_math_neg(lh_math_floor_div(half - lo, LH_PIXEL_ROUND_SUBPIXEL)));
    const lh_s32_t last = lh_cast_static(
        lh_s32_t, lh_math_neg(lh_math_floor_div(half - hi, LH_PIXEL_ROUND_SUBPIXEL)));
    const lh_s64_t dy = lh_pixel_round_axis_distance(lh_pixel_round_pixel_center(y),
                                                     lh_pixel_round_box_get_lo_y(self),
                                                     lh_pixel_round_box_get_hi_y(self), r);

    if (dy < 0)
    {
        lh_ptr_deref(out_x0) = first;
        lh_ptr_deref(out_x1) = first;
        return;
    }
    /* Between the corners every pixel whose centre is inside is wholly covered. */
    if (dy == 0)
    {
        lh_ptr_deref(out_x0) = first;
        lh_ptr_deref(out_x1) = lh_math_max(last, first);
        return;
    }
    /* In a corner's rows a pixel is wholly covered while its centre is no further
       past the corner circle's centre than `reach`: ::lh_pixel_round_cover_from_square
       answers 255 while the whole root of `dx * dx + dy * dy` is at most `inner`,
       which is while the square is below `(inner + 1)^2`. */
    const lh_s64_t inner = r - half;
    const lh_s64_t room = (inner + 1) * (inner + 1) - 1 - dy * dy;
    const lh_s64_t reach =
        (inner >= 0 && room > 0)
            ? lh_cast_static(lh_s64_t, lh_math_isqrt_u64(lh_cast_static(lh_u64_t, room)))
            : 0;
    /* The first centre at or past `lo + r - reach`, and one past the last centre at
       or before `hi - r + reach`: each corner from its own edge, so a box at a
       fractional place is right on both sides. */
    const lh_s32_t from = lh_cast_static(
        lh_s32_t, lh_math_neg(lh_math_floor_div(half - (lo + r - reach), LH_PIXEL_ROUND_SUBPIXEL)));
    const lh_s32_t to = lh_cast_static(
        lh_s32_t, lh_math_floor_div(hi - r + reach - half, LH_PIXEL_ROUND_SUBPIXEL) + 1);

    lh_ptr_deref(out_x0) = lh_math_max(from, first);
    lh_ptr_deref(out_x1) = lh_math_max(lh_math_min(to, last), lh_ptr_deref(out_x0));
}
