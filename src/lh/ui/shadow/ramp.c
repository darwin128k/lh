/**
 * @file ramp.c
 * @brief Implementation of `lh/ui/shadow/ramp.h`.
 */
#include <lh/assert.h>

#include <lh/ui/shadow/ramp.h>

lh_void
lh_ui_shadow_ramp_init(lh_ui_shadow_ramp_t *self, const lh_ui_shadow_shape_t *shape)
{
    lh_s32_t level;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(shape);
    lh_ui_shadow_ramp_set_count(self, lh_ui_shadow_shape_get_peak(shape));
    for (level = 0; level < lh_ui_shadow_ramp_get_count(self); ++level)
    {
        /* The least distance `d` past the corner's edge whose alpha is `level` or
           less: from a spread out, where nothing is left, back to just past a spread
           in, where the peak gives way -- and between, where the fade
           `(start - d * step) >> 16` first drops to `level`, which is
           `d * step > start - ((level + 1) << 16)`. */
        const lh_s64_t limit = lh_ui_shadow_shape_get_start(shape) - (lh_cast_static(lh_s64_t, (level + 1)) << 16);
        lh_s64_t least;
        lh_s64_t reach;

        if (lh_ui_shadow_shape_get_step(shape) > 0)
        {
            least = limit / lh_ui_shadow_shape_get_step(shape);
            if (!lh_math_is_multiple_of(limit, lh_ui_shadow_shape_get_step(shape)) && limit < 0)
            {
                --least;
            }
            ++least;
        }
        else
        {
            least = limit < 0 ? lh_math_neg(lh_cast_static(lh_s64_t, lh_ui_shadow_shape_get_spread(shape)))
                              : lh_cast_static(lh_s64_t, lh_ui_shadow_shape_get_spread(shape));
        }
        if (least < 1 - lh_cast_static(lh_s64_t, lh_ui_shadow_shape_get_spread(shape)))
        {
            least = 1 - lh_cast_static(lh_s64_t, lh_ui_shadow_shape_get_spread(shape));
        }
        if (least > lh_ui_shadow_shape_get_spread(shape))
        {
            least = lh_ui_shadow_shape_get_spread(shape);
        }
        /* A pixel past the corner's centre by `root` is past its edge by
           `root - r`, and `root` is the whole square root of the squared distance,
           so `root >= least + r` is the squared distance reaching `(least + r)^2`. */
        reach = least + lh_ui_shadow_shape_get_r(shape);
        lh_ui_shadow_ramp_get_square_mut(self)[level] = reach > 0 ? lh_cast_static(lh_u64_t, (reach * reach)) : 0U;
    }
}

lh_s32_t
lh_ui_shadow_ramp_walk(const lh_ui_shadow_ramp_t *self, lh_u64_t square, lh_s32_t level)
{
    /* The alpha is how many thresholds lie beyond the distance, and they fall as the
       alpha rises, so it is found by stepping from a neighbour's. */
    while (level < lh_ui_shadow_ramp_get_count(self) && square < lh_ui_shadow_ramp_get_square(self)[level])
    {
        ++level;
    }
    while (level > 0 && square >= lh_ui_shadow_ramp_get_square(self)[level - 1])
    {
        --level;
    }
    return level;
}

const lh_u64_t *
lh_ui_shadow_ramp_get_square(const lh_ui_shadow_ramp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->square;
}

lh_u64_t *
lh_ui_shadow_ramp_get_square_mut(lh_ui_shadow_ramp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->square;
}

lh_s32_t
lh_ui_shadow_ramp_get_count(const lh_ui_shadow_ramp_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_ui_shadow_ramp_set_count(lh_ui_shadow_ramp_t *self, lh_s32_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}
