/**
 * @file style.c
 * @brief Implementation of `lh/ui/style.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_ui_style_init(lh_ui_style_t *self)
{
    lh_ui_color_t black;

    lh_assert_runtime_ref(self);
    lh_ui_color_init(lh_addr_of(black), 0, 0, 0, 255);
    lh_ui_paint_init(lh_addr_of(self->fill));
    self->radius = lh_ui_scalar(0);
    lh_ui_insets_init_all(lh_addr_of(self->padding), lh_ui_scalar(0));
    self->font = lh_ui_font_get_default();
    lh_ui_paint_init_color(lh_addr_of(self->text), lh_addr_of(black));
    /* Text starts at the top left of the padded box, which is what it did before
       alignment was a thing a style could say. */
    self->align_h = lh_ui_text_align_h_left;
    self->align_v = lh_ui_text_align_v_top;
}

const lh_ui_paint_t *
lh_ui_style_get_fill(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->fill);
}

lh_void
lh_ui_style_set_fill(lh_ui_style_t *self, const lh_ui_paint_t *fill)
{
    lh_assert_runtime_ref(self);
    lh_ui_paint_init_copy(lh_addr_of(self->fill), fill);
}

const lh_ui_color_t *
lh_ui_style_get_fill_color(const lh_ui_style_t *self)
{
    return lh_ui_paint_get_color(lh_ui_style_get_fill(self));
}

lh_ui_scalar_t
lh_ui_style_get_radius(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_ui_style_set_radius(lh_ui_style_t *self, lh_ui_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(radius < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    self->radius = radius;
}

const lh_ui_insets_t *
lh_ui_style_get_padding(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->padding);
}

lh_void
lh_ui_style_set_padding(lh_ui_style_t *self, lh_ui_scalar_t padding)
{
    lh_ui_insets_t all;

    lh_ui_insets_init_all(lh_addr_of(all), padding);
    lh_ui_style_set_padding_insets(self, lh_addr_of(all));
}

lh_void
lh_ui_style_set_padding_insets(lh_ui_style_t *self, const lh_ui_insets_t *padding)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(padding);
    lh_assert_runtime_if(padding->left < lh_ui_scalar(0) || padding->top < lh_ui_scalar(0) ||
                             padding->right < lh_ui_scalar(0) || padding->bottom < lh_ui_scalar(0),
                         lh_runtime_error_code_invalid_argument);
    self->padding = *padding;
}

const lh_ui_font_t *
lh_ui_style_get_font(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_ui_style_set_font(lh_ui_style_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
}

const lh_ui_paint_t *
lh_ui_style_get_text(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_void
lh_ui_style_set_text(lh_ui_style_t *self, const lh_ui_paint_t *text)
{
    lh_assert_runtime_ref(self);
    lh_ui_paint_init_copy(lh_addr_of(self->text), text);
}

const lh_ui_color_t *
lh_ui_style_get_text_color(const lh_ui_style_t *self)
{
    return lh_ui_paint_get_color(lh_ui_style_get_text(self));
}

lh_ui_text_align_h_t
lh_ui_style_get_align_h(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_h;
}

lh_void
lh_ui_style_set_align_h(lh_ui_style_t *self, lh_ui_text_align_h_t align)
{
    lh_assert_runtime_ref(self);
    self->align_h = align;
}

lh_ui_text_align_v_t
lh_ui_style_get_align_v(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_v;
}

lh_void
lh_ui_style_set_align_v(lh_ui_style_t *self, lh_ui_text_align_v_t align)
{
    lh_assert_runtime_ref(self);
    self->align_v = align;
}
