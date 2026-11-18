/**
 * @file round.c
 * @brief Implementation of `lh/core/pixel/round.h`.
 */

#include <lh/core/pixel/round.h>

#include <lh/core/math.h>
#include <lh/core/util/return.h>

lh_s64_t
lh_pixel_round_pixel_center(lh_s32_t i)
{
    return lh_cast_static(lh_s64_t, i) * LH_PIXEL_ROUND_SUBPIXEL + LH_PIXEL_ROUND_SUBPIXEL / 2;
}

lh_s64_t
lh_pixel_round_axis_distance(lh_s64_t p, lh_s64_t lo, lh_s64_t hi, lh_s64_t r)
{
    lh_return_if(p < lo || p >= hi, -1);
    return lh_math_max(lh_math_max(lo + r - p, p - (hi - r)), 0);
}

lh_byte_t
lh_pixel_round_cover_from_distance(lh_s64_t r, lh_s64_t d)
{
    const lh_s64_t cover =
        lh_math_clamp(r + LH_PIXEL_ROUND_SUBPIXEL / 2 - d, 0, LH_PIXEL_ROUND_SUBPIXEL);

    return lh_cast_static(lh_byte_t, lh_math_div_round(cover * 255, LH_PIXEL_ROUND_SUBPIXEL));
}

lh_byte_t
lh_pixel_round_cover_from_square(lh_s64_t r, lh_s64_t d2)
{
    const lh_s64_t inner = r - LH_PIXEL_ROUND_SUBPIXEL / 2;
    const lh_s64_t outer = r + LH_PIXEL_ROUND_SUBPIXEL / 2;

    /* Whole pixels inside or outside the ramp need no square root. */
    lh_return_if(inner >= 0 && d2 <= inner * inner, 255);
    lh_return_if(d2 >= outer * outer, 0);
    return lh_pixel_round_cover_from_distance(
        r, lh_cast_static(lh_s64_t, lh_math_isqrt_u64(lh_cast_static(lh_u64_t, d2))));
}

lh_byte_t
lh_pixel_round_coverage(lh_s64_t lo_x, lh_s64_t lo_y, lh_s64_t hi_x, lh_s64_t hi_y, lh_s64_t r,
                        lh_s32_t x, lh_s32_t y)
{
    const lh_s64_t dx = lh_pixel_round_axis_distance(lh_pixel_round_pixel_center(x), lo_x, hi_x, r);
    const lh_s64_t dy = lh_pixel_round_axis_distance(lh_pixel_round_pixel_center(y), lo_y, hi_y, r);

    lh_return_if(dx < 0 || dy < 0, 0);
    lh_return_if(dx == 0 || dy == 0, 255);
    return lh_pixel_round_cover_from_square(r, dx * dx + dy * dy);
}
