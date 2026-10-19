/**
 * @file corner.c
 * @brief Implementation of `lh/os/window/corner.h`.
 */

#include <lh/math/ops.h>
#include <lh/os/window/corner.h>
#include <lh/util/return.h>
#include <lh/util/type.h>

lh_s32_t
lh_os_window_corner_inset(lh_s32_t radius, lh_s32_t row)
{
    const lh_s64_t radius64 = lh_type_cast(lh_s64_t, radius);
    const lh_s64_t dy =
        lh_math_sub(lh_math_sub(lh_math_mul(2, radius64), lh_math_mul(2, lh_type_cast(lh_s64_t, row))),
                    1);
    const lh_s64_t r2 = lh_math_mul(4, lh_math_mul(radius64, radius64));
    lh_s32_t inset;

    for (inset = 0; lh_math_lt(inset, radius); ++inset)
    {
        const lh_s64_t dx = lh_math_sub(
            lh_math_sub(lh_math_mul(2, radius64), lh_math_mul(2, lh_type_cast(lh_s64_t, inset))), 1);

        lh_return_if(lh_math_le(lh_math_add(lh_math_mul(dx, dx), lh_math_mul(dy, dy)), r2), inset);
    }
    return radius;
}

lh_s32_t
lh_os_window_corner_clamp(lh_s32_t corner, lh_s32_t width, lh_s32_t height, lh_bool_t maximized)
{
    lh_s32_t radius = maximized ? 0 : corner;

    radius = lh_math_min(radius, lh_math_div(width, 2));
    radius = lh_math_min(radius, lh_math_div(height, 2));
    return lh_math_min(radius, LH_OS_WINDOW_CORNER_MAX);
}
