#include <lh/ui/font.h>
#include <lh/assert.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

const lh_ui_font_t *
lh_ui_font_get_default(void)
{
    return lh_addr_of(LH_LIBRARY_OPTION_UI_FONT);
}

lh_int_t
lh_ui_font_row_bytes(lh_int_t glyph_width, lh_int_t bpp)
{
    return (glyph_width * bpp + 7) / 8;
}

lh_bool_t
lh_ui_font_bpp_ok(lh_int_t bpp)
{
    return bpp == 1 || bpp == 2 || bpp == 4 || bpp == 8;
}

lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_memory_view_t *glyphs, const lh_memory_view_t *advances,
                lh_int_t glyph_width, lh_int_t glyph_height, lh_int_t bpp, lh_int_t first,
                lh_int_t count)
{
    const lh_int_t row_bytes = lh_ui_font_row_bytes(glyph_width, bpp);
    const lh_usize_t bytes = lh_cast_static(lh_usize_t, count) * lh_cast_static(lh_usize_t, row_bytes) *
                             lh_cast_static(lh_usize_t, glyph_height);

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(glyphs);
    lh_assert_runtime_if(glyph_width <= 0 || glyph_height <= 0 || count <= 0 || !lh_ui_font_bpp_ok(bpp),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_memory_view_get_size(glyphs) < bytes, lh_runtime_error_code_invalid_argument);
    lh_memory_view_init_by_other(lh_addr_of(self->glyphs), glyphs);
    if (lh_ptr_is_set(advances))
    {
        lh_assert_runtime_if(lh_memory_view_get_size(advances) < lh_cast_static(lh_usize_t, count),
                             lh_runtime_error_code_invalid_argument);
        lh_memory_view_init_by_other(lh_addr_of(self->advances), advances);
    }
    else
    {
        lh_memory_view_init_empty(lh_addr_of(self->advances));
    }
    self->glyph_width = glyph_width;
    self->glyph_height = glyph_height;
    self->row_bytes = row_bytes;
    self->bpp = bpp;
    self->first = first;
    self->count = count;
}

lh_memory_view_t
lh_ui_font_get_glyphs(const lh_ui_font_t *self)
{
    lh_memory_view_t glyphs;

    lh_assert_runtime_ref(self);
    lh_memory_view_init_by_other(lh_addr_of(glyphs), lh_addr_of(self->glyphs));
    return glyphs;
}

lh_int_t
lh_ui_font_get_glyph_width(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyph_width;
}

lh_int_t
lh_ui_font_get_glyph_height(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyph_height;
}

lh_int_t
lh_ui_font_index(const lh_ui_font_t *self, lh_char_t ch)
{
    return lh_cast_static(lh_int_t, lh_cast_static(lh_byte_t, ch)) - self->first;
}

const lh_byte_t *
lh_ui_font_glyph(const lh_ui_font_t *self, lh_char_t ch)
{
    const lh_int_t index = lh_ui_font_index(self, ch);
    const lh_byte_t *bytes;
    lh_usize_t offset;

    if (index < 0 || index >= self->count)
    {
        return lh_null;
    }
    offset = lh_cast_static(lh_usize_t, index) * lh_cast_static(lh_usize_t, self->row_bytes) *
             lh_cast_static(lh_usize_t, self->glyph_height);
    if (offset + lh_cast_static(lh_usize_t, self->row_bytes) * lh_cast_static(lh_usize_t, self->glyph_height) >
        lh_memory_view_get_size(lh_addr_of(self->glyphs)))
    {
        return lh_null;
    }
    bytes = lh_ptr_rcast(const lh_byte_t, lh_memory_view_get_data(lh_addr_of(self->glyphs)));
    return bytes + offset;
}

lh_int_t
lh_ui_font_advance(const lh_ui_font_t *self, lh_char_t ch)
{
    const lh_int_t index = lh_ui_font_index(self, ch);
    const lh_byte_t *advances;

    if (lh_memory_view_is_uninitialized(lh_addr_of(self->advances)) || index < 0 || index >= self->count)
    {
        return self->glyph_width;
    }
    advances = lh_ptr_rcast(const lh_byte_t, lh_memory_view_get_data(lh_addr_of(self->advances)));
    return advances[index];
}

lh_math_vec2_t
lh_ui_font_measure(const lh_ui_font_t *self, const lh_char_t *text)
{
    lh_int_t line = 0;
    lh_int_t widest = 0;
    lh_int_t lines = 1;

    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(text) || text[0] == '\0')
    {
        return lh_math_vec2_make(0.0f, 0.0f);
    }
    for (const lh_char_t *cursor = text; *cursor != '\0'; ++cursor)
    {
        if (*cursor == '\n')
        {
            if (line > widest)
            {
                widest = line;
            }
            line = 0;
            lines += 1;
        }
        else
        {
            line += lh_ui_font_advance(self, *cursor);
        }
    }
    if (line > widest)
    {
        widest = line;
    }
    return lh_math_vec2_make(lh_cast_static(lh_float_t, widest),
                             lh_cast_static(lh_float_t, lines * self->glyph_height));
}

lh_void
lh_ui_font_draw(const lh_ui_font_t *self, lh_ui_canvas_t *canvas, lh_int_t x, lh_int_t y,
                const lh_char_t *text, lh_ui_color_t color)
{
    lh_int_t pen_x = x;
    lh_int_t pen_y = y;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(canvas);
    if (lh_ptr_is_null(text) || (lh_ui_color_to_argb(color) >> 24) == 0U)
    {
        return;
    }
    for (const lh_char_t *cursor = text; *cursor != '\0'; ++cursor)
    {
        const lh_byte_t *glyph;
        if (*cursor == '\n')
        {
            pen_x = x;
            pen_y += self->glyph_height;
            continue;
        }
        glyph = lh_ui_font_glyph(self, *cursor);
        if (lh_ptr_is_set(glyph))
        {
            const lh_int_t levels = (1 << self->bpp) - 1;
            const lh_int_t mask = levels;
            for (lh_int_t row = 0; row < self->glyph_height; ++row)
            {
                const lh_byte_t *const bits = glyph + row * self->row_bytes;
                for (lh_int_t column = 0; column < self->glyph_width; ++column)
                {
                    const lh_int_t bit = column * self->bpp;
                    const lh_int_t value =
                        (bits[bit / 8] >> (8 - self->bpp - (bit % 8))) & mask;
                    lh_ui_color_t ink = color;
                    if (value == 0)
                    {
                        continue;
                    }
                    if (value < levels)
                    {
                        const lh_uint_t argb = lh_ui_color_to_argb(color);
                        const lh_uint_t alpha = ((argb >> 24) * lh_cast_static(lh_uint_t, value)) /
                                                lh_cast_static(lh_uint_t, levels);
                        ink = lh_ui_color_from_argb((argb & 0x00FFFFFFU) | (alpha << 24));
                    }
                    lh_ui_canvas_blend_pixel(canvas, pen_x + column, pen_y + row, ink);
                }
            }
        }
        pen_x += lh_ui_font_advance(self, *cursor);
    }
}
