/**
 * @file font.c
 * @brief Implementation of `lh/gx/font.h`.
 */

#include <lh/gx/font.h>

#include <lh/core/assert/runtime.h>
lh_gx_font_get_glyph_fn *
lh_gx_font_get_get_glyph_fn(const lh_gx_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->get_glyph;
}

lh_void
lh_gx_font_set_get_glyph_fn(lh_gx_font_t *self, lh_gx_font_get_glyph_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->get_glyph = fn;
}

lh_gx_font_get_ascent_fn *
lh_gx_font_get_get_ascent_fn(const lh_gx_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->get_ascent;
}

lh_void
lh_gx_font_set_get_ascent_fn(lh_gx_font_t *self, lh_gx_font_get_ascent_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->get_ascent = fn;
}

lh_gx_font_get_line_height_fn *
lh_gx_font_get_get_line_height_fn(const lh_gx_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->get_line_height;
}

lh_void
lh_gx_font_set_get_line_height_fn(lh_gx_font_t *self, lh_gx_font_get_line_height_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->get_line_height = fn;
}

lh_ptr
lh_gx_font_get_context(const lh_gx_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_gx_font_set_context(lh_gx_font_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_bool_t
lh_gx_font_get_glyph(const lh_gx_font_t *self, lh_u32_t code, lh_gx_glyph_t *out)
{
    return lh_gx_font_get_get_glyph_fn(self)(lh_gx_font_get_context(self), code, out);
}

lh_s32_t
lh_gx_font_get_ascent(const lh_gx_font_t *self)
{
    return lh_gx_font_get_get_ascent_fn(self)(lh_gx_font_get_context(self));
}

lh_s32_t
lh_gx_font_get_line_height(const lh_gx_font_t *self)
{
    return lh_gx_font_get_get_line_height_fn(self)(lh_gx_font_get_context(self));
}

lh_void
lh_gx_font_init(lh_gx_font_t *self, lh_gx_font_get_glyph_fn *get_glyph,
                lh_gx_font_get_ascent_fn *get_ascent,
                lh_gx_font_get_line_height_fn *get_line_height, lh_ptr context)
{
    lh_gx_font_set_get_glyph_fn(self, get_glyph);
    lh_gx_font_set_get_ascent_fn(self, get_ascent);
    lh_gx_font_set_get_line_height_fn(self, get_line_height);
    lh_gx_font_set_context(self, context);
}
