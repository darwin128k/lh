/**
 * @file font.c
 * @brief Implementation of `lh/ui/font.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bit.h>
#include <lh/cast/static.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/font.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>
#include <lh/util/type.h>

lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_byte_t *glyphs, lh_usize_t glyph_size, const lh_byte_t *advances,
                lh_usize_t count, lh_math_coord_t cell_width, lh_math_coord_t height,
                lh_math_coord_t row_bytes, lh_byte_t bpp, lh_byte_t first)
{
    const lh_math_coord_t byte_bits = lh_cast_static(lh_math_coord_t, lh_type_bits(lh_byte_t));
    const lh_usize_t cell = lh_cast_static(lh_usize_t, height) * lh_cast_static(lh_usize_t, row_bytes);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(glyphs);
    lh_assert_runtime_ref(advances);
    lh_assert_runtime_if(count == 0 || cell_width <= 0 || height <= 0,
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8,
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_cast_static(lh_usize_t, first) + count > 256,
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(row_bytes != (cell_width * bpp + byte_bits - 1) / byte_bits,
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(glyph_size != count * cell, lh_runtime_error_code_invalid_argument);
    self->glyphs = lh_memory_view_make_by_size(lh_cast_static(lh_ptr, glyphs), glyph_size);
    self->advances = lh_memory_view_make_by_size(lh_cast_static(lh_ptr, advances), count);
    self->cell_width = cell_width;
    self->height = height;
    self->row_bytes = row_bytes;
    self->bpp = bpp;
    self->first = first;
    self->count = count;
}

lh_memory_view_t
lh_ui_font_get_glyphs(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->glyphs;
}

lh_memory_view_t
lh_ui_font_get_advances(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->advances;
}

lh_math_coord_t
lh_ui_font_get_cell_width(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cell_width;
}

lh_math_coord_t
lh_ui_font_get_height(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_math_coord_t
lh_ui_font_get_row_bytes(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->row_bytes;
}

lh_byte_t
lh_ui_font_get_bpp(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bpp;
}

lh_byte_t
lh_ui_font_get_first(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_usize_t
lh_ui_font_get_count(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_uint_t
lh_ui_font_get_last(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_uint_t, self->first) + lh_cast_static(lh_uint_t, self->count) - 1;
}

lh_bool_t
lh_ui_font_has_code(const lh_ui_font_t *self, lh_uint_t code)
{
    lh_assert_runtime_ref(self);
    return code >= lh_ui_font_get_first(self) && code <= lh_ui_font_get_last(self);
}

lh_usize_t
lh_ui_font_get_index(const lh_ui_font_t *self, lh_uint_t code)
{
    lh_assert_runtime_ref(self);
    lh_return_if(!lh_ui_font_has_code(self, code), 0);
    return code - lh_ui_font_get_first(self);
}

lh_usize_t
lh_ui_font_get_cell_bytes(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_usize_t, lh_ui_font_get_height(self))
        * lh_cast_static(lh_usize_t, lh_ui_font_get_row_bytes(self));
}

lh_bool_t
lh_ui_font_has_pixel(const lh_ui_font_t *self, lh_math_coord_t x, lh_math_coord_t y)
{
    lh_assert_runtime_ref(self);
    return x >= 0 && y >= 0 && x < lh_ui_font_get_cell_width(self) && y < lh_ui_font_get_height(self);
}

lh_memory_view_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_uint_t code)
{
    const lh_byte_t *glyphs;
    const lh_byte_t *begin;
    lh_usize_t cell;
    lh_assert_runtime_ref(self);
    cell = lh_ui_font_get_cell_bytes(self);
    lh_return_if(!lh_ui_font_has_code(self, code), lh_memory_view_make_empty());
    glyphs = lh_cast_static(const lh_byte_t *, lh_memory_view_get_begin(lh_addr_of(self->glyphs)));
    begin = glyphs + lh_ui_font_get_index(self, code) * cell;
    return lh_memory_view_make_by_size(lh_cast_static(lh_ptr, begin), cell);
}

lh_byte_t
lh_ui_font_get_advance(const lh_ui_font_t *self, lh_uint_t code)
{
    const lh_byte_t *advances;
    lh_assert_runtime_ref(self);
    lh_return_if(!lh_ui_font_has_code(self, code), 0);
    advances = lh_cast_static(const lh_byte_t *, lh_memory_view_get_begin(lh_addr_of(self->advances)));
    return advances[lh_ui_font_get_index(self, code)];
}

lh_byte_t
lh_ui_font_get_coverage(const lh_ui_font_t *self, lh_uint_t code, lh_math_coord_t x, lh_math_coord_t y)
{
    const lh_byte_t *bytes;
    const lh_uint_t byte_bits = lh_cast_static(lh_uint_t, lh_type_bits(lh_byte_t));
    lh_memory_view_t glyph;
    lh_byte_t bpp;
    lh_uint_t bit_index;
    lh_uint_t shift;
    lh_uint_t mask;
    lh_uint_t value;
    lh_assert_runtime_ref(self);
    glyph = lh_ui_font_get_glyph(self, code);
    bpp = lh_ui_font_get_bpp(self);
    lh_return_if(!lh_ui_font_has_code(self, code), 0);
    lh_return_if(!lh_ui_font_has_pixel(self, x, y), 0);
    bytes = lh_cast_static(const lh_byte_t *, lh_memory_view_get_begin(lh_addr_of(glyph)));
    bit_index = lh_cast_static(lh_uint_t, x) * bpp;
    shift = byte_bits - bpp - bit_index % byte_bits;
    mask = lh_bit_mask(bpp) - 1U;
    value = lh_bit_and(lh_bit_shr(bytes[lh_cast_static(lh_usize_t, y)
                                            * lh_cast_static(lh_usize_t, lh_ui_font_get_row_bytes(self))
                                        + bit_index / byte_bits],
                                  shift),
                       mask);
    return lh_cast_static(lh_byte_t, (value * 255U + mask / 2U) / mask);
}
