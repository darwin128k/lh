/**
 * @file shape.c
 * @brief Implementation of `lh/ui/shadow/shape.h`.
 */

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
    lh_ui_shadow_get_color(shadow, lh_addr_of(color));
    lh_s32_t lo_x;
    lh_s32_t lo_y;
    lh_s32_t half_x;
    lh_s32_t half_y;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(shadow);
    lh_assert_runtime_ref(rect);
    origin = lh_ui_rect_get_origin(rect);
    size = lh_ui_rect_get_size(rect);
    self->box = lh_ptr_deref(rect);
    self->corner = lh_ui_radius_clamp(rect, radius);
    self->box_x0 = lh_ui_canvas_round_left(rect);
    self->box_x1 = lh_ui_canvas_round_right(rect);
    self->box_y0 = lh_ui_canvas_round_top(rect);
    self->box_y1 = lh_ui_canvas_round_bottom(rect);
    lo_x = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_point_get_x(origin) +
                                                          lh_ui_shadow_get_offset_x(shadow)));
    lo_y = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_point_get_y(origin) +
                                                          lh_ui_shadow_get_offset_y(shadow)));
    half_x = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_size_get_width(size))) / 2;
    half_y = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_size_get_height(size))) / 2;
    self->r = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(self->corner));
    self->cx = lo_x + half_x;
    self->cy = lo_y + half_y;
    self->hx = half_x - self->r;
    self->hy = half_y - self->r;
    self->spread = lh_cast_static(lh_s32_t, lh_ui_radius_to_fixed(lh_ui_shadow_get_spread(shadow)));
    self->peak = (self->spread <= 0 || lh_ui_rect_is_empty(rect))
                     ? 0
                     : lh_cast_static(lh_s32_t, lh_ui_color_get_a(lh_addr_of(color)));
    /* The fade straddles the edge: the peak a spread inside it, nothing a spread
       outside, and a straight line between -- `peak * (spread - d) / (2 * spread)`,
       rounded, which is a start and a step once both are in 16.16. */
    if (self->peak > 0)
    {
        self->step = (lh_cast_static(lh_s64_t, self->peak) << 16) /
                     (2 * lh_cast_static(lh_s64_t, self->spread));
        self->start = (lh_cast_static(lh_s64_t, self->peak) << 15) + (1 << 15);
    }
    else
    {
        self->step = 0;
        self->start = 0;
    }
}

lh_s32_t
lh_ui_shadow_shape_row(const lh_ui_shadow_shape_t *self, lh_s32_t y)
{
    const lh_s32_t py = y * LH_UI_RADIUS_SUBPIXEL + LH_UI_RADIUS_SUBPIXEL / 2;

    lh_assert_runtime_ref(self);
    return (py > self->cy ? py - self->cy : self->cy - py) - self->hy;
}

lh_byte_t
lh_ui_shadow_shape_fade(const lh_ui_shadow_shape_t *self, lh_s32_t distance)
{
    lh_s64_t fade;

    lh_return_if(distance >= self->spread, 0);
    lh_return_if(distance <= lh_math_neg(self->spread), lh_cast_static(lh_byte_t, self->peak));
    fade = (self->start - lh_cast_static(lh_s64_t, distance) * self->step) >> 16;
    return (lh_byte_t)(fade <= 0 ? 0 : (fade > self->peak ? self->peak : fade));
}

lh_byte_t
lh_ui_shadow_shape_alpha(const lh_ui_shadow_shape_t *self, lh_s32_t row, lh_s32_t x, lh_s32_t y)
{
    const lh_s32_t px = x * LH_UI_RADIUS_SUBPIXEL + LH_UI_RADIUS_SUBPIXEL / 2;
    const lh_s32_t dx = (px > self->cx ? px - self->cx : self->cx - px) - self->hx;
    const lh_s32_t ox = dx > 0 ? dx : 0;
    const lh_s32_t oy = row > 0 ? row : 0;
    const lh_s32_t most = dx > row ? dx : row;
    lh_s32_t length;
    lh_s32_t distance;

    lh_return_if(self->peak == 0, 0);
    /* A pixel the fill covers wholly is the fill's. It is decided from the pixel's
       centre, as the fill decides it (::lh_ui_radius_coverage), or the shadow's edge
       and the fill's would not meet on a rounded corner. */
    if (x >= self->box_x0 && x < self->box_x1 && y >= self->box_y0 && y < self->box_y1 &&
        lh_ui_radius_coverage(lh_addr_of(self->box), self->corner, x, y) == 255U)
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
    distance = length + (most < 0 ? most : 0) - self->r;
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
    if (self->peak == 0)
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
    if (y >= self->box_y0 && y < self->box_y1)
    {
        lh_ui_radius_full_span(lh_addr_of(self->box), self->corner, y, lh_addr_of(full0),
                               lh_addr_of(full1));
    }
    for (x = x0; x < x1;)
    {
        const lh_s32_t dx = (px > self->cx ? px - self->cx : self->cx - px) - self->hx;
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
            end = x + (self->cx + self->hx - px) / LH_UI_RADIUS_SUBPIXEL + 1;
            end = end < x1 ? end : x1;
            end = x < full0 && full0 < end ? full0 : end;
            lh_memory_set(out + (x - x0), lh_cast_static(lh_usize_t, (end - x)),
                          lh_ui_shadow_shape_fade(self, oy - self->r));
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
                                                                (most < 0 ? most : 0) - self->r);
            }
        }
        px += (end - x) * LH_UI_RADIUS_SUBPIXEL;
        x = end;
    }
}
