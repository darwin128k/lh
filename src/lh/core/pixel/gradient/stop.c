/**
 * @file stop.c
 * @brief Implementation of `lh/core/pixel/gradient/stop.h`.
 */

#include <lh/core/pixel/gradient/stop.h>

#include <lh/core/assert.h>

lh_u32_t
lh_pixel_gradient_stop_get_color(const lh_pixel_gradient_stop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->color;
}

lh_void
lh_pixel_gradient_stop_set_color(lh_pixel_gradient_stop_t *self, lh_u32_t color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
}

lh_byte_t
lh_pixel_gradient_stop_get_frac(const lh_pixel_gradient_stop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frac;
}

lh_void
lh_pixel_gradient_stop_set_frac(lh_pixel_gradient_stop_t *self, lh_byte_t frac)
{
    lh_assert_runtime_ref(self);
    self->frac = frac;
}

lh_void
lh_pixel_gradient_stop_init(lh_pixel_gradient_stop_t *self, lh_u32_t color, lh_byte_t frac)
{
    lh_pixel_gradient_stop_set_color(self, color);
    lh_pixel_gradient_stop_set_frac(self, frac);
}
