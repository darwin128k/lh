/**
 * @file ramp.c
 * @brief Implementation of `lh/core/pixel/shadow/ramp.h`.
 */

#include <lh/core/pixel/shadow/ramp.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>

const lh_u64_t *
lh_pixel_shadow_ramp_get_square(const lh_pixel_shadow_ramp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->square;
}

lh_u64_t *
lh_pixel_shadow_ramp_get_square_mut(lh_pixel_shadow_ramp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->square;
}

lh_s32_t
lh_pixel_shadow_ramp_get_count(const lh_pixel_shadow_ramp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_pixel_shadow_ramp_set_count(lh_pixel_shadow_ramp_t *self, lh_s32_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_void
lh_pixel_shadow_ramp_init(lh_pixel_shadow_ramp_t *self, const lh_pixel_shadow_t *shadow)
{
    const lh_s64_t spread = lh_pixel_shadow_get_spread(shadow);
    const lh_s64_t step = lh_pixel_shadow_get_step(shadow);
    const lh_s64_t r = lh_pixel_shadow_get_r(shadow);
    const lh_s64_t one = 1 << 16;
    /* The least distance `d` past the corner's edge whose alpha is `level` or less:
       from a spread out, where nothing is left, back to just past a spread in, where
       the peak gives way -- and between, where the fade `(start - d * step) >> 16`
       first drops to `level`, which is `d > (start - ((level + 1) << 16)) / step`
       rounded down. The numerator falls by `1 << 16` a level, so the quotient and its
       remainder are carried down: two divisions for the ramp, not one a level. */
    lh_s64_t limit = lh_pixel_shadow_get_start(shadow) - one;
    lh_s64_t quotient = step > 0 ? lh_math_floor_div(limit, step) : 0;
    lh_s64_t rest = step > 0 ? lh_math_floor_mod(limit, step) : 0;
    const lh_s64_t fall = step > 0 ? lh_math_floor_div(one, step) : 0;
    const lh_s64_t fall_rest = step > 0 ? lh_math_floor_mod(one, step) : 0;

    lh_pixel_shadow_ramp_set_count(self, lh_pixel_shadow_get_peak(shadow));
    for (lh_s32_t level = 0; level < lh_pixel_shadow_ramp_get_count(self); ++level)
    {
        const lh_s64_t least =
            lh_math_clamp(step > 0 ? quotient + 1 : (limit < 0 ? lh_math_neg(spread) : spread),
                          1 - spread, spread);
        /* A pixel past the corner's centre by `root` is past its edge by `root - r`,
           and `root` is the whole square root of the squared distance, so
           `root >= least + r` is the squared distance reaching `(least + r)^2`. */
        const lh_s64_t reach = least + r;

        lh_pixel_shadow_ramp_get_square_mut(self)[level] =
            reach > 0 ? lh_cast_static(lh_u64_t, reach * reach) : 0U;
        limit -= one;
        quotient -= fall;
        rest -= fall_rest;
        if (rest < 0)
        {
            rest += step;
            --quotient;
        }
    }
}

lh_s32_t
lh_pixel_shadow_ramp_get_level(const lh_pixel_shadow_ramp_t *self, lh_u64_t square, lh_s32_t level)
{
    const lh_u64_t *thresholds = lh_pixel_shadow_ramp_get_square(self);
    const lh_s32_t count = lh_pixel_shadow_ramp_get_count(self);

    /* The alpha is how many thresholds lie beyond the distance, and they fall as the
       alpha rises, so it is found by stepping from a neighbour's. */
    while (level < count && square < thresholds[level])
    {
        ++level;
    }
    while (level > 0 && square >= thresholds[level - 1])
    {
        --level;
    }
    return level;
}
