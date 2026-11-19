/**
 * @file shadow.c
 * @brief Implementation of `lh/core/pixel/shadow.h`.
 */

#include <lh/core/pixel/shadow.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/core/memory.h>
#include <lh/core/pixel/shadow/ramp.h>
#include <lh/core/util/return.h>

const lh_pixel_round_box_t *
lh_pixel_shadow_get_box(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_void
lh_pixel_shadow_set_box(lh_pixel_shadow_t *self, const lh_pixel_round_box_t *box)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(box);
    self->box = lh_ptr_deref(box);
}

lh_s32_t
lh_pixel_shadow_get_cx(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cx;
}

lh_void
lh_pixel_shadow_set_cx(lh_pixel_shadow_t *self, lh_s32_t cx)
{
    lh_assert_runtime_ref(self);
    self->cx = cx;
}

lh_s32_t
lh_pixel_shadow_get_cy(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cy;
}

lh_void
lh_pixel_shadow_set_cy(lh_pixel_shadow_t *self, lh_s32_t cy)
{
    lh_assert_runtime_ref(self);
    self->cy = cy;
}

lh_s32_t
lh_pixel_shadow_get_hx(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hx;
}

lh_void
lh_pixel_shadow_set_hx(lh_pixel_shadow_t *self, lh_s32_t hx)
{
    lh_assert_runtime_ref(self);
    self->hx = hx;
}

lh_s32_t
lh_pixel_shadow_get_hy(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hy;
}

lh_void
lh_pixel_shadow_set_hy(lh_pixel_shadow_t *self, lh_s32_t hy)
{
    lh_assert_runtime_ref(self);
    self->hy = hy;
}

lh_s32_t
lh_pixel_shadow_get_r(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_void
lh_pixel_shadow_set_r(lh_pixel_shadow_t *self, lh_s32_t r)
{
    lh_assert_runtime_ref(self);
    self->r = r;
}

lh_s32_t
lh_pixel_shadow_get_spread(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->spread;
}

lh_void
lh_pixel_shadow_set_spread(lh_pixel_shadow_t *self, lh_s32_t spread)
{
    lh_assert_runtime_ref(self);
    self->spread = spread;
}

lh_s32_t
lh_pixel_shadow_get_peak(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->peak;
}

lh_void
lh_pixel_shadow_set_peak(lh_pixel_shadow_t *self, lh_s32_t peak)
{
    lh_assert_runtime_ref(self);
    self->peak = peak;
}

lh_s64_t
lh_pixel_shadow_get_start(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_pixel_shadow_set_start(lh_pixel_shadow_t *self, lh_s64_t start)
{
    lh_assert_runtime_ref(self);
    self->start = start;
}

lh_s64_t
lh_pixel_shadow_get_step(const lh_pixel_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

lh_void
lh_pixel_shadow_set_step(lh_pixel_shadow_t *self, lh_s64_t step)
{
    lh_assert_runtime_ref(self);
    self->step = step;
}

lh_void
lh_pixel_shadow_init(lh_pixel_shadow_t *self, const lh_pixel_round_box_t *box, lh_s64_t offset_x,
                     lh_s64_t offset_y, lh_s64_t spread, lh_byte_t peak)
{
    const lh_s64_t lo_x = lh_pixel_round_box_get_lo_x(box);
    const lh_s64_t lo_y = lh_pixel_round_box_get_lo_y(box);
    const lh_s64_t width = lh_pixel_round_box_get_hi_x(box) - lo_x;
    const lh_s64_t height = lh_pixel_round_box_get_hi_y(box) - lo_y;
    const lh_s32_t half_x = lh_cast_static(lh_s32_t, width) / 2;
    const lh_s32_t half_y = lh_cast_static(lh_s32_t, height) / 2;
    const lh_s32_t r = lh_cast_static(lh_s32_t, lh_pixel_round_box_get_r(box));
    const lh_s32_t reach = lh_cast_static(lh_s32_t, spread);
    const lh_s32_t alpha = (reach <= 0 || width <= 0 || height <= 0) ? 0 : peak;

    lh_pixel_shadow_set_box(self, box);
    lh_pixel_shadow_set_r(self, r);
    lh_pixel_shadow_set_cx(self, lh_cast_static(lh_s32_t, lo_x + offset_x) + half_x);
    lh_pixel_shadow_set_cy(self, lh_cast_static(lh_s32_t, lo_y + offset_y) + half_y);
    lh_pixel_shadow_set_hx(self, half_x - r);
    lh_pixel_shadow_set_hy(self, half_y - r);
    lh_pixel_shadow_set_spread(self, reach);
    lh_pixel_shadow_set_peak(self, alpha);
    /* The fade straddles the edge: the peak a spread inside it, nothing a spread
       outside, and a straight line between -- `peak * (spread - d) / (2 * spread)`,
       rounded, which is a start and a step once both are in 16.16. */
    if (alpha > 0)
    {
        lh_pixel_shadow_set_step(self, (lh_cast_static(lh_s64_t, alpha) << 16) /
                                           (2 * lh_cast_static(lh_s64_t, reach)));
        lh_pixel_shadow_set_start(self, (lh_cast_static(lh_s64_t, alpha) << 15) + (1 << 15));
        return;
    }
    lh_pixel_shadow_set_step(self, 0);
    lh_pixel_shadow_set_start(self, 0);
}

lh_bool_t
lh_pixel_shadow_is_equal(const lh_pixel_shadow_t *self, const lh_pixel_shadow_t *other)
{
    /* The fade's start and step follow from the peak and the spread, and the
       straight reaches from the box and the radius: what is left is compared. */
    return lh_pixel_round_box_is_equal(lh_pixel_shadow_get_box(self),
                                       lh_pixel_shadow_get_box(other)) &&
           lh_pixel_shadow_get_cx(self) == lh_pixel_shadow_get_cx(other) &&
           lh_pixel_shadow_get_cy(self) == lh_pixel_shadow_get_cy(other) &&
           lh_pixel_shadow_get_r(self) == lh_pixel_shadow_get_r(other) &&
           lh_pixel_shadow_get_spread(self) == lh_pixel_shadow_get_spread(other) &&
           lh_pixel_shadow_get_peak(self) == lh_pixel_shadow_get_peak(other);
}

lh_s32_t
lh_pixel_shadow_get_row(const lh_pixel_shadow_t *self, lh_s32_t y)
{
    const lh_s32_t py = y * LH_PIXEL_ROUND_SUBPIXEL + LH_PIXEL_ROUND_SUBPIXEL / 2;
    const lh_s32_t cy = lh_pixel_shadow_get_cy(self);

    return (py > cy ? py - cy : cy - py) - lh_pixel_shadow_get_hy(self);
}

lh_byte_t
lh_pixel_shadow_get_fade(const lh_pixel_shadow_t *self, lh_s32_t distance)
{
    const lh_s32_t spread = lh_pixel_shadow_get_spread(self);
    const lh_s32_t peak = lh_pixel_shadow_get_peak(self);

    lh_return_if(distance >= spread, 0);
    lh_return_if(distance <= lh_math_neg(spread), lh_cast_static(lh_byte_t, peak));
    const lh_s64_t fade = (lh_pixel_shadow_get_start(self) -
                           lh_cast_static(lh_s64_t, distance) * lh_pixel_shadow_get_step(self)) >>
                          16;

    return lh_cast_static(lh_byte_t, lh_math_clamp(fade, 0, peak));
}

lh_byte_t
lh_pixel_shadow_get_alpha(const lh_pixel_shadow_t *self, lh_s32_t row, lh_s32_t x, lh_s32_t y)
{
    lh_return_if(lh_pixel_shadow_get_peak(self) == 0, 0);
    const lh_pixel_round_box_t *box = lh_pixel_shadow_get_box(self);

    /* A pixel the box covers wholly is the box's. It is decided from the pixel's
       centre, as the box's own fill decides it, or the shadow's edge and the fill's
       would not meet on a rounded corner. */
    lh_return_if(x >= lh_pixel_round_box_get_left(box) && x < lh_pixel_round_box_get_right(box) &&
                     y >= lh_pixel_round_box_get_top(box) &&
                     y < lh_pixel_round_box_get_bottom(box) &&
                     lh_pixel_round_box_get_coverage(box, x, y) == 255U,
                 0);
    const lh_s32_t px = x * LH_PIXEL_ROUND_SUBPIXEL + LH_PIXEL_ROUND_SUBPIXEL / 2;
    const lh_s32_t dx =
        lh_math_abs(px - lh_pixel_shadow_get_cx(self)) - lh_pixel_shadow_get_hx(self);
    const lh_s32_t ox = lh_math_max(dx, 0);
    const lh_s32_t oy = lh_math_max(row, 0);
    /* The distance to the shifted rounded box: how far the pixel is outside it on
       each axis -- a square root only where it is outside both, at a corner -- or,
       inside it, how far in, which is negative. */
    lh_s32_t length = oy;

    if (ox != 0 && oy == 0)
    {
        length = ox;
    }
    else if (ox != 0)
    {
        length = lh_cast_static(
            lh_s32_t,
            lh_math_isqrt_u64(lh_cast_static(lh_u64_t, ox) * lh_cast_static(lh_u64_t, ox) +
                              lh_cast_static(lh_u64_t, oy) * lh_cast_static(lh_u64_t, oy)));
    }
    return lh_pixel_shadow_get_fade(self, length + lh_math_min(lh_math_max(dx, row), 0) -
                                              lh_pixel_shadow_get_r(self));
}

lh_void
lh_pixel_shadow_get_span(const lh_pixel_shadow_t *self, const struct lh_pixel_shadow_ramp *ramp,
                         lh_s32_t y, lh_s32_t x0, lh_s32_t x1, lh_byte_ptr out)
{
    lh_assert_runtime_ref(ramp);
    if (lh_pixel_shadow_get_peak(self) == 0)
    {
        if (x1 > x0)
        {
            lh_memory_set(out, lh_cast_static(lh_usize_t, x1 - x0), 0U);
        }
        return;
    }
    const lh_pixel_round_box_t *box = lh_pixel_shadow_get_box(self);
    const lh_s32_t row = lh_pixel_shadow_get_row(self, y);
    const lh_s32_t oy = lh_math_max(row, 0);
    const lh_s32_t cx = lh_pixel_shadow_get_cx(self);
    const lh_s32_t hx = lh_pixel_shadow_get_hx(self);
    const lh_s32_t r = lh_pixel_shadow_get_r(self);
    lh_s32_t full0 = x1;
    lh_s32_t full1 = x1;
    lh_s32_t px = x0 * LH_PIXEL_ROUND_SUBPIXEL + LH_PIXEL_ROUND_SUBPIXEL / 2;
    lh_s32_t level = 0;

    /* The pixels the box covers wholly, which are the box's: in a row of the box,
       the run ::lh_pixel_round_box_get_full_span finds -- every pixel of it at
       coverage 255 and every other pixel short of it. */
    if (y >= lh_pixel_round_box_get_top(box) && y < lh_pixel_round_box_get_bottom(box))
    {
        lh_pixel_round_box_get_full_span(box, y, lh_addr_of(full0), lh_addr_of(full1));
    }
    for (lh_s32_t x = x0; x < x1;)
    {
        const lh_s32_t dx = lh_math_abs(px - cx) - hx;
        const lh_s32_t ox = lh_math_max(dx, 0);
        lh_s32_t end = x + 1;

        if (x >= full0 && x < full1)
        {
            end = lh_math_min(full1, x1);
            lh_memory_set(out + (x - x0), lh_cast_static(lh_usize_t, end - x), 0U);
        }
        else if (ox == 0 && oy > 0)
        {
            /* Above or below the box and between its corners: every pixel is the
               same distance out, so the alpha is the row's, as far as the corner --
               where the pixel's centre passes `cx + hx` -- or the box's run. */
            end = lh_math_min(x + (cx + hx - px) / LH_PIXEL_ROUND_SUBPIXEL + 1, x1);
            end = x < full0 && full0 < end ? full0 : end;
            lh_memory_set(out + (x - x0), lh_cast_static(lh_usize_t, end - x),
                          lh_pixel_shadow_get_fade(self, oy - r));
        }
        else if (ox > 0 && oy > 0)
        {
            /* Past a corner, outside both edges: the alpha is the root's, read off the
               ramp from the squared distance, a step or two from the last pixel's. */
            level = lh_pixel_shadow_ramp_get_level(
                ramp,
                lh_cast_static(lh_u64_t, ox) * lh_cast_static(lh_u64_t, ox) +
                    lh_cast_static(lh_u64_t, oy) * lh_cast_static(lh_u64_t, oy),
                level);
            out[x - x0] = lh_cast_static(lh_byte_t, level);
        }
        else
        {
            out[x - x0] = lh_pixel_shadow_get_fade(
                self, (ox > 0 ? ox : oy) + lh_math_min(lh_math_max(dx, row), 0) - r);
        }
        px += (end - x) * LH_PIXEL_ROUND_SUBPIXEL;
        x = end;
    }
}
