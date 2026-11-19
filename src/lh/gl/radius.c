/**
 * @file radius.c
 * @brief Implementation of `lh/gl/radius.h`.
 *
 * Coverage works in fixed point (::LH_GL_RADIUS_SUBPIXEL units per pixel),
 * the same code for the integer and the float scalar, so it needs no FPU.
 */

#include <lh/core/math.h>
#include <lh/core/memory.h>
#include <lh/gl/radius.h>
#include <lh/core/util/return.h>

lh_s64_t
lh_gl_radius_to_fixed(lh_gl_scalar_t v)
{
    return lh_cast_static(lh_s64_t, v * LH_GL_RADIUS_SUBPIXEL);
}

lh_s64_t
lh_gl_radius_pixel_center(lh_s32_t i)
{
    return lh_pixel_round_pixel_center(i);
}

lh_gl_scalar_t
lh_gl_radius_get_max(const lh_gl_rect_t *rect)
{
    return lh_math_max(lh_gl_size_min_side(lh_gl_rect_get_size(rect)) / lh_gl_scalar(2),
                       lh_gl_scalar(0));
}

lh_byte_t
lh_gl_radius_cover_from_distance(lh_s64_t r, lh_s64_t d)
{
    return lh_pixel_round_cover_from_distance(r, d);
}

lh_byte_t
lh_gl_radius_scale(lh_byte_t a, lh_byte_t b)
{
    return lh_cast_static(lh_byte_t, (lh_cast_static(lh_u32_t, a) * b + 127U) / 255U);
}

lh_byte_t
lh_gl_radius_cover_from_square(lh_s64_t r, lh_s64_t d2)
{
    return lh_pixel_round_cover_from_square(r, d2);
}

lh_gl_scalar_t
lh_gl_radius_clamp(const lh_gl_rect_t *rect, lh_gl_scalar_t radius)
{
    lh_assert_runtime_ref(rect);
    return lh_math_clamp(radius, lh_gl_scalar(0), lh_gl_radius_get_max(rect));
}

lh_s64_t
lh_gl_radius_axis_distance(lh_s64_t p, lh_gl_scalar_t start, lh_gl_scalar_t length, lh_s64_t r)
{
    return lh_pixel_round_axis_distance(
        p, lh_gl_radius_to_fixed(start),
        lh_gl_radius_to_fixed(start) + lh_gl_radius_to_fixed(length), r);
}

lh_u64_t
lh_gl_radius_scale_8(lh_byte_t a, lh_u64_t b)
{
    lh_u64_t out = 0U;

    for (lh_u32_t i = 0U; i < 8U; ++i)
    {
        out |= lh_cast_static(lh_u64_t,
                              lh_gl_radius_scale(a, lh_cast_static(lh_byte_t, b >> (8U * i))))
               << (8U * i);
    }
    return out;
}

lh_void
lh_gl_radius_get_box(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_pixel_round_box_t *out)
{
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    const lh_s64_t lo_x = lh_gl_radius_to_fixed(lh_gl_point_get_x(origin));
    const lh_s64_t lo_y = lh_gl_radius_to_fixed(lh_gl_point_get_y(origin));

    lh_pixel_round_box_init(
        out, lo_x, lo_y, lo_x + lh_gl_radius_to_fixed(lh_gl_size_get_width(size)),
        lo_y + lh_gl_radius_to_fixed(lh_gl_size_get_height(size)), lh_gl_radius_to_fixed(radius));
}

lh_void
lh_gl_radius_run_init(struct lh_gl_radius_run *self, const lh_gl_rect_t *rect,
                      lh_gl_scalar_t radius)
{
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);

    lh_assert_runtime_ref(self);
    self->radius = radius;
    self->r = lh_gl_radius_to_fixed(radius);
    self->lo = lh_gl_radius_to_fixed(lh_gl_point_get_x(origin));
    self->hi = self->lo + lh_gl_radius_to_fixed(lh_gl_size_get_width(size));
    self->near = self->lo + self->r;
    self->far = self->hi - self->r;
    self->top = lh_gl_point_get_y(origin);
    self->height = lh_gl_size_get_height(size);
}

lh_byte_t
lh_gl_radius_coverage(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_s32_t x, lh_s32_t y)
{
    lh_pixel_round_box_t box;

    lh_gl_radius_get_box(rect, radius, lh_addr_of(box));
    return lh_pixel_round_box_get_coverage(lh_addr_of(box), x, y);
}

lh_void
lh_gl_radius_full_span(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_s32_t y,
                       lh_s32_t *out_x0, lh_s32_t *out_x1)
{
    lh_pixel_round_box_t box;

    lh_gl_radius_get_box(rect, radius, lh_addr_of(box));
    lh_pixel_round_box_get_full_span(lh_addr_of(box), y, out_x0, out_x1);
}

lh_bool_t
lh_gl_radius_contains(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_gl_point_t point)
{
    lh_return_if(!lh_gl_rect_contains_point(rect, point), lh_bool_false);
    lh_return_if(radius <= lh_gl_scalar(0), lh_bool_true);
    return lh_gl_radius_coverage(rect, lh_gl_radius_clamp(rect, radius),
                                 lh_gl_scalar_floor_s32(lh_gl_point_get_x(lh_addr_of(point))),
                                 lh_gl_scalar_floor_s32(lh_gl_point_get_y(lh_addr_of(point)))) >=
                   LH_GL_RADIUS_HIT_COVERAGE
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_gl_radius_run_row(const struct lh_gl_radius_run *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                     lh_byte_ptr out)
{
    const lh_s64_t dy =
        lh_gl_radius_axis_distance(lh_gl_radius_pixel_center(y), self->top, self->height, self->r);
    const lh_s64_t dy2 = dy * dy;
    /* dy is at most r, and the outer edge of the ramp is r + half a subpixel,
       so lim_sq never goes negative and the arc always has a non-empty core. */
    const lh_s64_t lim_sq =
        (self->r + LH_GL_RADIUS_SUBPIXEL / 2) * (self->r + LH_GL_RADIUS_SUBPIXEL / 2) - dy2;

    lh_assert_runtime_ref(out);
    lh_return_if(x1 <= x0);

    /* A row outside the rect is all 0 and nothing else is written, so this is
       the one branch that still has to clear. */
    if (dy < 0)
    {
        lh_memory_set(out, lh_cast_static(lh_usize_t, x1 - x0), 0);
        return;
    }
    /* A row away from every corner: whole pixels of the rect are 255, the
       columns outside it 0. dy == 0 alone does not decide that — the column
       still has to be inside the rect, or lh_gl_radius_coverage answers 0. */
    if (dy == 0)
    {
        for (lh_s32_t i = 0; i < x1 - x0; ++i)
        {
            lh_s64_t p = lh_gl_radius_pixel_center(x0 + i);

            out[i] = p < self->lo || p >= self->hi ? 0 : 255;
        }
        return;
    }

    lh_s64_t p = lh_gl_radius_pixel_center(x0);

    /* p steps by exactly one subpixel per column, so the fixed-point edges, the
       radius and the vertical distance are all hoisted. Every pixel is written,
       the ones outside the rect with a plain zero, so the loop below does not
       need the row cleared under it first: a corner band hands this two or three
       bytes at a time, and a cross-module memset of two bytes costs more than
       the whole loop. Past the arc's outer edge the coverage is 0, which is what
       lh_gl_radius_cover_from_square answers to d2 >= outer * outer — decided
       here, without a square root. */
    for (lh_s32_t i = 0; i < x1 - x0; ++i, p += LH_GL_RADIUS_SUBPIXEL)
    {
        lh_s64_t dx;
        lh_s64_t d2;

        if (p < self->lo || p >= self->hi)
        {
            out[i] = 0;
            continue;
        }
        dx = lh_math_max(lh_math_max(self->near - p, p - self->far), 0);
        if (dx == 0)
        {
            out[i] = 255;
            continue;
        }
        d2 = dx * dx;
        out[i] = d2 >= lim_sq ? 0 : lh_gl_radius_cover_from_square(self->r, d2 + dy2);
    }
}

lh_void
lh_gl_radius_coverage_run(const lh_gl_rect_t *rect, lh_gl_scalar_t radius, lh_s32_t x0, lh_s32_t x1,
                          lh_s32_t y, lh_byte_ptr out)
{
    struct lh_gl_radius_run run;

    lh_gl_radius_run_init(lh_addr_of(run), rect, radius);
    lh_gl_radius_run_row(lh_addr_of(run), x0, x1, y, out);
}
