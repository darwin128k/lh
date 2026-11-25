/**
 * @file mix.c
 * @brief Implementation of `lh/gl/pixmap.h`: mixing colours: a word over a word, in every format.
 */

#include <lh/gl/pixmap.h>

#include <lh/core/attribute/unused.h>
#include <lh/core/pixel/rgb565.h>
#include <lh/core/util/return.h>

lh_u32_t
lh_gl_pixmap_mix_rgb(lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    return lh_gl_color_argb_over_opaque(dst, (a << 24) | src) & 0x00FFFFFFU;
}

lh_u32_t
lh_gl_pixmap_mix_any(const lh_gl_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    lh_gl_color_t under;
    lh_gl_pixmap_unpack(self, dst, lh_addr_of(under));
    lh_gl_color_t over;

    lh_gl_color_init_argb(lh_addr_of(over), (a << 24) | src);
    lh_gl_color_over(lh_addr_of(under), lh_addr_of(over), lh_addr_of(under));
    return lh_gl_pixmap_pack(self, lh_addr_of(under));
}

lh_u32_t
lh_gl_pixmap_mix_argb(const lh_gl_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    LH_ATTRIBUTE_UNUSED(self);
    return lh_gl_color_argb_over(dst, (a << 24) | src);
}

lh_u32_t
lh_gl_pixmap_rb_of_565(lh_u32_t word)
{
    return lh_pixel_rgb565_get_rb(word);
}

lh_u32_t
lh_gl_pixmap_g_of_565(lh_u32_t word)
{
    return lh_pixel_rgb565_get_g(word);
}

lh_u32_t
lh_gl_pixmap_565_of(lh_u32_t rb, lh_u32_t g)
{
    return lh_pixel_rgb565_make(rb, g);
}

lh_u32_t
lh_gl_pixmap_mix_16(const lh_gl_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    /* The core's word mix (::lh_pixel_rgb565_mix): the 888 blend of the widened word,
       bit for bit, red and blue through one multiply pair, green through the other. */
    return lh_gl_pixmap_order_16(
        self, lh_pixel_rgb565_mix(lh_gl_pixmap_order_16(self, dst), src & 0x00FFFFFFU, a));
}

lh_u32_t
lh_gl_pixmap_over_argb(const lh_gl_pixmap_t *self, lh_u32_t dst, const lh_gl_color_t *color)
{
    const lh_u32_t argb = lh_gl_color_get_argb(color);
    const lh_u32_t a = argb >> 24;

    lh_return_if(a == LH_BYTE_T_MAX, argb);
    lh_return_if(a == 0U, dst);
    return lh_gl_pixmap_mix_argb(self, dst, argb & 0x00FFFFFFU, a);
}

lh_u32_t
lh_gl_pixmap_mix_word(const lh_gl_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    return lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self))
               ? lh_gl_pixmap_mix_16(self, dst, src, a)
               : lh_gl_pixmap_mix_argb(self, dst, src, a);
}

lh_u32_t
lh_gl_pixmap_over_16(const lh_gl_pixmap_t *self, lh_u32_t dst, const lh_gl_color_t *color)
{
    const lh_u32_t a = lh_gl_color_get_a(color);

    lh_return_if(a == LH_BYTE_T_MAX, lh_gl_pixmap_order_16(self, lh_gl_color_get_rgb565(color)));
    lh_return_if(a == 0U, dst);
    return lh_gl_pixmap_mix_16(self, dst, lh_gl_color_get_argb(color) & 0x00FFFFFFU, a);
}

lh_u32_t
lh_gl_pixmap_over_word(const lh_gl_pixmap_t *self, lh_u32_t dst, const lh_gl_color_t *color)
{
    return lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self))
               ? lh_gl_pixmap_over_16(self, dst, color)
               : lh_gl_pixmap_over_argb(self, dst, color);
}

lh_void
lh_gl_pixmap_blend_pixel(lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_gl_color_t *color)
{
    lh_gl_pixmap_write_word(
        self, x, y, lh_gl_pixmap_over_word(self, lh_gl_pixmap_read_word(self, x, y), color));
}

lh_void
lh_gl_pixmap_cover_pixel(lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_gl_color_t *color,
                         lh_byte_t coverage)
{
    lh_return_if(coverage == 0U);
    if (coverage == LH_BYTE_T_MAX)
    {
        lh_gl_pixmap_blend_pixel(self, x, y, color);
        return;
    }
    lh_gl_color_t edge;
    lh_gl_color_with_coverage(color, coverage, lh_addr_of(edge));
    lh_gl_pixmap_blend_pixel(self, x, y, lh_addr_of(edge));
}
