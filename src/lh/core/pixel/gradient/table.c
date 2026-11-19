/**
 * @file table.c
 * @brief Implementation of `lh/core/pixel/gradient/table.h`.
 */

#include <lh/core/pixel/gradient/table.h>

#include <lh/core/assert.h>
#include <lh/core/pixel/gradient/lerp.h>

const lh_u32_t *
lh_pixel_gradient_table_get_colors(const lh_pixel_gradient_table_t *self)
{
    lh_assert_runtime_ref(self);
    return self->colors;
}

lh_u32_t *
lh_pixel_gradient_table_get_colors_mut(lh_pixel_gradient_table_t *self)
{
    lh_assert_runtime_ref(self);
    return self->colors;
}

lh_void
lh_pixel_gradient_table_fill(lh_pixel_gradient_table_t *self, const lh_pixel_gradient_t *gradient)
{
    lh_u32_t *colors = lh_pixel_gradient_table_get_colors_mut(self);
    const lh_s32_t count = lh_pixel_gradient_get_stops_count(gradient);
    const lh_s32_t first =
        count > 1 ? lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(gradient, 0)) : 0;
    lh_s32_t carried = 0;
    lh_s32_t segment = 1;
    lh_s32_t start = first;
    lh_s32_t end =
        count > 1 ? lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(gradient, 1)) : 0;
    lh_s32_t span = end - start;
    lh_pixel_gradient_lerp_t lerp;

    for (lh_s32_t frac = 0; frac < 256; ++frac)
    {
        /* Past the end of the span: on to the span this step falls in. */
        if (count > 1 && frac > first && segment < count && frac > end)
        {
            segment = lh_pixel_gradient_get_segment_at(gradient, segment, frac);
            if (segment < count)
            {
                start = lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(
                    gradient, lh_cast_static(lh_pixel_gradient_stop_count_t, segment - 1)));
                end = lh_pixel_gradient_stop_get_frac(lh_pixel_gradient_get_stop(
                    gradient, lh_cast_static(lh_pixel_gradient_stop_count_t, segment)));
                span = end - start;
            }
        }
        /* Outside a span in order: the colour asked for alone. */
        if (count <= 1 || frac <= first || segment >= count || span <= 0 || frac - start < 0)
        {
            colors[frac] =
                lh_pixel_gradient_get_color_at(gradient, lh_cast_static(lh_byte_t, frac));
            carried = 0;
            continue;
        }
        if (carried != segment)
        {
            lh_pixel_gradient_lerp_init(
                lh_addr_of(lerp),
                lh_pixel_gradient_stop_get_color(lh_pixel_gradient_get_stop(
                    gradient, lh_cast_static(lh_pixel_gradient_stop_count_t, segment - 1))),
                lh_pixel_gradient_stop_get_color(lh_pixel_gradient_get_stop(
                    gradient, lh_cast_static(lh_pixel_gradient_stop_count_t, segment))),
                span, frac - start);
            carried = segment;
        }
        else
        {
            lh_pixel_gradient_lerp_next(lh_addr_of(lerp));
        }
        colors[frac] = lh_pixel_gradient_lerp_get_color(lh_addr_of(lerp));
    }
}

lh_void
lh_pixel_gradient_table_map(const lh_pixel_gradient_table_t *self, const lh_byte_t *levels,
                            lh_s32_t count, lh_u32_t *out)
{
    const lh_u32_t *colors = lh_pixel_gradient_table_get_colors(self);

    for (lh_s32_t i = 0; i < count; ++i)
    {
        out[i] = colors[levels[i]];
    }
}
