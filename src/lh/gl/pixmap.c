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

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
#    include <immintrin.h>
#    if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif
#endif

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
    const lh_u32_t word = lh_gl_pixmap_order_16(self, dst);

    /* The 888 blend of the widened word, bit for bit, without unpacking a channel
       at a time: red and blue go through one multiply pair (0x00FF00FF lanes),
       green through the other. This is the RGB565 path an MCU runs. */
    return lh_gl_pixmap_order_16(
        self, lh_gl_pixmap_565_of(
                  lh_gl_color_mix_pair(lh_gl_pixmap_rb_of_565(word), src & 0x00FF00FFU, a),
                  lh_gl_color_mix_pair(lh_gl_pixmap_g_of_565(word), (src >> 8) & 0xFFU, a)));
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

lh_u32_t
lh_gl_pixmap_blend_16_one(const lh_gl_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t solid,
                          lh_u32_t a)
{
    lh_return_if(a == 0U, dst);
    lh_return_if(a == LH_BYTE_T_MAX, solid);
    return lh_gl_pixmap_mix_16(self, dst, src, a);
}

lh_u32_t
lh_gl_pixmap_blend_16_pair(const lh_gl_pixmap_t *self, lh_u32_t pair, lh_u32_t src, lh_u32_t solid,
                           lh_u32_t a_lo, lh_u32_t a_hi)
{
    return lh_gl_pixmap_blend_16_one(self, pair & 0xFFFFU, src, solid, a_lo) |
           (lh_gl_pixmap_blend_16_one(self, pair >> 16, src, solid, a_hi) << 16);
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

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
/* The first @p count alphas at @p alpha in the low bytes of a register, 0 past them.
   The 32-bit kernels moved to the core (lh/core/pixel/span.h) with their own copy;
   this one stays for the 16-bit kernels until they move too (docs/cleanup.md). */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_gl_pixmap_gather_alpha(lh_byte_cptr alpha, lh_usize_t count)
{
    lh_u64_t bytes = 0U;

    for (lh_usize_t k = count; k > 0U; --k)
    {
        bytes = (bytes << 8) | alpha[k - 1U];
    }
    return _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(bytes)));
}
#endif

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
/* (x + 127) / 255, in 16-bit lanes, for x <= 65535 — the same value the scalar
   kernel divides its way to, with no divide at all. 255 is odd, so x / 255 never
   lands exactly on .5 and this round-to-nearest cannot disagree with it. */
#    define LH_GL_PIXMAP_DIV255(x)                                                                 \
        _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(x, _mm_srli_epi16(x, 8)), one), 8)

/* One channel plane of eight pixels: widen the stored bits to a byte, blend,
   narrow back, and leave the result in that plane's own register. The three
   channels sit at fixed bit positions inside each 16-bit word, so each plane can
   be lifted out with a shift and a mask and put back the same way — there is no
   pixel-to-lane shuffle anywhere in this kernel, and pixel k of the alpha array
   is lane k of all three planes, which is what keeps them lined up.

   The widening is spelled out as the shift pair rather than folded into a
   multiply. (r5 * 33) >> 2 does happen to equal (r5 << 3) | (r5 >> 2), because
   the two halves cannot overlap — but the same trick on green does NOT hold:
   (g6 * 17) >> 2 spreads g6/4 into the low bits where the scalar spreads g6/16,
   so every green value from 16 up came out a shade too light. Same two shifts,
   no cleverness, exactly the value the scalar kernel computes. */
#    define LH_GL_PIXMAP_BLEND_PLANE(out8, bits, src8, up, down, narrow)                           \
        do                                                                                         \
        {                                                                                          \
            const __m128i dst8 =                                                                   \
                _mm_or_si128(_mm_slli_epi16(bits, up), _mm_srli_epi16(bits, down));                \
            const __m128i t = _mm_add_epi16(                                                       \
                _mm_add_epi16(_mm_mullo_epi16(src8, alpha16), _mm_mullo_epi16(dst8, inv)), round); \
            out8 = _mm_srli_epi16(LH_GL_PIXMAP_DIV255(t), narrow);                                 \
        } while (0)

/* Eight RGB565 words blended in registers, pixel k at byte k of @p a8. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_gl_pixmap_blend_octet_m128(__m128i in, __m128i a8, lh_u32_t src, lh_bool_t swap)
{
    const __m128i zero = _mm_setzero_si128();
    const __m128i full = _mm_set1_epi16(255);
    const __m128i round = _mm_set1_epi16(127);
    const __m128i one = _mm_set1_epi16(1);
    const __m128i m31 = _mm_set1_epi16(0x1F);
    const __m128i m63 = _mm_set1_epi16(0x3F);
    /* The source as three planes of eight 8-bit values, each widened to lanes. */
    const __m128i src8r = _mm_set1_epi16(lh_cast_static(short, ((src >> 16) & 0xFF)));
    const __m128i src8g = _mm_set1_epi16(lh_cast_static(short, ((src >> 8) & 0xFF)));
    const __m128i src8b = _mm_set1_epi16(lh_cast_static(short, (src & 0xFF)));
    const __m128i alpha16 = _mm_unpacklo_epi8(a8, zero);
    const __m128i inv = _mm_sub_epi16(full, alpha16);

    /* A swapped format keeps the low byte first; one 16-bit byte swap per block
       turns it into the numeric RGB565 the planes below expect, and the same
       swap on the way out puts it back. */
    if (swap)
    {
        in = _mm_or_si128(_mm_slli_epi16(in, 8), _mm_srli_epi16(in, 8));
    }
    __m128i r8;
    LH_GL_PIXMAP_BLEND_PLANE(r8, _mm_and_si128(_mm_srli_epi16(in, 11), m31), src8r, 3, 2, 3);
    __m128i g8;
    LH_GL_PIXMAP_BLEND_PLANE(g8, _mm_and_si128(_mm_srli_epi16(in, 5), m63), src8g, 2, 4, 2);
    __m128i b8;
    LH_GL_PIXMAP_BLEND_PLANE(b8, _mm_and_si128(in, m31), src8b, 3, 2, 3);
    in = _mm_or_si128(_mm_slli_epi16(r8, 11), _mm_or_si128(_mm_slli_epi16(g8, 5), b8));
    if (swap)
    {
        in = _mm_or_si128(_mm_slli_epi16(in, 8), _mm_srli_epi16(in, 8));
    }
    return in;
}

/* The first @p count (1 .. 7) words at @p at in the low lanes, 0 past them -- a
   lane the blend never writes back. pinsrw takes its lane as an immediate, so the
   lanes are spelled out. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_gl_pixmap_gather_16(const lh_u16_t *at, lh_usize_t count)
{
    __m128i v = _mm_cvtsi32_si128(at[0]);

    if (count > 1U)
    {
        v = _mm_insert_epi16(v, at[1], 1);
    }
    if (count > 2U)
    {
        v = _mm_insert_epi16(v, at[2], 2);
    }
    if (count > 3U)
    {
        v = _mm_insert_epi16(v, at[3], 3);
    }
    if (count > 4U)
    {
        v = _mm_insert_epi16(v, at[4], 4);
    }
    if (count > 5U)
    {
        v = _mm_insert_epi16(v, at[5], 5);
    }
    if (count > 6U)
    {
        v = _mm_insert_epi16(v, at[6], 6);
    }
    return v;
}

/* The low @p count (1 .. 7) lanes of @p v back to @p at; the rest are not written. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE lh_void
lh_gl_pixmap_scatter_16(lh_u16_t *at, lh_usize_t count, __m128i v)
{
    at[0] = lh_cast_static(lh_u16_t, _mm_extract_epi16(v, 0));
    if (count > 1U)
    {
        at[1] = lh_cast_static(lh_u16_t, _mm_extract_epi16(v, 1));
    }
    if (count > 2U)
    {
        at[2] = lh_cast_static(lh_u16_t, _mm_extract_epi16(v, 2));
    }
    if (count > 3U)
    {
        at[3] = lh_cast_static(lh_u16_t, _mm_extract_epi16(v, 3));
    }
    if (count > 4U)
    {
        at[4] = lh_cast_static(lh_u16_t, _mm_extract_epi16(v, 4));
    }
    if (count > 5U)
    {
        at[5] = lh_cast_static(lh_u16_t, _mm_extract_epi16(v, 5));
    }
    if (count > 6U)
    {
        at[6] = lh_cast_static(lh_u16_t, _mm_extract_epi16(v, 6));
    }
}

lh_void
lh_gl_pixmap_blend_alpha_16_octet_sse2(lh_u16_t *at, lh_byte_cptr alpha, lh_u32_t src,
                                       lh_bool_t swap)
{
    _mm_storeu_si128(lh_ptr_rcast(__m128i, at),
                     lh_gl_pixmap_blend_octet_m128(
                         _mm_loadu_si128(lh_ptr_rcast(const __m128i, at)),
                         _mm_loadl_epi64(lh_ptr_rcast(const __m128i, alpha)), src, swap));
}

lh_void
lh_gl_pixmap_blend_alpha_16_sse2(const lh_gl_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha,
                                 lh_usize_t count, lh_u32_t src)
{
    const lh_bool_t swap = lh_gl_pixmap_get_format(self) == lh_gl_pixmap_format_rgb565_swapped;
    lh_usize_t i;

    for (i = 0U; i + 8U <= count; i += 8U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, at + i),
                         lh_gl_pixmap_blend_octet_m128(
                             _mm_loadu_si128(lh_ptr_rcast(const __m128i, at + i)),
                             _mm_loadl_epi64(lh_ptr_rcast(const __m128i, alpha + i)), src, swap));
    }
    lh_return_if(i == count);
    /* The tail is a block too, as in ::lh_pixel_span_blend_alphas_sse2_argb. */
    lh_gl_pixmap_scatter_16(
        at + i, count - i,
        lh_gl_pixmap_blend_octet_m128(lh_gl_pixmap_gather_16(at + i, count - i),
                                      lh_gl_pixmap_gather_alpha(alpha + i, count - i), src, swap));
}

lh_void
lh_gl_pixmap_blend_cover_16_sse2(lh_u16_t *at, lh_usize_t count, lh_u64_t cover, lh_u32_t src,
                                 lh_bool_t swap)
{
    const __m128i a8 = _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(cover)));

    lh_assert_runtime(count >= 1U && count <= 8U, lh_runtime_error_code_invalid_argument);
    if (count == 8U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, at),
                         lh_gl_pixmap_blend_octet_m128(
                             _mm_loadu_si128(lh_ptr_rcast(const __m128i, at)), a8, src, swap));
        return;
    }
    lh_gl_pixmap_scatter_16(
        at, count, lh_gl_pixmap_blend_octet_m128(lh_gl_pixmap_gather_16(at, count), a8, src, swap));
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

lh_void
lh_gl_pixmap_blend_16_pair_at(const lh_gl_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha,
                              lh_u32_t src, lh_u32_t solid)
{
    /* Each pixel through its own 16-bit access (a 32-bit lvalue on a 16-bit buffer
       is an aliasing violation; the compiler may merge the two). Joining them into
       one word only for lh_gl_pixmap_blend_16_pair to split it again bought nothing. */
    lh_return_if(alpha[0] == 0U && alpha[1] == 0U);
    at[0] = lh_cast_static(lh_u16_t, lh_gl_pixmap_blend_16_one(self, at[0], src, solid, alpha[0]));
    at[1] = lh_cast_static(lh_u16_t, lh_gl_pixmap_blend_16_one(self, at[1], src, solid, alpha[1]));
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
lh_gl_pixmap_blend_alpha_16_words(const lh_gl_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha,
                                  lh_usize_t count, lh_u32_t src)
{
    const lh_u32_t solid = lh_gl_pixmap_order_16(self, lh_gl_pixmap_pack_rgb(self, src));
    lh_usize_t i = 0U;

    /* Two pixels a 32-bit load and store once @p at is word-aligned; the pixel
       before that and an odd last one go alone. */
    if (count > 0U && (lh_ptr_to_uaddr(at) & 2U) != 0U)
    {
        at[0] =
            lh_cast_static(lh_u16_t, lh_gl_pixmap_blend_16_one(self, at[0], src, solid, alpha[0]));
        i = 1U;
    }
    for (; i + 2U <= count; i += 2U)
    {
        lh_gl_pixmap_blend_16_pair_at(self, at + i, alpha + i, src, solid);
    }
    if (i < count)
    {
        at[i] =
            lh_cast_static(lh_u16_t, lh_gl_pixmap_blend_16_one(self, at[i], src, solid, alpha[i]));
    }
}

lh_void
lh_gl_pixmap_blend_alpha_16(const lh_gl_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha,
                            lh_usize_t count, lh_u32_t src)
{
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (lh_cpu_simd_has_sse2())
    {
        lh_gl_pixmap_blend_alpha_16_sse2(self, at, alpha, count, src);
        return;
    }
#endif
    lh_gl_pixmap_blend_alpha_16_words(self, at, alpha, count, src);
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
lh_gl_pixmap_blend_cover_16_words(const lh_gl_pixmap_t *self, lh_u16_t *at, lh_usize_t count,
                                  lh_u64_t cover, lh_u32_t src)
{
    const lh_u32_t solid = lh_gl_pixmap_order_16(self, lh_gl_pixmap_pack_rgb(self, src));
    /* Two 32-bit halves keep a 32-bit core off 64-bit shifts. */
    const lh_u32_t half[2] = {lh_cast_static(lh_u32_t, cover),
                              lh_cast_static(lh_u32_t, cover >> 32)};
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = (half[i >> 2] >> (8U * (i & 3U))) & 0xFFU;
        at[i] = lh_cast_static(lh_u16_t, lh_gl_pixmap_blend_16_one(self, at[i], src, solid, a));
    }
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
            lh_gl_pixmap_blend_cover_16_sse2(
                lh_ptr_rcast(lh_u16_t, at), lh_cast_static(lh_usize_t, count), cover, src,
                lh_gl_pixmap_get_format(self) == lh_gl_pixmap_format_rgb565_swapped);
            return;
        }
        lh_gl_pixmap_blend_cover_32_sse2(lh_ptr_rcast(lh_u32_t, at),
                                         lh_cast_static(lh_usize_t, count), cover, src);
        return;
    }
#endif
    if (lh_gl_pixmap_format_is_16(lh_gl_pixmap_get_format(self)))
    {
        lh_gl_pixmap_blend_cover_16_words(
            self, lh_ptr_rcast(lh_u16_t, lh_gl_pixmap_get_address(self, x, y)),
            lh_cast_static(lh_usize_t, count), cover, src);
        return;
    }
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
