/**
 * @file typography.c
 * @brief Implementation of `lh/gl/style/typography.h`.
 */

#include <lh/gl/style/typography.h>

#include <lh/core/assert.h>

const lh_gl_paint_t *
lh_gl_style_typography_get_paint(const lh_gl_style_typography_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->paint);
}

lh_gl_paint_t *
lh_gl_style_typography_get_paint_mut(lh_gl_style_typography_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->paint);
}

lh_void
lh_gl_style_typography_set_paint(lh_gl_style_typography_t *self, lh_gl_paint_t paint)
{
    lh_assert_runtime_ref(self);
    self->paint = paint;
}

const lh_gl_font_t *
lh_gl_style_typography_get_font(const lh_gl_style_typography_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_gl_style_typography_set_font(lh_gl_style_typography_t *self, const lh_gl_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
}

lh_gl_text_align_h_t
lh_gl_style_typography_get_align_h(const lh_gl_style_typography_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_h;
}

lh_void
lh_gl_style_typography_set_align_h(lh_gl_style_typography_t *self, lh_gl_text_align_h_t align_h)
{
    lh_assert_runtime_ref(self);
    self->align_h = align_h;
}

lh_gl_text_align_v_t
lh_gl_style_typography_get_align_v(const lh_gl_style_typography_t *self)
{
    lh_assert_runtime_ref(self);
    return self->align_v;
}

lh_void
lh_gl_style_typography_set_align_v(lh_gl_style_typography_t *self, lh_gl_text_align_v_t align_v)
{
    lh_assert_runtime_ref(self);
    self->align_v = align_v;
}

lh_gl_text_overflow_t
lh_gl_style_typography_get_overflow(const lh_gl_style_typography_t *self)
{
    lh_assert_runtime_ref(self);
    return self->overflow;
}

lh_void
lh_gl_style_typography_set_overflow(lh_gl_style_typography_t *self, lh_gl_text_overflow_t overflow)
{
    lh_assert_runtime_ref(self);
    self->overflow = overflow;
}

lh_gl_text_wrap_t
lh_gl_style_typography_get_wrap(const lh_gl_style_typography_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wrap;
}

lh_void
lh_gl_style_typography_set_wrap(lh_gl_style_typography_t *self, lh_gl_text_wrap_t wrap)
{
    lh_assert_runtime_ref(self);
    self->wrap = wrap;
}

lh_void
lh_gl_style_typography_init(lh_gl_style_typography_t *self)
{
    lh_gl_color_t black;

    lh_gl_color_init(lh_addr_of(black), 0, 0, 0, 255);
    lh_gl_style_typography_set_font(self, lh_gl_font_get_default());
    lh_gl_paint_init_color(lh_gl_style_typography_get_paint_mut(self), lh_addr_of(black));
    /* Text starts at the top left of the padded box, which is what it did before
       alignment was a thing a style could say. */
    lh_gl_style_typography_set_align_h(self, lh_gl_text_align_h_left);
    lh_gl_style_typography_set_align_v(self, lh_gl_text_align_v_top);
    /* Drawn whole and clipped, which is what a label did before it could say more. */
    lh_gl_style_typography_set_overflow(self, lh_gl_text_overflow_clip);
    lh_gl_style_typography_set_wrap(self, lh_gl_text_wrap_none);
}
