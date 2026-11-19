/**
 * @file sampler.c
 * @brief Implementation of `lh/gl/gradient/sampler.h`.
 */

#include <lh/gl/gradient/sampler.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/core/pixel/gradient/table.h>

const lh_pixel_gradient_sampler_t *
lh_gl_gradient_sampler_get_sampler(const lh_gl_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sampler);
}

lh_pixel_gradient_sampler_t *
lh_gl_gradient_sampler_get_sampler_mut(lh_gl_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sampler);
}

const lh_gl_color_t *
lh_gl_gradient_sampler_get_colors(const lh_gl_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->colors;
}

lh_gl_color_t *
lh_gl_gradient_sampler_get_colors_mut(lh_gl_gradient_sampler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->colors;
}

lh_void
lh_gl_gradient_sampler_init(lh_gl_gradient_sampler_t *self, const lh_pixel_gradient_t *gradient,
                            const lh_gl_rect_t *rect)
{
    lh_assert_runtime_ref(gradient);
    lh_assert_runtime_ref(rect);
    lh_math_rect_t box;

    lh_gl_gradient_get_box_px(rect, lh_addr_of(box));
    lh_pixel_gradient_sampler_init(lh_gl_gradient_sampler_get_sampler_mut(self), gradient,
                                   lh_addr_of(box));
    lh_pixel_gradient_table_t table;

    lh_pixel_gradient_table_fill(lh_addr_of(table), gradient);
    for (lh_s32_t frac = 0; frac < 256; ++frac)
    {
        lh_gl_color_init_argb(lh_addr_of(lh_gl_gradient_sampler_get_colors_mut(self)[frac]),
                              lh_pixel_gradient_table_get_colors(lh_addr_of(table))[frac]);
    }
}

lh_void
lh_gl_gradient_sampler_map(const lh_gl_gradient_sampler_t *self, const lh_byte_t *levels,
                           lh_s32_t count, lh_gl_color_t *out)
{
    const lh_gl_color_t *colors = lh_gl_gradient_sampler_get_colors(self);

    for (lh_s32_t i = 0; i < count; ++i)
    {
        out[i] = colors[levels[i]];
    }
}

lh_void
lh_gl_gradient_sampler_row(const lh_gl_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1,
                           lh_s32_t y, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_byte_t levels[LH_GL_GRADIENT_SAMPLER_CHUNK];

    for (lh_s32_t x = x0; x < x1; x += LH_GL_GRADIENT_SAMPLER_CHUNK)
    {
        const lh_s32_t end = lh_math_min(x1, x + LH_GL_GRADIENT_SAMPLER_CHUNK);

        lh_pixel_gradient_sampler_row(lh_gl_gradient_sampler_get_sampler(self), x, end, y, levels);
        lh_gl_gradient_sampler_map(self, levels, end - x, out + (x - x0));
    }
}

lh_void
lh_gl_gradient_sampler_column(const lh_gl_gradient_sampler_t *self, lh_s32_t x, lh_s32_t y0,
                              lh_s32_t y1, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_byte_t levels[LH_GL_GRADIENT_SAMPLER_CHUNK];

    for (lh_s32_t y = y0; y < y1; y += LH_GL_GRADIENT_SAMPLER_CHUNK)
    {
        const lh_s32_t end = lh_math_min(y1, y + LH_GL_GRADIENT_SAMPLER_CHUNK);

        lh_pixel_gradient_sampler_column(lh_gl_gradient_sampler_get_sampler(self), x, y, end,
                                         levels);
        lh_gl_gradient_sampler_map(self, levels, end - y, out + (y - y0));
    }
}
