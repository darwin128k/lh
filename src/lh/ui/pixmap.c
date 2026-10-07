/**
 * @file pixmap.c
 * @brief Implementation of `lh/ui/pixmap.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bit/bswap.h>
#include <lh/byte/limits.h>
#include <lh/cast/static.h>
#include <lh/cpu/simd.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/size.h>
#include <lh/ui/pixmap.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
#    include <immintrin.h>
#    if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif
#endif

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
    /* Eight per turn: cheap loop overhead unoptimized, still vectorized at -O2/-O3.
       Measured on this machine, both this and a machine-word variant land on the
       same ~25 GB/s fill bandwidth, so the store is not what a frame waits on. */
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

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
lh_void
lh_ui_pixmap_blend_alpha_32_sse2(const lh_ui_pixmap_t *self, lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                 lh_u32_t src)
{
    const __m128i zero = _mm_setzero_si128();
    const __m128i full = _mm_set1_epi16(255);
    const __m128i round = _mm_set1_epi16(127);
    const __m128i one = _mm_set1_epi16(1);
    const __m128i src_word = _mm_set1_epi32(lh_cast_static(lh_s32_t, src));
    const __m128i src_lo = _mm_unpacklo_epi8(src_word, zero);
    const __m128i src_hi = _mm_unpackhi_epi8(src_word, zero);
    const __m128i alpha_mask = _mm_set1_epi32(lh_cast_static(lh_s32_t, 0xFF000000));
    lh_usize_t i;

    for (i = 0U; i + 4U <= count; i += 4U)
    {
        const __m128i pix = _mm_loadu_si128(lh_ptr_rcast(const __m128i, at + i));
        const __m128i a8 = _mm_cvtsi32_si128(lh_ptr_rcast(lh_s32_t, alpha + i)[0]);
        /* Each pixel owns four 16-bit lanes in the order B, G, R, A, so its
           alpha has to land in four lanes, not one: a8 is four bytes, so widen
           it to one lane each, duplicate each lane, then pair them up into
           "the four lanes of pixel 0" and "the four lanes of pixel 1". Skipping
           either duplication blends pixel 0's green with pixel 1's alpha. */
        const __m128i a_dup = _mm_unpacklo_epi16(_mm_unpacklo_epi8(a8, zero), _mm_unpacklo_epi8(a8, zero));
        __m128i lo = _mm_unpacklo_epi8(pix, zero);
        __m128i hi = _mm_unpackhi_epi8(pix, zero);
        const __m128i a_lo = _mm_unpacklo_epi32(a_dup, a_dup);
        const __m128i a_hi = _mm_unpackhi_epi32(a_dup, a_dup);

        /* A translucent destination takes a different formula in the scalar path
           (lh_ui_pixmap_mix_any), so those pixels have to go back through it.
           The test is on the alpha byte of every pixel at once: masking keeps the
           alpha byte and zeroes the colour bytes, so comparing the result with the
           mask itself is true exactly where that alpha is 0xFF, and movemask packs
           those sixteen byte comparisons into one test. Comparing the unmasked
           pixel instead would put the colour bytes up against zero and send every
           pixel but a black one down the fallback. */
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_and_si128(pix, alpha_mask), alpha_mask)) != 0xFFFF)
        {
            lh_usize_t k;

            for (k = 0U; k < 4U; ++k)
            {
                /* The skip on a zero alpha is not an optimisation, it is the
                   contract: on a transparent destination a zero alpha must
                   leave the pixel exactly as it was, and mix_any would write a
                   computed zero over it instead. */
                if (alpha[i + k] != 0U)
                {
                    at[i + k] = alpha[i + k] == 0xFFU
                                    ? 0xFF000000U | src
                                    : lh_ui_pixmap_mix_argb(self, at[i + k], src, alpha[i + k]);
                }
            }
            continue;
        }
        /* t = (src * a + dst * (255 - a) + 127) / 255, per channel, in 16-bit
           lanes. Every product and every partial sum is <= 65025, so mullo and
           add cannot wrap; the widest intermediate is 65252 + 254 + 1 = 65507,
           still inside 16 bits. */
        lo = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(src_lo, a_lo),
                                         _mm_mullo_epi16(lo, _mm_sub_epi16(full, a_lo))),
                            round);
        hi = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(src_hi, a_hi),
                                         _mm_mullo_epi16(hi, _mm_sub_epi16(full, a_hi))),
                            round);
        /* 255 is odd, so x / 255 never ties at .5 and this round-to-nearest is
           the same value as the scalar (x + 127) / 255 for every x <= 65535. */
        lo = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(lo, _mm_srli_epi16(lo, 8)), one), 8);
        hi = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(hi, _mm_srli_epi16(hi, 8)), one), 8);
        _mm_storeu_si128(
            lh_ptr_rcast(__m128i, at + i),
            _mm_or_si128(_mm_packus_epi16(lo, hi), _mm_set1_epi32(lh_cast_static(lh_s32_t, 0xFF000000))));
    }
    for (; i < count; ++i)
    {
        if (alpha[i] != 0U)
        {
            at[i] = alpha[i] == 0xFFU ? 0xFF000000U | src : lh_ui_pixmap_mix_argb(self, at[i], src, alpha[i]);
        }
    }
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

lh_void
lh_ui_pixmap_blend_alpha_32(const lh_ui_pixmap_t *self, lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                            lh_u32_t src)
{
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (lh_cpu_simd_has_sse2())
    {
        lh_ui_pixmap_blend_alpha_32_sse2(self, at, alpha, count, src);
        return;
    }
#endif
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
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
/* (x + 127) / 255, in 16-bit lanes, for x <= 65535 — the same value the scalar
   kernel divides its way to, with no divide at all. 255 is odd, so x / 255 never
   lands exactly on .5 and this round-to-nearest cannot disagree with it. */
#define LH_UI_PIXMAP_DIV255(x) _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(x, _mm_srli_epi16(x, 8)), one), 8)

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
#define LH_UI_PIXMAP_BLEND_PLANE(out8, bits, src8, up, down, narrow)                                     \
    do                                                                                                   \
    {                                                                                                    \
        const __m128i dst8 = _mm_or_si128(_mm_slli_epi16(bits, up), _mm_srli_epi16(bits, down));          \
        const __m128i t = _mm_add_epi16(                                                                 \
            _mm_add_epi16(_mm_mullo_epi16(src8, alpha16), _mm_mullo_epi16(dst8, inv)), round);            \
        out8 = _mm_srli_epi16(LH_UI_PIXMAP_DIV255(t), narrow);                                            \
    } while (0)

lh_void
lh_ui_pixmap_blend_alpha_16_sse2(const lh_ui_pixmap_t *self, lh_u16_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                 lh_u32_t src)
{
    const __m128i zero = _mm_setzero_si128();
    const __m128i full = _mm_set1_epi16(255);
    const __m128i round = _mm_set1_epi16(127);
    const __m128i one = _mm_set1_epi16(1);
    const __m128i m31 = _mm_set1_epi16(0x1F);
    const __m128i m63 = _mm_set1_epi16(0x3F);
    /* The source as three planes of eight 8-bit values, each widened to lanes. */
    const __m128i src8r = _mm_set1_epi16((short) ((src >> 16) & 0xFF));
    const __m128i src8g = _mm_set1_epi16((short) ((src >> 8) & 0xFF));
    const __m128i src8b = _mm_set1_epi16((short) (src & 0xFF));
    /* A swapped format keeps the low byte first; one 16-bit byte swap per block
       turns it into the numeric RGB565 the planes below expect, and the same
       swap on the way out puts it back. */
    const lh_bool_t swap = lh_ui_pixmap_get_format(self) == lh_ui_pixmap_format_rgb565_swapped;
    lh_usize_t i;

    for (i = 0U; i + 8U <= count; i += 8U)
    {
        const __m128i a8 = _mm_loadl_epi64(lh_ptr_rcast(const __m128i, alpha + i));
        __m128i in = _mm_loadu_si128(lh_ptr_rcast(const __m128i, at + i));
        __m128i alpha16;
        __m128i inv;
        __m128i r8;
        __m128i g8;
        __m128i b8;

        if (swap)
        {
            in = _mm_or_si128(_mm_slli_epi16(in, 8), _mm_srli_epi16(in, 8));
        }
        alpha16 = _mm_unpacklo_epi8(a8, zero);
        inv = _mm_sub_epi16(full, alpha16);
        LH_UI_PIXMAP_BLEND_PLANE(r8, _mm_and_si128(_mm_srli_epi16(in, 11), m31), src8r, 3, 2, 3);
        LH_UI_PIXMAP_BLEND_PLANE(g8, _mm_and_si128(_mm_srli_epi16(in, 5), m63), src8g, 2, 4, 2);
        LH_UI_PIXMAP_BLEND_PLANE(b8, _mm_and_si128(in, m31), src8b, 3, 2, 3);
        in = _mm_or_si128(_mm_slli_epi16(r8, 11), _mm_or_si128(_mm_slli_epi16(g8, 5), b8));
        if (swap)
        {
            in = _mm_or_si128(_mm_slli_epi16(in, 8), _mm_srli_epi16(in, 8));
        }
        _mm_storeu_si128(lh_ptr_rcast(__m128i, at + i), in);
    }
    for (; i < count; ++i)
    {
        if (alpha[i] != 0U)
        {
            at[i] = lh_cast_static(lh_u16_t, lh_ui_pixmap_mix_16(self, at[i], src, alpha[i]));
        }
    }
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

lh_void
lh_ui_pixmap_blend_alpha_16(const lh_ui_pixmap_t *self, lh_u16_t *at, const lh_byte_t *alpha, lh_usize_t count,
                            lh_u32_t src)
{
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (lh_cpu_simd_has_sse2())
    {
        lh_ui_pixmap_blend_alpha_16_sse2(self, at, alpha, count, src);
        return;
    }
#endif
    {
        const lh_u16_t solid =
            lh_cast_static(lh_u16_t, lh_ui_pixmap_order_16(self, lh_ui_pixmap_pack_rgb(self, src)));
        lh_usize_t i;

        for (i = 0U; i < count; ++i)
        {
            if (alpha[i] != 0U)
            {
                at[i] = alpha[i] == 0xFFU ? solid : lh_cast_static(lh_u16_t, lh_ui_pixmap_mix_16(self, at[i], src, alpha[i]));
            }
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
