/**
 * @file font.c
 * @brief Lookup in a borrowed row of glyphs.
 */
#include <lh/gl/font.h>

#include <lh/core/assert/runtime.h>

const lh_gl_glyph_t *
lh_gl_font_get_glyphs(const lh_gl_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyphs;
}

lh_u32_t
lh_gl_font_get_first(const lh_gl_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_u32_t
lh_gl_font_get_count(const lh_gl_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_gl_scalar_t
lh_gl_font_get_line_height(const lh_gl_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line_height;
}

lh_gl_scalar_t
lh_gl_font_get_ascent(const lh_gl_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ascent;
}

lh_void
lh_gl_font_set_glyphs(lh_gl_font_t *self, const lh_gl_glyph_t *glyphs)
{
    lh_assert_runtime_ref(self);
    self->glyphs = glyphs;
}

lh_void
lh_gl_font_set_first(lh_gl_font_t *self, lh_u32_t first)
{
    lh_assert_runtime_ref(self);
    self->first = first;
}

lh_void
lh_gl_font_set_count(lh_gl_font_t *self, lh_u32_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_void
lh_gl_font_set_line_height(lh_gl_font_t *self, lh_gl_scalar_t line_height)
{
    lh_assert_runtime_ref(self);
    self->line_height = line_height;
}

lh_void
lh_gl_font_set_ascent(lh_gl_font_t *self, lh_gl_scalar_t ascent)
{
    lh_assert_runtime_ref(self);
    self->ascent = ascent;
}

const lh_gl_glyph_t *
lh_gl_font_get_glyph(const lh_gl_font_t *self, lh_u32_t code)
{
    lh_u32_t first = lh_gl_font_get_first(self);

    if (code < first)
    {
        return lh_null;
    }
    lh_u32_t index = code - first;

    if (index >= lh_gl_font_get_count(self) || lh_ptr_is_null(lh_gl_font_get_glyphs(self)))
    {
        return lh_null;
    }
    return lh_ptr_add_by_offset(const lh_gl_glyph_t, lh_gl_font_get_glyphs(self),
                                (lh_uaddr_t)index * sizeof(lh_gl_glyph_t));
}

lh_void
lh_gl_font_init(lh_gl_font_t *self, const lh_gl_glyph_t *glyphs, lh_u32_t first, lh_u32_t count,
                lh_gl_scalar_t line_height, lh_gl_scalar_t ascent)
{
    lh_gl_font_set_glyphs(self, glyphs);
    lh_gl_font_set_first(self, first);
    lh_gl_font_set_count(self, count);
    lh_gl_font_set_line_height(self, line_height);
    lh_gl_font_set_ascent(self, ascent);
}
