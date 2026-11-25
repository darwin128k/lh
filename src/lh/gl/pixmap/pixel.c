/**
 * @file pixel.c
 * @brief Implementation of `lh/gl/pixmap.h`: one pixel: packing, reading and writing a word,
 * getting and setting it.
 */

#include <lh/gl/pixmap.h>

#include <lh/core/assert.h>
#include <lh/core/bit/bswap.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>

lh_u32_t
lh_gl_pixmap_order_16(const lh_gl_pixmap_t *self, lh_u32_t word)
{
    lh_return_if(lh_gl_pixmap_get_format(self) != lh_gl_pixmap_format_rgb565_swapped, word);
    return lh_bit_bswap_u16(lh_cast_static(lh_u16_t, word));
}

lh_u32_t
lh_gl_pixmap_read_word(const lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_byte_cptr at = lh_gl_pixmap_get_address(self, x, y);

    return lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self))
               ? *lh_ptr_rcast(const lh_u16_t, at)
               : *lh_ptr_rcast(const lh_u32_t, at);
}

lh_void
lh_gl_pixmap_write_word(lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y, lh_u32_t word)
{
    lh_byte_ptr at = lh_gl_pixmap_get_address(self, x, y);

    if (lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
    {
        lh_ptr_deref(lh_ptr_rcast(lh_u16_t, at)) = lh_cast_static(lh_u16_t, word);
        return;
    }
    lh_ptr_deref(lh_ptr_rcast(lh_u32_t, at)) = word;
}

lh_u32_t
lh_gl_pixmap_pack_rgb(const lh_gl_pixmap_t *self, lh_u32_t src)
{
    LH_ATTRIBUTE_UNUSED(self);
    lh_gl_color_t color;
    lh_gl_color_init_argb(lh_addr_of(color), lh_bit_or(0xFF000000U, src));
    return lh_gl_color_get_rgb565(lh_addr_of(color));
}

lh_u32_t
lh_gl_pixmap_pack(const lh_gl_pixmap_t *self, const lh_gl_color_t *color)
{
    lh_return_if(!lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)),
                 lh_gl_color_get_argb(color));
    return lh_gl_pixmap_order_16(self, lh_gl_color_get_rgb565(color));
}

lh_void
lh_gl_pixmap_unpack(const lh_gl_pixmap_t *self, lh_u32_t word, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_color_t color;

    if (lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
    {
        lh_gl_color_init_rgb565(lh_addr_of(color), lh_gl_pixmap_order_16(self, word));
        lh_ptr_deref(out) = color;
        return;
    }
    lh_gl_color_init_argb(lh_addr_of(color), word);
    lh_ptr_deref(out) = color;
}

lh_void
lh_gl_pixmap_get_pixel(const lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y, lh_gl_color_t *out)
{
    lh_gl_pixmap_unpack(self, lh_gl_pixmap_read_word(self, x, y), out);
}

lh_void
lh_gl_pixmap_set_pixel(lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_gl_color_t *color)
{
    lh_gl_pixmap_write_word(self, x, y, lh_gl_pixmap_pack(self, color));
}
