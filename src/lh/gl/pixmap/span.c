/**
 * @file span.c
 * @brief Implementation of `lh/gl/pixmap.h`: spans and boxes: runs of pixels stored, filled and
 * blended, and the kernels.
 */

#include <lh/gl/pixmap.h>

#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/byte/limits.h>
#include <lh/core/cpu/simd.h>
#include <lh/core/memory.h>
#include <lh/core/pixel/span.h>
#include <lh/core/util/return.h>

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

lh_void
lh_gl_pixmap_blend_alpha_16(const lh_gl_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha,
                            lh_usize_t count, lh_u32_t src)
{
    lh_pixel_span_blend_alphas_rgb565(
        at, alpha, count, src, lh_gl_pixmap_get_format(self) == lh_gl_pixmap_format_rgb565_swapped);
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
    lh_return_if(x1 <= x0 || y1 <= y0);
    /* The address, the format and the length are the same for every row: worked out
       once, and each row is one stride further, instead of a store_span per row that
       finds them again. */
    const lh_usize_t count = lh_cast_static(lh_usize_t, x1 - x0);
    const lh_s32_t stride = lh_gl_pixmap_get_stride(self);
    lh_byte_ptr row = lh_gl_pixmap_get_address(self, x0, y0);

    if (lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
    {
        for (lh_s32_t y = y0; y < y1; ++y)
        {
            lh_memory_set_u16(lh_ptr_rcast(lh_u16_t, row), count, lh_cast_static(lh_u16_t, word));
            row += stride;
        }
        return;
    }
    for (lh_s32_t y = y0; y < y1; ++y)
    {
        lh_memory_set_u32(lh_ptr_rcast(lh_u32_t, row), count, word);
        row += stride;
    }
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
lh_gl_pixmap_clear(lh_gl_pixmap_t *self, const lh_gl_color_t *color)
{
    lh_gl_pixmap_store_box(self, 0, 0, lh_gl_pixmap_get_width(self), lh_gl_pixmap_get_height(self),
                           lh_gl_pixmap_pack(self, color));
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
