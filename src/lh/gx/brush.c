/**
 * @file brush.c
 * @brief Implementation of `lh/gx/brush.h`.
 */

#include <lh/gx/brush.h>

#include <lh/core/pixel/argb.h>

lh_gx_color_t
lh_gx_brush_get_color(const lh_gx_brush_t *self)
{
    lh_assert_runtime_ref(self);
    return self->color;
}

lh_void
lh_gx_brush_set_color(lh_gx_brush_t *self, lh_gx_color_t color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
}

const lh_pixel_gradient_t *
lh_gx_brush_get_gradient(const lh_gx_brush_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gradient;
}

lh_void
lh_gx_brush_set_gradient(lh_gx_brush_t *self, const lh_pixel_gradient_t *gradient)
{
    lh_assert_runtime_ref(self);
    self->gradient = gradient;
}

const lh_pixel_gradient_table_t *
lh_gx_brush_get_table(const lh_gx_brush_t *self)
{
    lh_assert_runtime_ref(self);
    return self->table;
}

lh_void
lh_gx_brush_set_table(lh_gx_brush_t *self, const lh_pixel_gradient_table_t *table)
{
    lh_assert_runtime_ref(self);
    self->table = table;
}

const lh_gx_rect_t *
lh_gx_brush_get_box(const lh_gx_brush_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->box);
}

lh_void
lh_gx_brush_set_box(lh_gx_brush_t *self, lh_gx_rect_t box)
{
    lh_assert_runtime_ref(self);
    self->box = box;
}

lh_bool_t
lh_gx_brush_is_gradient(const lh_gx_brush_t *self)
{
    return lh_null_ne(lh_gx_brush_get_gradient(self));
}

lh_gx_color_t
lh_gx_brush_get_color_at(const lh_gx_brush_t *self, lh_s32_t x, lh_s32_t y)
{
    const lh_pixel_gradient_t *gradient = lh_gx_brush_get_gradient(self);

    lh_return_if(lh_null_eq(gradient), lh_gx_brush_get_color(self));
    lh_math_rect_t box;

    lh_gx_rect_to_rect(lh_gx_brush_get_box(self), lh_addr_of(box));
    const lh_byte_t frac = lh_pixel_gradient_get_frac_at(gradient, lh_addr_of(box), x, y);
    const lh_pixel_gradient_table_t *table = lh_gx_brush_get_table(self);

    lh_return_if(lh_null_ne(table), lh_pixel_gradient_table_get_colors(table)[frac]);
    return lh_pixel_gradient_get_color_at(gradient, frac);
}

const lh_pixel_gradient_table_t *
lh_gx_brush_prepare(const lh_gx_brush_t *self, lh_pixel_gradient_table_t *own,
                    lh_pixel_gradient_sampler_t *sampler)
{
    const lh_pixel_gradient_t *gradient = lh_gx_brush_get_gradient(self);
    const lh_pixel_gradient_table_t *table = lh_gx_brush_get_table(self);
    lh_math_rect_t box;

    lh_gx_rect_to_rect(lh_gx_brush_get_box(self), lh_addr_of(box));
    lh_pixel_gradient_sampler_init(sampler, gradient, lh_addr_of(box));
    lh_return_if(lh_null_ne(table), table);
    lh_pixel_gradient_table_fill(own, gradient);
    return own;
}

lh_void
lh_gx_brush_init_color(lh_gx_brush_t *self, lh_gx_color_t color)
{
    lh_gx_rect_t none;

    lh_gx_rect_init(lh_addr_of(none), 0, 0, 0, 0);
    lh_gx_brush_set_color(self, color);
    lh_gx_brush_set_gradient(self, lh_null);
    lh_gx_brush_set_table(self, lh_null);
    lh_gx_brush_set_box(self, none);
}

lh_void
lh_gx_brush_init_gradient(lh_gx_brush_t *self, const lh_pixel_gradient_t *gradient,
                          const lh_pixel_gradient_table_t *table, lh_gx_rect_t box)
{
    lh_gx_brush_set_color(self, lh_pixel_gradient_get_color_at(gradient, 0));
    lh_gx_brush_set_gradient(self, gradient);
    lh_gx_brush_set_table(self, table);
    lh_gx_brush_set_box(self, box);
}

lh_bool_t
lh_gx_brush_is_clear(const lh_gx_brush_t *self)
{
    return !lh_gx_brush_is_gradient(self) &&
           lh_pixel_argb_get_channel(lh_gx_brush_get_color(self), 24) == 0U;
}
