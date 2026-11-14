/**
 * @file glyph.c
 * @brief Accessors for one borrowed glyph.
 */
#include <lh/gl/glyph.h>

#include <lh/core/assert/runtime.h>

lh_gl_scalar_t
lh_gl_glyph_get_width(const lh_gl_glyph_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_gl_scalar_t
lh_gl_glyph_get_height(const lh_gl_glyph_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_gl_scalar_t
lh_gl_glyph_get_advance(const lh_gl_glyph_t *self)
{
    lh_assert_runtime_ref(self);
    return self->advance;
}

lh_gl_scalar_t
lh_gl_glyph_get_top(const lh_gl_glyph_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top;
}

const lh_byte_t *
lh_gl_glyph_get_bits(const lh_gl_glyph_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bits;
}

lh_void
lh_gl_glyph_set_width(lh_gl_glyph_t *self, lh_gl_scalar_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_gl_glyph_set_height(lh_gl_glyph_t *self, lh_gl_scalar_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_void
lh_gl_glyph_set_advance(lh_gl_glyph_t *self, lh_gl_scalar_t advance)
{
    lh_assert_runtime_ref(self);
    self->advance = advance;
}

lh_void
lh_gl_glyph_set_top(lh_gl_glyph_t *self, lh_gl_scalar_t top)
{
    lh_assert_runtime_ref(self);
    self->top = top;
}

lh_void
lh_gl_glyph_set_bits(lh_gl_glyph_t *self, const lh_byte_t *bits)
{
    lh_assert_runtime_ref(self);
    self->bits = bits;
}

lh_void
lh_gl_glyph_init(lh_gl_glyph_t *self, lh_gl_scalar_t width, lh_gl_scalar_t height,
                 lh_gl_scalar_t advance, lh_gl_scalar_t top, const lh_byte_t *bits)
{
    lh_gl_glyph_set_width(self, width);
    lh_gl_glyph_set_height(self, height);
    lh_gl_glyph_set_advance(self, advance);
    lh_gl_glyph_set_top(self, top);
    lh_gl_glyph_set_bits(self, bits);
}
