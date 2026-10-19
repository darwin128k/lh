/**
 * @file corner.c
 * @brief Implementation of `lh/os/window/corner.h`.
 */

#include <lh/os/window/corner.h>
#include <lh/util/return.h>

lh_s32_t
lh_os_window_corner_inset(lh_s32_t radius, lh_s32_t row)
{
    const long dy = 2L * radius - 2L * row - 1L;
    lh_s32_t inset;

    for (inset = 0; inset < radius; ++inset)
    {
        const long dx = 2L * radius - 2L * inset - 1L;

        lh_return_if(dx * dx + dy * dy <= 4L * radius * radius, inset);
    }
    return radius;
}

lh_s32_t
lh_os_window_corner_clamp(lh_s32_t corner, lh_s32_t width, lh_s32_t height, lh_bool_t maximized)
{
    lh_s32_t radius = maximized ? 0 : corner;

    radius = radius < width / 2 ? radius : width / 2;
    radius = radius < height / 2 ? radius : height / 2;
    return radius < LH_OS_WINDOW_CORNER_MAX ? radius : LH_OS_WINDOW_CORNER_MAX;
}
