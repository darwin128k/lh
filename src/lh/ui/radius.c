/**
 * @file radius.c
 * @brief Implementation of `lh/ui/radius.h`.
 *
 * Coverage works in fixed point (::LH_UI_RADIUS_SUBPIXEL units per pixel),
 * the same code for the integer and the float scalar, so it needs no FPU.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/math/isqrt.h>
#include <lh/memory.h>
#include <lh/ui/radius.h>
#include <lh/util/return.h>

lh_s64_t
lh_ui_radius_to_fixed(lh_ui_scalar_t v)
{
    return lh_cast_static(lh_s64_t, v * LH_UI_RADIUS_SUBPIXEL);
}

lh_s64_t
lh_ui_radius_pixel_center(lh_s32_t i)
{
    return lh_cast_static(lh_s64_t, i) * LH_UI_RADIUS_SUBPIXEL + LH_UI_RADIUS_SUBPIXEL / 2;
}

lh_ui_scalar_t
lh_ui_radius_get_max(const lh_ui_rect_t *rect)
{
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_ui_scalar_t short_side = lh_math_min(lh_ui_size_get_width(size), lh_ui_size_get_height(size));
    return lh_math_max(short_side / lh_ui_scalar(2), lh_ui_scalar(0));
}

lh_ui_scalar_t
lh_ui_radius_clamp(const lh_ui_rect_t *rect, lh_ui_scalar_t radius)
{
    lh_assert_runtime_ref(rect);
    return lh_math_clamp(radius, lh_ui_scalar(0), lh_ui_radius_get_max(rect));
}

lh_s64_t
lh_ui_radius_axis_distance(lh_s64_t p, lh_ui_scalar_t start, lh_ui_scalar_t length, lh_s64_t r)
{
    const lh_s64_t lo = lh_ui_radius_to_fixed(start);
    const lh_s64_t hi = lo + lh_ui_radius_to_fixed(length);
    lh_return_if(p < lo || p >= hi, -1);
    return lh_math_max(lh_math_max(lo + r - p, p - (hi - r)), 0);
}

lh_byte_t
lh_ui_radius_cover_from_distance(lh_s64_t r, lh_s64_t d)
{
    const lh_s64_t cover = lh_math_clamp(r + LH_UI_RADIUS_SUBPIXEL / 2 - d, 0, LH_UI_RADIUS_SUBPIXEL);
    return lh_cast_static(lh_byte_t, (cover * 255 + LH_UI_RADIUS_SUBPIXEL / 2) / LH_UI_RADIUS_SUBPIXEL);
}

lh_bool_t
lh_ui_radius_contains(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_ui_point_t point)
{
    lh_return_if(!lh_ui_rect_contains_point(rect, point), lh_bool_false);
    lh_return_if(radius <= lh_ui_scalar(0), lh_bool_true);
    return lh_ui_radius_coverage(rect, lh_ui_radius_clamp(rect, radius),
                                 lh_ui_scalar_floor_s32(lh_ui_point_get_x(lh_addr_of(point))),
                                 lh_ui_scalar_floor_s32(lh_ui_point_get_y(lh_addr_of(point)))) >=
                   LH_UI_RADIUS_HIT_COVERAGE
               ? lh_bool_true
               : lh_bool_false;
}

lh_byte_t
lh_ui_radius_scale(lh_byte_t a, lh_byte_t b)
{
    return lh_cast_static(lh_byte_t, (lh_cast_static(lh_u32_t, a) * b + 127U) / 255U);
}

lh_byte_t
lh_ui_radius_cover_from_square(lh_s64_t r, lh_s64_t d2)
{
    const lh_s64_t inner = r - LH_UI_RADIUS_SUBPIXEL / 2;
    const lh_s64_t outer = r + LH_UI_RADIUS_SUBPIXEL / 2;

    /* Whole pixels inside or outside the ramp need no square root. */
    lh_return_if(inner >= 0 && d2 <= inner * inner, 255);
    lh_return_if(d2 >= outer * outer, 0);
    return lh_ui_radius_cover_from_distance(r, lh_cast_static(lh_s64_t, lh_math_isqrt_u64(lh_cast_static(lh_u64_t, d2))));
}

lh_byte_t
lh_ui_radius_coverage(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x, lh_s32_t y)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_s64_t r = lh_ui_radius_to_fixed(radius);
    const lh_s64_t dx = lh_ui_radius_axis_distance(lh_ui_radius_pixel_center(x), lh_ui_point_get_x(origin),
                                                   lh_ui_size_get_width(size), r);
    const lh_s64_t dy = lh_ui_radius_axis_distance(lh_ui_radius_pixel_center(y), lh_ui_point_get_y(origin),
                                                   lh_ui_size_get_height(size), r);
    lh_return_if(dx < 0 || dy < 0, 0);
    lh_return_if(dx == 0 || dy == 0, 255);
    return lh_ui_radius_cover_from_square(r, dx * dx + dy * dy);
}

lh_void
lh_ui_radius_coverage_run(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                          lh_byte_t *out)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_s64_t r = lh_ui_radius_to_fixed(radius);
    const lh_s64_t lo = lh_ui_radius_to_fixed(lh_ui_point_get_x(origin));
    const lh_s64_t hi = lo + lh_ui_radius_to_fixed(lh_ui_size_get_width(size));
    const lh_s64_t dy = lh_ui_radius_axis_distance(lh_ui_radius_pixel_center(y), lh_ui_point_get_y(origin),
                                                   lh_ui_size_get_height(size), r);
    const lh_s64_t near = lo + r; /* right edge of the left corner zone */
    const lh_s64_t far = hi - r;  /* left edge of the right corner zone */
    const lh_s64_t dy2 = dy * dy;
    /* dy is at most r, and the outer edge of the ramp is r + half a subpixel,
       so lim_sq never goes negative and the arc always has a non-empty core. */
    const lh_s64_t lim_sq = (r + LH_UI_RADIUS_SUBPIXEL / 2) * (r + LH_UI_RADIUS_SUBPIXEL / 2) - dy2;
    lh_s64_t p;
    lh_s32_t i;

    lh_assert_runtime_ref(out);
    lh_return_if(x1 <= x0);

    lh_memory_set(out, lh_cast_static(lh_usize_t, x1 - x0), 0);

    /* A row outside the rect is all 0, already written. */
    if (dy < 0)
    {
        return;
    }
    /* A row away from every corner: whole pixels of the rect are 255, the
       columns outside it 0. dy == 0 alone does not decide that — the column
       still has to be inside the rect, or lh_ui_radius_coverage answers 0. */
    if (dy == 0)
    {
        for (i = 0; i < x1 - x0; ++i)
        {
            p = lh_ui_radius_pixel_center(x0 + i);
            out[i] = p < lo || p >= hi ? 0 : 255;
        }
        return;
    }

    /* p steps by exactly one subpixel per column, so the fixed-point edges, the
       radius and the vertical distance are all hoisted. Past the arc's outer
       edge the coverage is 0, which is what lh_ui_radius_cover_from_square
       answers to d2 >= outer * outer — decided here, without a square root. */
    for (i = 0, p = lh_ui_radius_pixel_center(x0); i < x1 - x0; ++i, p += LH_UI_RADIUS_SUBPIXEL)
    {
        lh_s64_t dx;
        lh_s64_t d2;

        if (p < lo || p >= hi)
        {
            continue;
        }
        dx = lh_math_max(lh_math_max(near - p, p - far), 0);
        if (dx == 0)
        {
            out[i] = 255;
            continue;
        }
        d2 = dx * dx;
        out[i] = d2 >= lim_sq ? 0 : lh_ui_radius_cover_from_square(r, d2 + dy2);
    }
}
