/**
 * @file shape.c
 * @brief Implementation of `lh/ui/shadow/shape.h`.
 */
#include <lh/assert.h>

#include <lh/math/isqrt.h>
#include <lh/memory.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/radius.h>
#include <lh/ui/shadow/ramp.h>
#include <lh/util/return.h>

lh_void
lh_ui_shadow_shape_init(lh_ui_shadow_shape_t *self, const lh_ui_shadow_t *shadow,
                        const lh_ui_rect_t *rect, lh_ui_scalar_t radius)
{
    const lh_ui_point_t *origin;
    const lh_ui_size_t *size;
    lh_ui_color_t color;
    lh_ui_shadow_copy_color(shadow, lh_addr_of(color));
    lh_s32_t lo_x;
    lh_s32_t lo_y;
    lh_s32_t half_x;
    lh_s32_t half_y;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(shadow);
    lh_assert_runtime_ref(rect);
    origin = lh_ui_rect_get_origin(rect);
    size = lh_ui_rect_get_size(rect);
    lh_ui_shadow_shape_set_box(self, lh_ptr_deref(rect));
    lh_ui_shadow_shape_set_corner(self, lh_ui_radius_clamp(rect, radius));
    lh_ui_shadow_shape_set_box_x0(self, lh_ui_canvas_round_left(rect));
    lh_ui_shadow_shape_set_box_x1(self, lh_ui_canvas_round_right(rect));
    lh_ui_shadow_shape_set_box_y0(self, lh_ui_canvas_round_top(rect));
    lh_ui_shadow_shape_set_box_y1(self, lh_ui_canvas_round_bottom(rect));
    lo_x = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_point_get_x(origin) +
                                                          lh_ui_shadow_get_offset_x(shadow)));
    lo_y = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_point_get_y(origin) +
                                                          lh_ui_shadow_get_offset_y(shadow)));
    half_x = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_size_get_width(size))) / 2;
    half_y = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_size_get_height(size))) / 2;
    lh_ui_shadow_shape_set_r(self, lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_shadow_shape_get_corner(self))));
    lh_ui_shadow_shape_set_cx(self, lo_x + half_x);
    lh_ui_shadow_shape_set_cy(self, lo_y + half_y);
    lh_ui_shadow_shape_set_hx(self, half_x - lh_ui_shadow_shape_get_r(self));
    lh_ui_shadow_shape_set_hy(self, half_y - lh_ui_shadow_shape_get_r(self));
    lh_ui_shadow_shape_set_spread(self, lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_shadow_get_spread(shadow))));
    lh_ui_shadow_shape_set_peak(self, (lh_ui_shadow_shape_get_spread(self) <= 0 || lh_ui_rect_is_empty(rect)) ? 0 : lh_cast_static(lh_s32_t, lh_ui_color_get_a(lh_addr_of(color))));
    /* The fade straddles the edge: the peak a spread inside it, nothing a spread
       outside, and a straight line between -- `peak * (spread - d) / (2 * spread)`,
       rounded, which is a start and a step once both are in 16.16. */
    if (lh_ui_shadow_shape_get_peak(self) > 0)
    {
        lh_ui_shadow_shape_set_step(self, (lh_cast_static(lh_s64_t, lh_ui_shadow_shape_get_peak(self)) << 16) / (2 * lh_cast_static(lh_s64_t, lh_ui_shadow_shape_get_spread(self))));
        lh_ui_shadow_shape_set_start(self, (lh_cast_static(lh_s64_t, lh_ui_shadow_shape_get_peak(self)) << 15) + (1 << 15));
    }
    else
    {
        lh_ui_shadow_shape_set_step(self, 0);
        lh_ui_shadow_shape_set_start(self, 0);
    }
}

lh_s32_t
lh_ui_shadow_shape_row(const lh_ui_shadow_shape_t *self, lh_s32_t y)
{
    const lh_s32_t py = y * LH_UI_RADIUS_SUBPIXEL + LH_UI_RADIUS_SUBPIXEL / 2;

    lh_assert_runtime_ref(self);
    return (py > lh_ui_shadow_shape_get_cy(self) ? py - lh_ui_shadow_shape_get_cy(self) : lh_ui_shadow_shape_get_cy(self) - py) - lh_ui_shadow_shape_get_hy(self);
}

lh_byte_t
lh_ui_shadow_shape_fade(const lh_ui_shadow_shape_t *self, lh_s32_t distance)
{
    lh_s64_t fade;

    lh_return_if(distance >= lh_ui_shadow_shape_get_spread(self), 0);
    lh_return_if(distance <= lh_math_neg(lh_ui_shadow_shape_get_spread(self)), lh_cast_static(lh_byte_t, lh_ui_shadow_shape_get_peak(self)));
    fade = (lh_ui_shadow_shape_get_start(self) - lh_cast_static(lh_s64_t, distance) * lh_ui_shadow_shape_get_step(self)) >> 16;
    return (lh_byte_t)(fade <= 0 ? 0 : (fade > lh_ui_shadow_shape_get_peak(self) ? lh_ui_shadow_shape_get_peak(self) : fade));
}

lh_byte_t
lh_ui_shadow_shape_alpha(const lh_ui_shadow_shape_t *self, lh_s32_t row, lh_s32_t x, lh_s32_t y)
{
    const lh_s32_t px = x * LH_UI_RADIUS_SUBPIXEL + LH_UI_RADIUS_SUBPIXEL / 2;
    const lh_s32_t dx = (px > lh_ui_shadow_shape_get_cx(self) ? px - lh_ui_shadow_shape_get_cx(self) : lh_ui_shadow_shape_get_cx(self) - px) - lh_ui_shadow_shape_get_hx(self);
    const lh_s32_t ox = dx > 0 ? dx : 0;
    const lh_s32_t oy = row > 0 ? row : 0;
    const lh_s32_t most = dx > row ? dx : row;
    lh_s32_t length;
    lh_s32_t distance;

    lh_return_if(lh_ui_shadow_shape_get_peak(self) == 0, 0);
    /* A pixel the fill covers wholly is the fill's. It is decided from the pixel's
       centre, as the fill decides it (::lh_ui_radius_coverage), or the shadow's edge
       and the fill's would not meet on a rounded corner. */
    if (x >= lh_ui_shadow_shape_get_box_x0(self) && x < lh_ui_shadow_shape_get_box_x1(self) && y >= lh_ui_shadow_shape_get_box_y0(self) && y < lh_ui_shadow_shape_get_box_y1(self) &&
        lh_ui_radius_coverage(lh_ui_shadow_shape_get_box(self), lh_ui_shadow_shape_get_corner(self), x, y) == 255U)
    {
        return 0;
    }
    /* The distance to the shifted rounded box: the length of how far the pixel is
       outside it on each axis -- a square root only where it is outside both, at a
       corner -- or, inside it, how far in, which is negative. */
    if (ox == 0)
    {
        length = oy;
    }
    else if (oy == 0)
    {
        length = ox;
    }
    else
    {
        length = lh_cast_static(
            lh_s32_t,
            lh_math_isqrt_u64(lh_cast_static(lh_u64_t, ox) * lh_cast_static(lh_u64_t, ox) +
                              lh_cast_static(lh_u64_t, oy) * lh_cast_static(lh_u64_t, oy)));
    }
    distance = length + (most < 0 ? most : 0) - lh_ui_shadow_shape_get_r(self);
    return lh_ui_shadow_shape_fade(self, distance);
}

lh_void
lh_ui_shadow_shape_span(const lh_ui_shadow_shape_t *self, const struct lh_ui_shadow_ramp *ramp,
                        lh_s32_t y, lh_s32_t x0, lh_s32_t x1, lh_byte_ptr out)
{
    const lh_s32_t row = lh_ui_shadow_shape_row(self, y);
    const lh_s32_t oy = row > 0 ? row : 0;
    lh_s32_t full0 = x1;
    lh_s32_t full1 = x1;
    lh_s32_t px = x0 * LH_UI_RADIUS_SUBPIXEL + LH_UI_RADIUS_SUBPIXEL / 2;
    lh_s32_t level = 0;
    lh_s32_t x;

    lh_assert_runtime_ref(ramp);
    if (lh_ui_shadow_shape_get_peak(self) == 0)
    {
        for (x = x0; x < x1; ++x)
        {
            out[x - x0] = 0;
        }
        return;
    }
    /* The pixels the fill covers wholly, which are the fill's: in a row of the box,
       the span ::lh_ui_radius_full_span finds -- every pixel of it at coverage 255 and
       every other pixel short of it, so no pixel's coverage has to be asked. */
    if (y >= lh_ui_shadow_shape_get_box_y0(self) && y < lh_ui_shadow_shape_get_box_y1(self))
    {
        lh_ui_radius_full_span(lh_ui_shadow_shape_get_box(self), lh_ui_shadow_shape_get_corner(self), y, lh_addr_of(full0),
                               lh_addr_of(full1));
    }
    for (x = x0; x < x1;)
    {
        const lh_s32_t dx = (px > lh_ui_shadow_shape_get_cx(self) ? px - lh_ui_shadow_shape_get_cx(self) : lh_ui_shadow_shape_get_cx(self) - px) - lh_ui_shadow_shape_get_hx(self);
        const lh_s32_t ox = dx > 0 ? dx : 0;
        const lh_s32_t most = dx > row ? dx : row;
        lh_s32_t end;

        if (x >= full0 && x < full1)
        {
            end = full1 < x1 ? full1 : x1;
            lh_memory_set(out + (x - x0), lh_cast_static(lh_usize_t, (end - x)), 0U);
        }
        else if (ox == 0 && oy > 0)
        {
            /* Above or below the box and between its corners: every pixel is the
               same distance out, so the alpha is the row's, as far as the corner --
               where the pixel's centre passes `cx + hx` -- or the fill's span. */
            end = x + (lh_ui_shadow_shape_get_cx(self) + lh_ui_shadow_shape_get_hx(self) - px) / LH_UI_RADIUS_SUBPIXEL + 1;
            end = end < x1 ? end : x1;
            end = x < full0 && full0 < end ? full0 : end;
            lh_memory_set(out + (x - x0), lh_cast_static(lh_usize_t, (end - x)),
                          lh_ui_shadow_shape_fade(self, oy - lh_ui_shadow_shape_get_r(self)));
        }
        else
        {
            end = x + 1;
            if (ox > 0 && oy > 0)
            {
                /* Past a corner, outside both edges: the alpha is the root's, read off
                   the ramp from the squared distance, a step or two from the last
                   pixel's. */
                level = lh_ui_shadow_ramp_walk(
                    ramp,
                    lh_cast_static(lh_u64_t, ox) * lh_cast_static(lh_u64_t, ox) +
                        lh_cast_static(lh_u64_t, oy) * lh_cast_static(lh_u64_t, oy),
                    level);
                out[x - x0] = lh_cast_static(lh_byte_t, level);
            }
            else
            {
                out[x - x0] = lh_ui_shadow_shape_fade(self, (ox > 0 ? ox : oy) +
                                                                (most < 0 ? most : 0) - lh_ui_shadow_shape_get_r(self));
            }
        }
        px += (end - x) * LH_UI_RADIUS_SUBPIXEL;
        x = end;
    }
}

const lh_ui_rect_t *
lh_ui_shadow_shape_get_box(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_ui_rect_t *
lh_ui_shadow_shape_get_box_mut(lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_void
lh_ui_shadow_shape_set_box(lh_ui_shadow_shape_t *self, lh_ui_rect_t box)
{
    lh_assert_runtime_ref(self);
    self->box = box;
}

lh_ui_scalar_t
lh_ui_shadow_shape_get_corner(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->corner;
}

lh_void
lh_ui_shadow_shape_set_corner(lh_ui_shadow_shape_t *self, lh_ui_scalar_t corner)
{
    lh_assert_runtime_ref(self);
    self->corner = corner;
}

lh_s32_t
lh_ui_shadow_shape_get_box_x0(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->box_x0;
}

lh_void
lh_ui_shadow_shape_set_box_x0(lh_ui_shadow_shape_t *self, lh_s32_t box_x0)
{
    lh_assert_runtime_ref(self);
    self->box_x0 = box_x0;
}

lh_s32_t
lh_ui_shadow_shape_get_box_x1(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->box_x1;
}

lh_void
lh_ui_shadow_shape_set_box_x1(lh_ui_shadow_shape_t *self, lh_s32_t box_x1)
{
    lh_assert_runtime_ref(self);
    self->box_x1 = box_x1;
}

lh_s32_t
lh_ui_shadow_shape_get_box_y0(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->box_y0;
}

lh_void
lh_ui_shadow_shape_set_box_y0(lh_ui_shadow_shape_t *self, lh_s32_t box_y0)
{
    lh_assert_runtime_ref(self);
    self->box_y0 = box_y0;
}

lh_s32_t
lh_ui_shadow_shape_get_box_y1(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->box_y1;
}

lh_void
lh_ui_shadow_shape_set_box_y1(lh_ui_shadow_shape_t *self, lh_s32_t box_y1)
{
    lh_assert_runtime_ref(self);
    self->box_y1 = box_y1;
}

lh_s32_t
lh_ui_shadow_shape_get_cx(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cx;
}

lh_void
lh_ui_shadow_shape_set_cx(lh_ui_shadow_shape_t *self, lh_s32_t cx)
{
    lh_assert_runtime_ref(self);
    self->cx = cx;
}

lh_s32_t
lh_ui_shadow_shape_get_cy(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cy;
}

lh_void
lh_ui_shadow_shape_set_cy(lh_ui_shadow_shape_t *self, lh_s32_t cy)
{
    lh_assert_runtime_ref(self);
    self->cy = cy;
}

lh_s32_t
lh_ui_shadow_shape_get_hx(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hx;
}

lh_void
lh_ui_shadow_shape_set_hx(lh_ui_shadow_shape_t *self, lh_s32_t hx)
{
    lh_assert_runtime_ref(self);
    self->hx = hx;
}

lh_s32_t
lh_ui_shadow_shape_get_hy(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hy;
}

lh_void
lh_ui_shadow_shape_set_hy(lh_ui_shadow_shape_t *self, lh_s32_t hy)
{
    lh_assert_runtime_ref(self);
    self->hy = hy;
}

lh_s32_t
lh_ui_shadow_shape_get_r(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_void
lh_ui_shadow_shape_set_r(lh_ui_shadow_shape_t *self, lh_s32_t r)
{
    lh_assert_runtime_ref(self);
    self->r = r;
}

lh_s32_t
lh_ui_shadow_shape_get_spread(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->spread;
}

lh_void
lh_ui_shadow_shape_set_spread(lh_ui_shadow_shape_t *self, lh_s32_t spread)
{
    lh_assert_runtime_ref(self);
    self->spread = spread;
}

lh_s32_t
lh_ui_shadow_shape_get_peak(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->peak;
}

lh_void
lh_ui_shadow_shape_set_peak(lh_ui_shadow_shape_t *self, lh_s32_t peak)
{
    lh_assert_runtime_ref(self);
    self->peak = peak;
}

lh_s64_t
lh_ui_shadow_shape_get_start(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_ui_shadow_shape_set_start(lh_ui_shadow_shape_t *self, lh_s64_t start)
{
    lh_assert_runtime_ref(self);
    self->start = start;
}

lh_s64_t
lh_ui_shadow_shape_get_step(const lh_ui_shadow_shape_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

lh_void
lh_ui_shadow_shape_set_step(lh_ui_shadow_shape_t *self, lh_s64_t step)
{
    lh_assert_runtime_ref(self);
    self->step = step;
}
