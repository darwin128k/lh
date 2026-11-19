/**
 * @file style.c
 * @brief Implementation of `lh/gx/style.h`.
 */

#include <lh/gx/style.h>

#include <lh/core/pixel/argb.h>

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

const lh_gx_corners_t *
lh_gx_style_get_radius(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->radius);
}

lh_gx_corners_t *
lh_gx_style_get_radius_mut(lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->radius);
}

lh_void
lh_gx_style_set_radius(lh_gx_style_t *self, const lh_gx_corners_t *radius)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(radius);
    self->radius = *radius;
}

lh_gx_color_t
lh_gx_style_get_ink(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ink;
}

lh_void
lh_gx_style_set_ink(lh_gx_style_t *self, lh_gx_color_t ink)
{
    lh_assert_runtime_ref(self);
    self->ink = ink;
}

const lh_gx_font_t *
lh_gx_style_get_font(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_gx_style_set_font(lh_gx_style_t *self, const lh_gx_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
}

const lh_pixel_gradient_t *
lh_gx_style_get_gradient(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gradient;
}

lh_void
lh_gx_style_set_gradient(lh_gx_style_t *self, const lh_pixel_gradient_t *gradient)
{
    lh_assert_runtime_ref(self);
    self->gradient = gradient;
}

const lh_pixel_gradient_table_t *
lh_gx_style_get_gradient_table(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gradient_table;
}

lh_void
lh_gx_style_set_gradient_table(lh_gx_style_t *self, const lh_pixel_gradient_table_t *table)
{
    lh_assert_runtime_ref(self);
    self->gradient_table = table;
}

const lh_gx_shadow_t *
lh_gx_style_get_shadow(const lh_gx_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shadow;
}

lh_void
lh_gx_style_set_shadow(lh_gx_style_t *self, const lh_gx_shadow_t *shadow)
{
    lh_assert_runtime_ref(self);
    self->shadow = shadow;
}

lh_bool_t
lh_gx_style_is_fill_opaque(const lh_gx_style_t *self)
{
    const lh_pixel_gradient_t *gradient = lh_gx_style_get_gradient(self);

    lh_return_if(lh_null_ne(gradient), lh_pixel_gradient_is_opaque(gradient));
    return lh_pixel_argb_get_channel(lh_gx_style_get_fill(self), 24) == LH_BYTE_T_MAX;
}

lh_void
lh_gx_style_set_radius_all(lh_gx_style_t *self, lh_gx_scalar_t radius)
{
    lh_gx_corners_init_all(lh_gx_style_get_radius_mut(self), radius);
}

lh_void
lh_gx_style_init(lh_gx_style_t *self, lh_gx_color_t fill, lh_gx_color_t border,
                 lh_gx_scalar_t border_width, lh_gx_scalar_t radius, lh_gx_color_t ink,
                 const lh_gx_font_t *font)
{
    lh_gx_style_set_fill(self, fill);
    lh_gx_style_set_border(self, border);
    lh_gx_style_set_border_width(self, border_width);
    lh_gx_style_set_radius_all(self, radius);
    lh_gx_style_set_ink(self, ink);
    lh_gx_style_set_font(self, font);
    lh_gx_style_set_gradient(self, lh_null);
    lh_gx_style_set_gradient_table(self, lh_null);
    lh_gx_style_set_shadow(self, lh_null);
}
