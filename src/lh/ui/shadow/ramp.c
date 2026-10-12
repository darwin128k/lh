/**
 * @file ramp.c
 * @brief Implementation of `lh/ui/shadow/ramp.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/shadow/ramp.h>

lh_void
lh_ui_shadow_ramp_init(lh_ui_shadow_ramp_t *self, const lh_ui_shadow_shape_t *shape)
{
    lh_s32_t level;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(shape);
    self->count = shape->peak;
    for (level = 0; level < self->count; ++level)
    {
        /* The least distance `d` past the corner's edge whose alpha is `level` or
           less: from a spread out, where nothing is left, back to just past a spread
           in, where the peak gives way -- and between, where the fade
           `(start - d * step) >> 16` first drops to `level`, which is
           `d * step > start - ((level + 1) << 16)`. */
        const lh_s64_t limit = shape->start - ((lh_s64_t)(level + 1) << 16);
        lh_s64_t least;
        lh_s64_t reach;

        if (shape->step > 0)
        {
            least = limit / shape->step;
            if (limit % shape->step != 0 && limit < 0)
            {
                --least;
            }
            ++least;
        }
        else
        {
            least = limit < 0 ? -(lh_s64_t)shape->spread : (lh_s64_t)shape->spread;
        }
        if (least < 1 - (lh_s64_t)shape->spread)
        {
            least = 1 - (lh_s64_t)shape->spread;
        }
        if (least > shape->spread)
        {
            least = shape->spread;
        }
        /* A pixel past the corner's centre by `root` is past its edge by
           `root - r`, and `root` is the whole square root of the squared distance,
           so `root >= least + r` is the squared distance reaching `(least + r)^2`. */
        reach = least + shape->r;
        self->square[level] = reach > 0 ? (lh_u64_t)(reach * reach) : 0U;
    }
}

lh_s32_t
lh_ui_shadow_ramp_walk(const lh_ui_shadow_ramp_t *self, lh_u64_t square, lh_s32_t level)
{
    /* The alpha is how many thresholds lie beyond the distance, and they fall as the
       alpha rises, so it is found by stepping from a neighbour's. */
    while (level < self->count && square < self->square[level])
    {
        ++level;
    }
    while (level > 0 && square >= self->square[level - 1])
    {
        --level;
    }
    return level;
}
