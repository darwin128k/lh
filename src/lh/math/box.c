/**
 * @file box.c
 * @brief Implementation of `lh/math/box.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/float/sqrt.h>
#include <lh/math.h>
#include <lh/math/box.h>

lh_float_t
lh_math_box_distance(lh_math_vec2_t point, lh_math_vec2_t size, lh_float_t radius)
{
    /* Clamped at zero, the way lh_math_rect_is_empty reads a side: a box with
       no room on an axis has no interior on it, whether it was given no size
       or a negative one, and the arithmetic below would otherwise fold a
       negative half-span back on itself and answer about a box that is not
       there. */
    const lh_float_t half_x = lh_math_max(lh_math_vec2_get_x(lh_addr_of(size)), 0.0f) * 0.5f;
    const lh_float_t half_y = lh_math_max(lh_math_vec2_get_y(lh_addr_of(size)), 0.0f) * 0.5f;
    const lh_float_t corner = lh_math_min(radius, lh_math_min(half_x, half_y));
    /* The centre, so the box is symmetric about it and the answer does not
       depend on which side of the origin it is measured from. */
    const lh_float_t dx = lh_math_vec2_get_x(lh_addr_of(point)) - half_x;
    const lh_float_t dy = lh_math_vec2_get_y(lh_addr_of(point)) - half_y;
    /* How far outside the straight part of the box the point is, per axis.
       Negative means the point is within that span. */
    const lh_float_t out_x = lh_math_abs(dx) - (half_x - corner);
    const lh_float_t out_y = lh_math_abs(dy) - (half_y - corner);
    /* Outside: the distance to the nearest rim, so both axes matter. Inside:
       the larger of the two, which is the nearer edge, with the corners
       rounded off by the same corner. */
    return lh_float_sqrt(lh_math_max(out_x, 0.0f) * lh_math_max(out_x, 0.0f) +
                         lh_math_max(out_y, 0.0f) * lh_math_max(out_y, 0.0f)) +
           lh_math_min(lh_math_max(out_x, out_y), 0.0f) - corner;
}
