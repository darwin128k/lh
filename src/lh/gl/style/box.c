/**
 * @file box.c
 * @brief Implementation of `lh/gl/style/box.h`.
 */

#include <lh/gl/style/box.h>

#include <lh/core/assert.h>

const lh_gl_paint_t *
lh_gl_style_box_get_fill(const lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

lh_gl_paint_t *
lh_gl_style_box_get_fill_mut(lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

lh_void
lh_gl_style_box_set_fill(lh_gl_style_box_t *self, lh_gl_paint_t fill)
{
    lh_assert_runtime_ref(self);
    self->fill = fill;
}

const lh_gl_radii_t *
lh_gl_style_box_get_radii(const lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->radii);
}

lh_gl_radii_t *
lh_gl_style_box_get_radii_mut(lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->radii);
}

lh_void
lh_gl_style_box_set_radii(lh_gl_style_box_t *self, lh_gl_radii_t radii)
{
    lh_assert_runtime_ref(self);
    self->radii = radii;
}

const lh_gl_insets_t *
lh_gl_style_box_get_padding(const lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->padding);
}

lh_gl_insets_t *
lh_gl_style_box_get_padding_mut(lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->padding);
}

lh_void
lh_gl_style_box_set_padding(lh_gl_style_box_t *self, lh_gl_insets_t padding)
{
    lh_assert_runtime_ref(self);
    self->padding = padding;
}

const lh_gl_shadow_t *
lh_gl_style_box_get_shadow(const lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->shadow);
}

lh_gl_shadow_t *
lh_gl_style_box_get_shadow_mut(lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->shadow);
}

lh_void
lh_gl_style_box_set_shadow(lh_gl_style_box_t *self, lh_gl_shadow_t shadow)
{
    lh_assert_runtime_ref(self);
    self->shadow = shadow;
}

const lh_gl_insets_t *
lh_gl_style_box_get_border(const lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border);
}

lh_gl_insets_t *
lh_gl_style_box_get_border_mut(lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border);
}

lh_void
lh_gl_style_box_set_border(lh_gl_style_box_t *self, lh_gl_insets_t border)
{
    lh_assert_runtime_ref(self);
    self->border = border;
}

const lh_gl_paint_t *
lh_gl_style_box_get_border_fill(const lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border_fill);
}

lh_gl_paint_t *
lh_gl_style_box_get_border_fill_mut(lh_gl_style_box_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->border_fill);
}

lh_void
lh_gl_style_box_set_border_fill(lh_gl_style_box_t *self, lh_gl_paint_t border_fill)
{
    lh_assert_runtime_ref(self);
    self->border_fill = border_fill;
}

lh_void
lh_gl_style_box_init(lh_gl_style_box_t *self)
{
    lh_gl_paint_init(lh_gl_style_box_get_fill_mut(self));
    lh_gl_radii_init_all(lh_gl_style_box_get_radii_mut(self), lh_gl_scalar(0));
    lh_gl_insets_init_all(lh_gl_style_box_get_padding_mut(self), lh_gl_scalar(0));
    lh_gl_shadow_init(lh_gl_style_box_get_shadow_mut(self));
    /* No edge until a caller asks for one. A style that grew a border and
       painted it on every existing widget would put a line on things that
       were square on purpose. */
    lh_gl_insets_init_all(lh_gl_style_box_get_border_mut(self), lh_gl_scalar(0));
    lh_gl_paint_init(lh_gl_style_box_get_border_fill_mut(self));
}
