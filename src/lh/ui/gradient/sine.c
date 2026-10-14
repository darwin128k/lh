/**
 * @file sine.c
 * @brief Implementation of `lh/ui/gradient/sine.h`.
 */

#include <lh/ui/gradient/sine.h>

/** sin(d) for d in 0 .. 90 degrees, times 4096. Whole degrees are all CSS angles a sheet
   writes in practice, and a table is the same on a build with no FPU. */
static const lh_s16_t lh_ui_gradient_sine_table[91] = {
    0, 71, 143, 214, 286, 357, 428, 499, 570, 641, 711, 782,
    852, 921, 991, 1060, 1129, 1198, 1266, 1334, 1401, 1468, 1534, 1600,
    1666, 1731, 1796, 1860, 1923, 1986, 2048, 2110, 2171, 2231, 2290, 2349,
    2408, 2465, 2522, 2578, 2633, 2687, 2741, 2793, 2845, 2896, 2946, 2996,
    3044, 3091, 3138, 3183, 3228, 3271, 3314, 3355, 3396, 3435, 3474, 3511,
    3547, 3582, 3617, 3650, 3681, 3712, 3742, 3770, 3798, 3824, 3849, 3873,
    3896, 3917, 3937, 3956, 3974, 3991, 4006, 4021, 4034, 4046, 4056, 4065,
    4074, 4080, 4086, 4090, 4094, 4095, 4096,
};

lh_s32_t
lh_ui_gradient_sine(lh_s32_t degrees)
{
    degrees %= 360;
    if (degrees < 0)
    {
        degrees += 360;
    }
    if (degrees <= 90)
    {
        return lh_ui_gradient_sine_table[degrees];
    }
    if (degrees <= 180)
    {
        return lh_ui_gradient_sine_table[180 - degrees];
    }
    if (degrees <= 270)
    {
        return -lh_ui_gradient_sine_table[degrees - 180];
    }
    return -lh_ui_gradient_sine_table[360 - degrees];
}
