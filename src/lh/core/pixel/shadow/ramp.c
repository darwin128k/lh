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
    const lh_s64_t start = lh_pixel_shadow_get_start(shadow);

    lh_pixel_shadow_ramp_set_count(self, lh_pixel_shadow_get_peak(shadow));
    for (lh_s32_t level = 0; level < lh_pixel_shadow_ramp_get_count(self); ++level)
    {
        /* The least distance `d` past the corner's edge whose alpha is `level` or
           less: from a spread out, where nothing is left, back to just past a spread
           in, where the peak gives way -- and between, where the fade
           `(start - d * step) >> 16` first drops to `level`, which is
           `d * step > start - ((level + 1) << 16)`. */
        const lh_s64_t limit = start - (lh_cast_static(lh_s64_t, level + 1) << 16);
        lh_s64_t least = limit < 0 ? lh_math_neg(spread) : spread;

        if (step > 0)
        {
            least = limit / step;
            if (!lh_math_is_multiple_of(limit, step) && limit < 0)
            {
                --least;
            }
            ++least;
        }
        least = lh_math_clamp(least, 1 - spread, spread);
        /* A pixel past the corner's centre by `root` is past its edge by
           `root - r`, and `root` is the whole square root of the squared distance,
           so `root >= least + r` is the squared distance reaching `(least + r)^2`. */
        const lh_s64_t reach = least + lh_pixel_shadow_get_r(shadow);

        lh_pixel_shadow_ramp_get_square_mut(self)[level] =
            reach > 0 ? lh_cast_static(lh_u64_t, reach * reach) : 0U;
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
