/**
 * @file pixmap.c
 * @brief Implementation of `lh/ui/pixmap.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bit/bswap.h>
#include <lh/byte/limits.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/size.h>
#include <lh/ui/pixmap.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Lifetime and shape ──────────────────────────────────────────────────── */

lh_bool_t
lh_ui_pixmap_format_is_16(lh_ui_pixmap_format_t format)
{
    return format == lh_ui_pixmap_format_rgb565 || format == lh_ui_pixmap_format_rgb565_swapped ? lh_bool_true
                                                                                                 : lh_bool_false;
}

lh_s32_t
lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_format_t format)
{
    return lh_ui_pixmap_format_is_16(format) ? 2 : 4;
}

lh_void
lh_ui_pixmap_init(lh_ui_pixmap_t *self, lh_byte_t *bits, lh_s32_t width, lh_s32_t height, lh_s32_t stride,
                  lh_ui_pixmap_format_t format)
{
    const lh_s32_t bytes = lh_ui_pixmap_format_get_bytes(format);

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(width >= 0 && height >= 0 && stride >= width * bytes && stride % bytes == 0,
                          lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(lh_null_ne(bits) || width == 0 || height == 0, lh_runtime_error_code_invalid_argument);
    self->bits = bits;
    self->width = width;
    self->height = height;
    self->stride = stride;
    self->format = format;
}

lh_void
lh_ui_pixmap_init_empty(lh_ui_pixmap_t *self)
{
    lh_ui_pixmap_init(self, lh_null, 0, 0, 0, lh_ui_pixmap_format_argb8888);
}

lh_s32_t
lh_ui_pixmap_get_width(const lh_ui_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_s32_t
lh_ui_pixmap_get_height(const lh_ui_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_ui_pixmap_format_t
lh_ui_pixmap_get_format(const lh_ui_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->format;
}

lh_s32_t
lh_ui_pixmap_get_pixel_bytes(const lh_ui_pixmap_t *self)
{
    return lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_get_format(self));
}

lh_ui_rect_t
lh_ui_pixmap_get_bounds(const lh_ui_pixmap_t *self)
{
    lh_ui_rect_t bounds;

    lh_assert_runtime_ref(self);
    lh_ui_rect_init(lh_addr_of(bounds), 0, 0, self->width, self->height);
    return bounds;
}

lh_byte_t *
lh_ui_pixmap_get_row(const lh_ui_pixmap_t *self, lh_s32_t y)
{
    lh_assert_runtime_ref(self);
    return self->bits + y * self->stride;
}

lh_byte_t *
lh_ui_pixmap_get_address(const lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_ui_pixmap_get_row(self, y) + x * lh_ui_pixmap_get_pixel_bytes(self);
}

/* ── Pixels ──────────────────────────────────────────────────────────────── */

lh_u32_t
lh_ui_pixmap_order_16(const lh_ui_pixmap_t *self, lh_u32_t word)
{
    lh_return_if(lh_ui_pixmap_get_format(self) != lh_ui_pixmap_format_rgb565_swapped, word);
    return lh_bit_bswap_u16(lh_cast_static(lh_u16_t, word));
}

lh_u32_t
lh_ui_pixmap_pack(const lh_ui_pixmap_t *self, const lh_ui_color_t *color)
{
    lh_return_if(!lh_ui_pixmap_format_is_16(lh_ui_pixmap_get_format(self)), lh_ui_color_get_argb(color));
    return lh_ui_pixmap_order_16(self, lh_ui_color_get_rgb565(color));
}

lh_ui_color_t
lh_ui_pixmap_unpack(const lh_ui_pixmap_t *self, lh_u32_t word)
{
    lh_ui_color_t color;

    if (lh_ui_pixmap_format_is_16(lh_ui_pixmap_get_format(self)))
    {
        lh_ui_color_init_rgb565(lh_addr_of(color), lh_ui_pixmap_order_16(self, word));
        return color;
    }
    lh_ui_color_init_argb(lh_addr_of(color), word);
    return color;
}

lh_u32_t
lh_ui_pixmap_read_word(const lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y)
{
    const lh_byte_t *at = lh_ui_pixmap_get_address(self, x, y);

    return lh_ui_pixmap_format_is_16(self->format) ? *lh_ptr_rcast(const lh_u16_t, at)
                                                      : *lh_ptr_rcast(const lh_u32_t, at);
}

lh_void
lh_ui_pixmap_write_word(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, lh_u32_t word)
{
    lh_byte_t *at = lh_ui_pixmap_get_address(self, x, y);

    if (lh_ui_pixmap_format_is_16(self->format))
    {
        *lh_ptr_rcast(lh_u16_t, at) = lh_cast_static(lh_u16_t, word);
        return;
    }
    *lh_ptr_rcast(lh_u32_t, at) = word;
}

lh_ui_color_t
lh_ui_pixmap_get_pixel(const lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_ui_pixmap_unpack(self, lh_ui_pixmap_read_word(self, x, y));
}

lh_void
lh_ui_pixmap_set_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_ui_pixmap_write_word(self, x, y, lh_ui_pixmap_pack(self, color));
}

lh_void
lh_ui_pixmap_blend_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_ui_color_t dst;

    if (lh_ui_color_get_a(color) == LH_BYTE_T_MAX)
    {
        lh_ui_pixmap_set_pixel(self, x, y, color);
        return;
    }
    dst = lh_ui_pixmap_get_pixel(self, x, y);
    dst = lh_ui_color_over(lh_addr_of(dst), color);
    lh_ui_pixmap_set_pixel(self, x, y, lh_addr_of(dst));
}

lh_void
lh_ui_pixmap_cover_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color,
                         lh_byte_t coverage)
{
    lh_ui_color_t edge;

    lh_return_if(coverage == 0U);
    if (coverage == LH_BYTE_T_MAX)
    {
        lh_ui_pixmap_blend_pixel(self, x, y, color);
        return;
    }
    edge = lh_ui_color_with_coverage(color, coverage);
    lh_ui_pixmap_blend_pixel(self, x, y, lh_addr_of(edge));
}

/* ── Spans and boxes ─────────────────────────────────────────────────────── */

lh_void
lh_ui_pixmap_store_words32(lh_u32_t *at, lh_usize_t count, lh_u32_t word)
{
    /* Eight per turn: cheap loop overhead unoptimized, still vectorized at -O2/-O3. */
    for (; count >= 8U; count -= 8U, at += 8)
    {
        at[0] = word, at[1] = word, at[2] = word, at[3] = word;
        at[4] = word, at[5] = word, at[6] = word, at[7] = word;
    }
    for (; count > 0U; --count, ++at)
    {
        *at = word;
    }
}

lh_void
lh_ui_pixmap_store_words16(lh_u16_t *at, lh_usize_t count, lh_u16_t word)
{
    for (; count >= 8U; count -= 8U, at += 8)
    {
        at[0] = word, at[1] = word, at[2] = word, at[3] = word;
        at[4] = word, at[5] = word, at[6] = word, at[7] = word;
    }
    for (; count > 0U; --count, ++at)
    {
        *at = word;
    }
}

lh_void
lh_ui_pixmap_store_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t word)
{
    lh_byte_t *at = lh_ui_pixmap_get_address(self, x0, y);

    lh_return_if(x1 <= x0);
    if (lh_ui_pixmap_format_is_16(self->format))
    {
        lh_ui_pixmap_store_words16(lh_ptr_rcast(lh_u16_t, at), lh_cast_static(lh_usize_t, x1 - x0),
                                   lh_cast_static(lh_u16_t, word));
        return;
    }
    lh_ui_pixmap_store_words32(lh_ptr_rcast(lh_u32_t, at), lh_cast_static(lh_usize_t, x1 - x0), word);
}

/* ── Row kernels ─────────────────────────────────────────────────────────── */

lh_u32_t
lh_ui_pixmap_mix_rgb(lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    return (lh_cast_static(lh_u32_t, lh_ui_color_over_opaque_channel((src >> 16) & 0xFFU, (dst >> 16) & 0xFFU, a))
            << 16) |
           (lh_cast_static(lh_u32_t, lh_ui_color_over_opaque_channel((src >> 8) & 0xFFU, (dst >> 8) & 0xFFU, a)) << 8) |
           lh_cast_static(lh_u32_t, lh_ui_color_over_opaque_channel(src & 0xFFU, dst & 0xFFU, a));
}

lh_u32_t
lh_ui_pixmap_mix_any(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    lh_ui_color_t under = lh_ui_pixmap_unpack(self, dst);
    lh_ui_color_t over;

    lh_ui_color_init_argb(lh_addr_of(over), (a << 24) | src);
    under = lh_ui_color_over(lh_addr_of(under), lh_addr_of(over));
    return lh_ui_pixmap_pack(self, lh_addr_of(under));
}

lh_u32_t
lh_ui_pixmap_mix_argb(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    lh_return_if((dst >> 24) != 0xFFU, lh_ui_pixmap_mix_any(self, dst, src, a));
    return 0xFF000000U | lh_ui_pixmap_mix_rgb(dst, src, a);
}

lh_u32_t
lh_ui_pixmap_mix_16(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    const lh_u32_t word = lh_ui_pixmap_order_16(self, dst);
    const lh_u32_t r5 = (word >> 11) & 0x1FU;
    const lh_u32_t g6 = (word >> 5) & 0x3FU;
    const lh_u32_t b5 = word & 0x1FU;
    lh_u32_t mixed;

    /* The channel widening is lh_ui_color_expand_bits (replicate the top bits
       into the bottom) and the narrowing is lh_ui_color_get_rgb565's truncation,
       applied here directly: the same bytes, without an lh_ui_color_t per pixel.
       This is the RGB565 path, the one an MCU actually runs, so the struct round
       trip through unpack / color_get_argb / color_init_argb / pack is not
       affordable here. */
    mixed = lh_ui_pixmap_mix_rgb(0xFF000000U | (((r5 << 3) | (r5 >> 2)) << 16) |
                                     (((g6 << 2) | (g6 >> 4)) << 8) | ((b5 << 3) | (b5 >> 2)),
                                src, a);
    return lh_ui_pixmap_order_16(self, (((mixed >> 16) & 0xFFU) >> 3) << 11 |
                                     ((((mixed >> 8) & 0xFFU) >> 2) << 5) | ((mixed & 0xFFU) >> 3));
}

lh_void
lh_ui_pixmap_blend_alpha_32(const lh_ui_pixmap_t *self, lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                            lh_u32_t src)
{
    lh_usize_t i;

    for (i = 0U; i < count; ++i)
    {
        if (alpha[i] != 0U)
        {
            at[i] = alpha[i] == 0xFFU ? 0xFF000000U | src : lh_ui_pixmap_mix_argb(self, at[i], src, alpha[i]);
        }
    }
}

lh_void
lh_ui_pixmap_blend_alpha_16(const lh_ui_pixmap_t *self, lh_u16_t *at, const lh_byte_t *alpha, lh_usize_t count,
                            lh_u32_t src)
{
    const lh_u16_t solid = lh_cast_static(lh_u16_t, lh_ui_pixmap_order_16(self, lh_ui_pixmap_pack_rgb(self, src)));
    lh_usize_t i;

    for (i = 0U; i < count; ++i)
    {
        if (alpha[i] != 0U)
        {
            at[i] = alpha[i] == 0xFFU ? solid : lh_cast_static(lh_u16_t, lh_ui_pixmap_mix_16(self, at[i], src, alpha[i]));
        }
    }
}

lh_u32_t
lh_ui_pixmap_pack_rgb(const lh_ui_pixmap_t *self, lh_u32_t src)
{
    lh_ui_color_t color;

    (void)self;
    lh_ui_color_init_argb(lh_addr_of(color), 0xFF000000U | src);
    return lh_ui_color_get_rgb565(lh_addr_of(color));
}

lh_void
lh_ui_pixmap_blend_alpha_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t src,
                              const lh_byte_t *alpha)
{
    lh_byte_t *at = lh_ui_pixmap_get_address(self, x0, y);

    lh_return_if(x1 <= x0);
    if (lh_ui_pixmap_format_is_16(self->format))
    {
        lh_ui_pixmap_blend_alpha_16(self, lh_ptr_rcast(lh_u16_t, at), alpha, lh_cast_static(lh_usize_t, x1 - x0), src);
        return;
    }
    lh_ui_pixmap_blend_alpha_32(self, lh_ptr_rcast(lh_u32_t, at), alpha, lh_cast_static(lh_usize_t, x1 - x0), src);
}

lh_void
lh_ui_pixmap_blend_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_byte_t alpha[LH_UI_PIXMAP_RUN];
    lh_s32_t end;

    lh_memory_set(alpha, sizeof(alpha), lh_ui_color_get_a(color));
    for (; x0 < x1; x0 = end)
    {
        end = lh_math_min(x1, x0 + LH_UI_PIXMAP_RUN);
        lh_ui_pixmap_blend_alpha_span(self, x0, end, y, lh_ui_color_get_argb(color) & 0x00FFFFFFU, alpha);
    }
}

lh_void
lh_ui_pixmap_fill_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    if (lh_ui_color_get_a(color) == LH_BYTE_T_MAX)
    {
        lh_ui_pixmap_store_span(self, x0, x1, y, lh_ui_pixmap_pack(self, color));
        return;
    }
    lh_ui_pixmap_blend_span(self, x0, x1, y, color);
}

lh_void
lh_ui_pixmap_store_box(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1, lh_u32_t word)
{
    lh_s32_t y;

    for (y = y0; y < y1; ++y)
    {
        lh_ui_pixmap_store_span(self, x0, x1, y, word);
    }
}

lh_void
lh_ui_pixmap_blend_box(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                       const lh_ui_color_t *color)
{
    for (; y0 < y1; ++y0)
    {
        lh_ui_pixmap_blend_span(self, x0, x1, y0, color);
    }
}

lh_void
lh_ui_pixmap_fill_box(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                      const lh_ui_color_t *color)
{
    if (lh_ui_color_get_a(color) == LH_BYTE_T_MAX)
    {
        lh_ui_pixmap_store_box(self, x0, y0, x1, y1, lh_ui_pixmap_pack(self, color));
        return;
    }
    lh_ui_pixmap_blend_box(self, x0, y0, x1, y1, color);
}

lh_void
lh_ui_pixmap_clear(lh_ui_pixmap_t *self, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_ui_pixmap_store_box(self, 0, 0, self->width, self->height, lh_ui_pixmap_pack(self, color));
}
