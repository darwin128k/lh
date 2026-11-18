/**
 * @file style.c
 * @brief Implementation of `lh/gx/style.h`.
 */

#include <lh/gx/style.h>

lh_gx_color_t
lh_gx_style_get_fill(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fill;
}

lh_void
lh_gx_style_set_fill(lh_gx_style_t *self, lh_gx_color_t fill)
{
    lh_assert_runtime_ref(self);
    self->fill = fill;
}

lh_gx_color_t
lh_gx_style_get_border(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->border;
}

lh_void
lh_gx_style_set_border(lh_gx_style_t *self, lh_gx_color_t border)
{
    lh_assert_runtime_ref(self);
    self->border = border;
}

lh_gx_scalar_t
lh_gx_style_get_border_width(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->border_width;
}

lh_void
lh_gx_style_set_border_width(lh_gx_style_t *self, lh_gx_scalar_t border_width)
{
    lh_assert_runtime_ref(self);
    self->border_width = border_width;
}

lh_gx_scalar_t
lh_gx_style_get_radius(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_gx_style_set_radius(lh_gx_style_t *self, lh_gx_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius;
}

lh_void
lh_gx_style_init(lh_gx_style_t *self, lh_gx_color_t fill, lh_gx_color_t border,
                 lh_gx_scalar_t border_width, lh_gx_scalar_t radius)
{
    lh_gx_style_set_fill(self, fill);
    lh_gx_style_set_border(self, border);
    lh_gx_style_set_border_width(self, border_width);
    lh_gx_style_set_radius(self, radius);
}
