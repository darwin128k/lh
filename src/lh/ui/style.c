#include <lh/ui/style.h>
#include <lh/assert.h>

lh_void
lh_ui_style_init(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    self->bg_color = lh_ui_color_make(0U, 0U, 0U, 0U);
    self->text_color = lh_ui_color_make(0U, 0U, 0U, 0U);
    self->border_color = lh_ui_color_make(0U, 0U, 0U, 0U);
    self->border_width = 0;
    self->radius = 0;
}

lh_ui_color_t
lh_ui_style_get_bg_color(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bg_color;
}

lh_void
lh_ui_style_set_bg_color(lh_ui_style_t *self, lh_ui_color_t bg_color)
{
    lh_assert_runtime_ref(self);
    self->bg_color = bg_color;
}

lh_ui_color_t
lh_ui_style_get_text_color(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text_color;
}

lh_void
lh_ui_style_set_text_color(lh_ui_style_t *self, lh_ui_color_t text_color)
{
    lh_assert_runtime_ref(self);
    self->text_color = text_color;
}

lh_ui_color_t
lh_ui_style_get_border_color(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->border_color;
}

lh_void
lh_ui_style_set_border_color(lh_ui_style_t *self, lh_ui_color_t border_color)
{
    lh_assert_runtime_ref(self);
    self->border_color = border_color;
}

lh_int_t
lh_ui_style_get_border_width(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->border_width;
}

lh_void
lh_ui_style_set_border_width(lh_ui_style_t *self, lh_int_t border_width)
{
    lh_assert_runtime_ref(self);
    self->border_width = border_width < 0 ? 0 : border_width;
}

lh_int_t
lh_ui_style_get_radius(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_ui_style_set_radius(lh_ui_style_t *self, lh_int_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
}

lh_void
lh_ui_style_set_border(lh_ui_style_t *self, lh_ui_color_t color, lh_int_t width)
{
    lh_assert_runtime_ref(self);
    self->border_color = color;
    self->border_width = width < 0 ? 0 : width;
}

lh_void
lh_ui_style_clear_border(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    self->border_color = lh_ui_color_make(0U, 0U, 0U, 0U);
    self->border_width = 0;
}
