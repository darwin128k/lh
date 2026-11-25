/**
 * @file pixmap.c
 * @brief Implementation of `lh/gl/pixmap.h`.
 */
#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>

#include <lh/core/cpu/simd.h>
#include <lh/core/memory.h>
#include <lh/core/numeric/float.h>
#include <lh/gl/pixmap.h>
#include <lh/core/pixel/rgb565.h>
#include <lh/core/pixel/span.h>

/* -- Lifetime and shape ---------------------------------------------------- */

lh_bool_t
lh_gl_pixmap_format_is_16(lh_gl_pixmap_format_t format)
{
    return format == lh_gl_pixmap_format_rgb565 || format == lh_gl_pixmap_format_rgb565_swapped
               ? lh_bool_true
               : lh_bool_false;
}

lh_s32_t
lh_gl_pixmap_format_get_bytes(lh_gl_pixmap_format_t format)
{
    return lh_gl_pixmap_format_is_16(format) ? 2 : 4;
}

lh_s32_t
lh_gl_pixmap_format_get_bits(lh_gl_pixmap_format_t format)
{
    return lh_math_mul(lh_gl_pixmap_format_get_bytes(format), 8);
}

lh_void
lh_gl_pixmap_init(lh_gl_pixmap_t *self, lh_byte_ptr bits, lh_s32_t width, lh_s32_t height,
                  lh_s32_t stride, lh_gl_pixmap_format_t format)
{
    const lh_s32_t bytes = lh_gl_pixmap_format_get_bytes(format);

    lh_assert_runtime_ifn(width >= 0 && height >= 0 && stride >= width * bytes &&
                              lh_math_is_multiple_of(stride, bytes),
                          lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(lh_null_ne(bits) || width == 0 || height == 0,
                          lh_runtime_error_code_invalid_argument);
    lh_gl_pixmap_set_bits(self, bits);
    lh_gl_pixmap_set_width(self, width);
    lh_gl_pixmap_set_height(self, height);
    lh_gl_pixmap_set_stride(self, stride);
    lh_gl_pixmap_set_format(self, format);
}

lh_void
lh_gl_pixmap_init_empty(lh_gl_pixmap_t *self)
{
    lh_gl_pixmap_init(self, lh_null, 0, 0, 0, lh_gl_pixmap_format_argb8888);
}

lh_s32_t
lh_gl_pixmap_get_width(const lh_gl_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_s32_t
lh_gl_pixmap_get_height(const lh_gl_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_gl_pixmap_format_t
lh_gl_pixmap_get_format(const lh_gl_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->format;
}

lh_s32_t
lh_gl_pixmap_get_pixel_bytes(const lh_gl_pixmap_t *self)
{
    return lh_gl_pixmap_format_get_bytes(lh_gl_pixmap_get_format(self));
}

lh_void
lh_gl_pixmap_get_bounds(const lh_gl_pixmap_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), 0, 0, lh_gl_pixmap_get_width(self),
                    lh_gl_pixmap_get_height(self));
    lh_ptr_deref(out) = bounds;
}

lh_byte_ptr
lh_gl_pixmap_get_row(const lh_gl_pixmap_t *self, lh_s32_t y)
{
    return lh_gl_pixmap_get_bits(self) + y * lh_gl_pixmap_get_stride(self);
}

lh_byte_ptr
lh_gl_pixmap_get_address(const lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_gl_pixmap_get_row(self, y) + x * lh_gl_pixmap_get_pixel_bytes(self);
}

/* -- Pixels ---------------------------------------------------------------- */

lh_u32_t
lh_gl_pixmap_order_16(const lh_gl_pixmap_t *self, lh_u32_t word)
{
    lh_return_if(lh_gl_pixmap_get_format(self) != lh_gl_pixmap_format_rgb565_swapped, word);
    return lh_bit_bswap_u16(lh_cast_static(lh_u16_t, word));
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

/* -- Spans and boxes ------------------------------------------------------- */

lh_void
lh_gl_pixmap_store_span(lh_gl_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t word)
{
    lh_byte_ptr at = lh_gl_pixmap_get_address(self, x0, y);

    lh_return_if(x1 <= x0);
    if (lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
    {
        lh_memory_set_u16(lh_ptr_rcast(lh_u16_t, at), lh_cast_static(lh_usize_t, x1 - x0),
                          lh_cast_static(lh_u16_t, word));
        return;
    }
    lh_memory_set_u32(lh_ptr_rcast(lh_u32_t, at), lh_cast_static(lh_usize_t, x1 - x0), word);
}

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

/* -- Row kernels ----------------------------------------------------------- */

lh_u32_t
lh_gl_pixmap_mix_argb(const lh_gl_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    LH_ATTRIBUTE_UNUSED(self);
    return lh_gl_color_argb_over(dst, (a << 24) | src);
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

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
lh_void
lh_gl_pixmap_blend_cover_32_sse2(lh_u32_t *at, lh_usize_t count, lh_u64_t cover, lh_u32_t src)
{
    lh_assert_runtime(count >= 1U && count <= 8U, lh_runtime_error_code_invalid_argument);
    lh_pixel_span_blend_alphas8_sse2_argb(at, count, cover, src);
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

lh_void
lh_gl_pixmap_blend_const_32(const lh_gl_pixmap_t *self, lh_u32_t *at, lh_usize_t count,
                            lh_u32_t src, lh_u32_t alpha)
{
    LH_ATTRIBUTE_UNUSED(self);
    lh_pixel_span_blend_argb(at, count, src, alpha);
}

lh_u32_t
lh_gl_pixmap_pack_rgb(const lh_gl_pixmap_t *self, lh_u32_t src)
{
    LH_ATTRIBUTE_UNUSED(self);
    lh_gl_color_t color;
    lh_gl_color_init_argb(lh_addr_of(color), lh_bit_or(0xFF000000U, src));
    return lh_gl_color_get_rgb565(lh_addr_of(color));
}

lh_void
lh_gl_pixmap_blend_alpha_16(const lh_gl_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha,
                            lh_usize_t count, lh_u32_t src)
{
    lh_pixel_span_blend_alphas_rgb565(
        at, alpha, count, src, lh_gl_pixmap_get_format(self) == lh_gl_pixmap_format_rgb565_swapped);
}

lh_void
lh_gl_pixmap_blend_alpha_span(lh_gl_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                              lh_u32_t src, lh_byte_cptr alpha)
{
    lh_byte_ptr at = lh_gl_pixmap_get_address(self, x0, y);

    lh_return_if(x1 <= x0);
    if (lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
    {
        lh_gl_pixmap_blend_alpha_16(self, lh_ptr_rcast(lh_u16_t, at), alpha,
                                    lh_cast_static(lh_usize_t, x1 - x0), src);
        return;
    }
    lh_pixel_span_blend_alphas_argb(lh_ptr_rcast(lh_u32_t, at), alpha,
                                    lh_cast_static(lh_usize_t, x1 - x0), src);
}

lh_void
lh_gl_pixmap_blend_cover_8(lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y, lh_s32_t count,
                           lh_u64_t cover, lh_u32_t src)
{
    lh_assert_runtime(count >= 1 && count <= 8, lh_runtime_error_code_invalid_argument);
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (lh_cpu_simd_has_sse2())
    {
        lh_byte_ptr at = lh_gl_pixmap_get_address(self, x, y);

        if (lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
        {
            lh_pixel_span_blend_alphas8_sse2_rgb565(
                lh_ptr_rcast(lh_u16_t, at), lh_cast_static(lh_usize_t, count), cover, src,
                lh_gl_pixmap_get_format(self) == lh_gl_pixmap_format_rgb565_swapped);
            return;
        }
        lh_gl_pixmap_blend_cover_32_sse2(lh_ptr_rcast(lh_u32_t, at),
                                         lh_cast_static(lh_usize_t, count), cover, src);
        return;
    }
#endif
    lh_byte_t alpha[8];
    for (lh_s32_t i = 0; i < count; ++i)
    {
        alpha[i] = lh_cast_static(lh_byte_t, cover >> (8 * i));
    }
    lh_gl_pixmap_blend_alpha_span(self, x, x + count, y, src, alpha);
}

lh_void
lh_gl_pixmap_blend_span(lh_gl_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                        const lh_gl_color_t *color)
{
    const lh_u32_t coverage = lh_gl_color_get_a(color);

    lh_return_if(x1 <= x0 || coverage == 0U);
    if (!lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
    {
        lh_gl_pixmap_blend_const_32(self,
                                    lh_ptr_rcast(lh_u32_t, lh_gl_pixmap_get_address(self, x0, y)),
                                    lh_cast_static(lh_usize_t, x1 - x0),
                                    lh_gl_color_get_argb(color) & 0x00FFFFFFU, coverage);
        return;
    }
    lh_byte_t alpha[LH_GL_PIXMAP_RUN];

    lh_memory_set(alpha, sizeof(alpha), lh_cast_static(lh_byte_t, coverage));
    while (x0 < x1)
    {
        lh_s32_t end = lh_math_min(x1, x0 + LH_GL_PIXMAP_RUN);

        lh_gl_pixmap_blend_alpha_span(self, x0, end, y, lh_gl_color_get_argb(color) & 0x00FFFFFFU,
                                      alpha);
        x0 = end;
    }
}

lh_void
lh_gl_pixmap_fill_span(lh_gl_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                       const lh_gl_color_t *color)
{
    if (lh_gl_color_get_a(color) == LH_BYTE_T_MAX)
    {
        lh_gl_pixmap_store_span(self, x0, x1, y, lh_gl_pixmap_pack(self, color));
        return;
    }
    lh_gl_pixmap_blend_span(self, x0, x1, y, color);
}

lh_void
lh_gl_pixmap_fill_colors_span(lh_gl_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                              const lh_gl_color_t *colors)
{
    const lh_usize_t count = x1 > x0 ? lh_cast_static(lh_usize_t, x1 - x0) : 0U;
    lh_byte_ptr row = lh_gl_pixmap_get_address(self, x0, y);

    if (lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
    {
        lh_u16_t *at = lh_ptr_rcast(lh_u16_t, row);

        for (lh_usize_t i = 0U; i < count; ++i)
        {
            at[i] = lh_cast_static(lh_u16_t, lh_gl_pixmap_over_16(self, at[i], colors + i));
        }
        return;
    }
    {
        lh_u32_t *at = lh_ptr_rcast(lh_u32_t, row);

        for (lh_usize_t i = 0U; i < count; ++i)
        {
            at[i] = lh_gl_pixmap_over_argb(self, at[i], colors + i);
        }
    }
}

lh_void
lh_gl_pixmap_store_box(lh_gl_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                       lh_u32_t word)
{
    for (lh_s32_t y = y0; y < y1; ++y)
    {
        lh_gl_pixmap_store_span(self, x0, x1, y, word);
    }
}

lh_void
lh_gl_pixmap_blend_box(lh_gl_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                       const lh_gl_color_t *color)
{
    for (; y0 < y1; ++y0)
    {
        lh_gl_pixmap_blend_span(self, x0, x1, y0, color);
    }
}

lh_void
lh_gl_pixmap_fill_box(lh_gl_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                      const lh_gl_color_t *color)
{
    if (lh_gl_color_get_a(color) == LH_BYTE_T_MAX)
    {
        lh_gl_pixmap_store_box(self, x0, y0, x1, y1, lh_gl_pixmap_pack(self, color));
        return;
    }
    lh_gl_pixmap_blend_box(self, x0, y0, x1, y1, color);
}

lh_void
lh_gl_pixmap_clear(lh_gl_pixmap_t *self, const lh_gl_color_t *color)
{
    lh_gl_pixmap_store_box(self, 0, 0, lh_gl_pixmap_get_width(self), lh_gl_pixmap_get_height(self),
                           lh_gl_pixmap_pack(self, color));
}

lh_byte_t *
lh_gl_pixmap_get_bits(const lh_gl_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bits;
}

lh_void
lh_gl_pixmap_set_bits(lh_gl_pixmap_t *self, lh_byte_t *bits)
{
    lh_assert_runtime_ref(self);
    self->bits = bits;
}

lh_void
lh_gl_pixmap_set_width(lh_gl_pixmap_t *self, lh_s32_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_gl_pixmap_set_height(lh_gl_pixmap_t *self, lh_s32_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_s32_t
lh_gl_pixmap_get_stride(const lh_gl_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stride;
}

lh_void
lh_gl_pixmap_set_stride(lh_gl_pixmap_t *self, lh_s32_t stride)
{
    lh_assert_runtime_ref(self);
    self->stride = stride;
}

lh_void
lh_gl_pixmap_set_format(lh_gl_pixmap_t *self, lh_gl_pixmap_format_t format)
{
    lh_assert_runtime_ref(self);
    self->format = format;
}
