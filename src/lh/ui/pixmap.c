/**
 * @file pixmap.c
 * @brief Implementation of `lh/ui/pixmap.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/byte/ptr.h>
#include <lh/bit/bswap.h>
#include <lh/byte/limits.h>
#include <lh/cast/static.h>
#include <lh/compiler/attribute/force_inline.h>
#include <lh/cpu/simd.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/numeric/float.h>
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
lh_ui_pixmap_init(lh_ui_pixmap_t *self, lh_byte_ptr bits, lh_s32_t width, lh_s32_t height, lh_s32_t stride,
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

lh_byte_ptr
lh_ui_pixmap_get_row(const lh_ui_pixmap_t *self, lh_s32_t y)
{
    lh_assert_runtime_ref(self);
    return self->bits + y * self->stride;
}

lh_byte_ptr
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
    lh_byte_cptr at = lh_ui_pixmap_get_address(self, x, y);

    return lh_ui_pixmap_format_is_16(self->format) ? *lh_ptr_rcast(const lh_u16_t, at)
                                                      : *lh_ptr_rcast(const lh_u32_t, at);
}

lh_void
lh_ui_pixmap_write_word(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, lh_u32_t word)
{
    lh_byte_ptr at = lh_ui_pixmap_get_address(self, x, y);

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
    lh_ui_pixmap_write_word(self, x, y, lh_ui_pixmap_over_word(self, lh_ui_pixmap_read_word(self, x, y), color));
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
    lh_byte_ptr at = lh_ui_pixmap_get_address(self, x0, y);

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
    return lh_ui_color_argb_over_opaque(dst, (a << 24) | src) & 0x00FFFFFFU;
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
    (void)self;
    return lh_ui_color_argb_over(dst, (a << 24) | src);
}

lh_u32_t
lh_ui_pixmap_mix_word(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    return lh_ui_pixmap_format_is_16(lh_ui_pixmap_get_format(self)) ? lh_ui_pixmap_mix_16(self, dst, src, a)
                                                                      : lh_ui_pixmap_mix_argb(self, dst, src, a);
}

lh_u32_t
lh_ui_pixmap_over_argb(const lh_ui_pixmap_t *self, lh_u32_t dst, const lh_ui_color_t *color)
{
    const lh_u32_t argb = lh_ui_color_get_argb(color);
    const lh_u32_t a = argb >> 24;

    lh_return_if(a == LH_BYTE_T_MAX, argb);
    lh_return_if(a == 0U, dst);
    return lh_ui_pixmap_mix_argb(self, dst, argb & 0x00FFFFFFU, a);
}

lh_u32_t
lh_ui_pixmap_over_16(const lh_ui_pixmap_t *self, lh_u32_t dst, const lh_ui_color_t *color)
{
    const lh_u32_t a = lh_ui_color_get_a(color);

    lh_return_if(a == LH_BYTE_T_MAX, lh_ui_pixmap_order_16(self, lh_ui_color_get_rgb565(color)));
    lh_return_if(a == 0U, dst);
    return lh_ui_pixmap_mix_16(self, dst, lh_ui_color_get_argb(color) & 0x00FFFFFFU, a);
}

lh_u32_t
lh_ui_pixmap_over_word(const lh_ui_pixmap_t *self, lh_u32_t dst, const lh_ui_color_t *color)
{
    return lh_ui_pixmap_format_is_16(lh_ui_pixmap_get_format(self)) ? lh_ui_pixmap_over_16(self, dst, color)
                                                                      : lh_ui_pixmap_over_argb(self, dst, color);
}

lh_u32_t
lh_ui_pixmap_rb_of_565(lh_u32_t word)
{
    /* Red to bits 16.., blue at 0, each then widened to 8 bits in its lane:
       v * 33 >> 2 is (v << 3) | (v >> 2) for a 5-bit v (lh_ui_color_expand_bits),
       and the mask drops what the shift brings down from the red lane. */
    return ((((word & 0xF800U) << 5) | (word & 0x001FU)) * 33U >> 2) & 0x00FF00FFU;
}

lh_u32_t
lh_ui_pixmap_g_of_565(lh_u32_t word)
{
    /* g * 65 >> 4 is (g << 2) | (g >> 4) for a 6-bit g. */
    return (((word >> 5) & 0x3FU) * 65U) >> 4;
}

lh_u32_t
lh_ui_pixmap_565_of(lh_u32_t rb, lh_u32_t g)
{
    /* Narrowing is truncation, as lh_ui_color_get_rgb565's. */
    return ((rb >> 19) << 11) | ((g >> 2) << 5) | ((rb & 0xFFU) >> 3);
}

lh_u32_t
lh_ui_pixmap_mix_16(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    const lh_u32_t word = lh_ui_pixmap_order_16(self, dst);

    /* The 888 blend of the widened word, bit for bit, without unpacking a channel
       at a time: red and blue go through one multiply pair (0x00FF00FF lanes),
       green through the other. This is the RGB565 path an MCU runs. */
    return lh_ui_pixmap_order_16(
        self, lh_ui_pixmap_565_of(lh_ui_color_mix_pair(lh_ui_pixmap_rb_of_565(word), src & 0x00FF00FFU, a),
                                  lh_ui_color_mix_pair(lh_ui_pixmap_g_of_565(word), (src >> 8) & 0xFFU, a)));
}

lh_u32_t
lh_ui_pixmap_blend_16_one(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t solid, lh_u32_t a)
{
    lh_return_if(a == 0U, dst);
    lh_return_if(a == LH_BYTE_T_MAX, solid);
    return lh_ui_pixmap_mix_16(self, dst, src, a);
}

lh_u32_t
lh_ui_pixmap_blend_16_pair(const lh_ui_pixmap_t *self, lh_u32_t pair, lh_u32_t src, lh_u32_t solid, lh_u32_t a_lo,
                           lh_u32_t a_hi)
{
    return lh_ui_pixmap_blend_16_one(self, pair & 0xFFFFU, src, solid, a_lo) |
           (lh_ui_pixmap_blend_16_one(self, pair >> 16, src, solid, a_hi) << 16);
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
/* One channel of four pixels, `(s * src_w + d * dst_w + total / 2) / total`: the
   channel at bit @p shift of every pixel, as floats, over its own divisor. */
#define LH_UI_PIXMAP_OVER_CHANNEL(pix, shift, src_word, src_w, dst_w, half, total)                        \
    _mm_cvttps_epi32(_mm_div_ps(                                                                          \
        _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(lh_cast_static(lh_float_t, ((src_word) >> (shift)) & 0xFFU)), \
                                         (src_w)),                                                        \
                              _mm_mul_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(pix, shift),         \
                                                                       _mm_set1_epi32(0xFF))),            \
                                         (dst_w))),                                                       \
                   (half)),                                                                               \
        (total)))

/* The translucent "over" of four words in registers: four alphas, one per 32-bit
   lane of @p a32. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_ui_pixmap_over_quad_m128(__m128i pix, __m128i a32, lh_u32_t src)
{
    const __m128i zero = _mm_setzero_si128();
    const __m128 full = _mm_set1_ps(255.0F);
    const __m128 round = _mm_set1_ps(127.0F);
    const __m128 a = _mm_cvtepi32_ps(a32);
    /* The weights of ::lh_ui_color_argb_over_translucent, exact in floats:
       src_w = a * 255, dst_w = dst_a * (255 - a), both at most 65025. */
    const __m128 src_w = _mm_mul_ps(a, full);
    const __m128 dst_w = _mm_mul_ps(_mm_cvtepi32_ps(_mm_srli_epi32(pix, 24)), _mm_sub_ps(full, a));
    const __m128 total = _mm_add_ps(src_w, dst_w);
    const __m128i total32 = _mm_cvttps_epi32(total);
    const __m128 half = _mm_cvtepi32_ps(_mm_srli_epi32(total32, 1));
    const __m128i r = LH_UI_PIXMAP_OVER_CHANNEL(pix, 16, src, src_w, dst_w, half, total);
    const __m128i g = LH_UI_PIXMAP_OVER_CHANNEL(pix, 8, src, src_w, dst_w, half, total);
    const __m128i b = LH_UI_PIXMAP_OVER_CHANNEL(pix, 0, src, src_w, dst_w, half, total);
    const __m128i out_a = _mm_cvttps_epi32(_mm_div_ps(_mm_add_ps(total, round), full));
    /* Two transparent words give 0 (the divisor was 0, the lanes are junk),
       and a zero alpha keeps the pixel it found. */
    const __m128i keep = _mm_cmpeq_epi32(a32, zero);
    __m128i word = _mm_or_si128(_mm_or_si128(_mm_slli_epi32(out_a, 24), _mm_slli_epi32(r, 16)),
                                _mm_or_si128(_mm_slli_epi32(g, 8), b));

    word = _mm_andnot_si128(_mm_cmpeq_epi32(total32, zero), word);
    return _mm_or_si128(_mm_and_si128(keep, pix), _mm_andnot_si128(keep, word));
}

/* Four alphas, the low four bytes of @p a8, one per 32-bit lane. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_ui_pixmap_alpha_lanes_32(__m128i a8)
{
    const __m128i zero = _mm_setzero_si128();

    return _mm_unpacklo_epi16(_mm_unpacklo_epi8(a8, zero), zero);
}

/* Four words blended in registers, pixel k at byte k of @p a8. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_ui_pixmap_blend_quad_m128(__m128i pix, __m128i a8, lh_u32_t src)
{
    const __m128i zero = _mm_setzero_si128();
    const __m128i full = _mm_set1_epi16(255);
    const __m128i round = _mm_set1_epi16(127);
    const __m128i one = _mm_set1_epi16(1);
    const __m128i src_word = _mm_set1_epi32(lh_cast_static(lh_s32_t, src));
    const __m128i src_lo = _mm_unpacklo_epi8(src_word, zero);
    const __m128i src_hi = _mm_unpackhi_epi8(src_word, zero);
    const __m128i alpha_mask = _mm_set1_epi32(lh_cast_static(lh_s32_t, 0xFF000000));
    /* Each pixel owns four 16-bit lanes in the order B, G, R, A, so its
       alpha has to land in four lanes, not one: a8 is four bytes, so widen
       it to one lane each, duplicate each lane, then pair them up into
       "the four lanes of pixel 0" and "the four lanes of pixel 1". Skipping
       either duplication blends pixel 0's green with pixel 1's alpha. */
    const __m128i a_dup = _mm_unpacklo_epi16(_mm_unpacklo_epi8(a8, zero), _mm_unpacklo_epi8(a8, zero));
    const __m128i a_lo = _mm_unpacklo_epi32(a_dup, a_dup);
    const __m128i a_hi = _mm_unpackhi_epi32(a_dup, a_dup);
    __m128i lo = _mm_unpacklo_epi8(pix, zero);
    __m128i hi = _mm_unpackhi_epi8(pix, zero);

    /* A translucent destination takes the general formula, with a divisor per
       pixel, so a block with one in it goes through the "over".
       The test is on the alpha byte of every pixel at once: masking keeps the
       alpha byte and zeroes the colour bytes, so comparing the result with the
       mask itself is true exactly where that alpha is 0xFF, and movemask packs
       those sixteen byte comparisons into one test. Comparing the unmasked
       pixel instead would put the colour bytes up against zero and send every
       pixel but a black one down the fallback. */
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_and_si128(pix, alpha_mask), alpha_mask)) != 0xFFFF)
    {
        return lh_ui_pixmap_over_quad_m128(pix, lh_ui_pixmap_alpha_lanes_32(a8), src);
    }
    /* t = (src * a + dst * (255 - a) + 127) / 255, per channel, in 16-bit
       lanes. Every product and every partial sum is <= 65025, so mullo and
       add cannot wrap; the widest intermediate is 65252 + 254 + 1 = 65507,
       still inside 16 bits. */
    lo = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(src_lo, a_lo), _mm_mullo_epi16(lo, _mm_sub_epi16(full, a_lo))),
                       round);
    hi = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(src_hi, a_hi), _mm_mullo_epi16(hi, _mm_sub_epi16(full, a_hi))),
                       round);
    /* 255 is odd, so x / 255 never ties at .5 and this round-to-nearest is
       the same value as the scalar (x + 127) / 255 for every x <= 65535. */
    lo = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(lo, _mm_srli_epi16(lo, 8)), one), 8);
    hi = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(hi, _mm_srli_epi16(hi, 8)), one), 8);
    return _mm_or_si128(_mm_packus_epi16(lo, hi), alpha_mask);
}

/* The first @p count (1 .. 3) words at @p at in the low lanes; the lanes past them
   opaque, so they never send an opaque run down the "over". Read word by word
   into the register: the eight-byte store and sixteen-byte load of a buffer on
   the stack is a store the CPU cannot forward. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_ui_pixmap_gather_32(const lh_u32_t *at, lh_usize_t count)
{
    const lh_s32_t opaque = lh_cast_static(lh_s32_t, 0xFF000000U);

    return _mm_setr_epi32(lh_cast_static(lh_s32_t, at[0]), count > 1U ? lh_cast_static(lh_s32_t, at[1]) : opaque,
                          count > 2U ? lh_cast_static(lh_s32_t, at[2]) : opaque, opaque);
}

/* The low @p count (1 .. 3) lanes of @p v back to @p at; the rest are not written. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE lh_void
lh_ui_pixmap_scatter_32(lh_u32_t *at, lh_usize_t count, __m128i v)
{
    at[0] = lh_cast_static(lh_u32_t, _mm_cvtsi128_si32(v));
    if (count > 1U)
    {
        at[1] = lh_cast_static(lh_u32_t, _mm_cvtsi128_si32(_mm_srli_si128(v, 4)));
    }
    if (count > 2U)
    {
        at[2] = lh_cast_static(lh_u32_t, _mm_cvtsi128_si32(_mm_srli_si128(v, 8)));
    }
}

/* The first @p count alphas at @p alpha in the low bytes of a register, 0 past them. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_ui_pixmap_gather_alpha(lh_byte_cptr alpha, lh_usize_t count)
{
    lh_u64_t bytes = 0U;
    lh_usize_t k;

    for (k = count; k > 0U; --k)
    {
        bytes = (bytes << 8) | alpha[k - 1U];
    }
    return _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(bytes)));
}

lh_void
lh_ui_pixmap_blend_alpha_32_over_quad_sse2(lh_u32_t *at, lh_byte_cptr alpha, lh_u32_t src)
{
    const __m128i pix = _mm_loadu_si128(lh_ptr_rcast(const __m128i, at));
    const __m128i a8 = _mm_cvtsi32_si128(lh_ptr_rcast(const lh_s32_t, alpha)[0]);

    _mm_storeu_si128(lh_ptr_rcast(__m128i, at), lh_ui_pixmap_over_quad_m128(pix, lh_ui_pixmap_alpha_lanes_32(a8), src));
}

lh_void
lh_ui_pixmap_blend_alpha_32_over_sse2(lh_u32_t *at, lh_byte_cptr alpha, lh_usize_t count, lh_u32_t src)
{
    lh_usize_t i;

    for (i = 0U; i + 4U <= count; i += 4U)
    {
        const __m128i a8 = _mm_cvtsi32_si128(lh_ptr_rcast(const lh_s32_t, alpha + i)[0]);

        _mm_storeu_si128(lh_ptr_rcast(__m128i, at + i),
                         lh_ui_pixmap_over_quad_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at + i)),
                                                     lh_ui_pixmap_alpha_lanes_32(a8), src));
    }
    lh_return_if(i == count);
    lh_ui_pixmap_scatter_32(at + i, count - i,
                            lh_ui_pixmap_over_quad_m128(lh_ui_pixmap_gather_32(at + i, count - i),
                                                        lh_ui_pixmap_alpha_lanes_32(lh_ui_pixmap_gather_alpha(alpha + i, count - i)),
                                                        src));
}

lh_void
lh_ui_pixmap_blend_alpha_32_quad_sse2(lh_u32_t *at, lh_byte_cptr alpha, lh_u32_t src)
{
    const __m128i pix = _mm_loadu_si128(lh_ptr_rcast(const __m128i, at));
    const __m128i a8 = _mm_cvtsi32_si128(lh_ptr_rcast(const lh_s32_t, alpha)[0]);

    _mm_storeu_si128(lh_ptr_rcast(__m128i, at), lh_ui_pixmap_blend_quad_m128(pix, a8, src));
}

lh_void
lh_ui_pixmap_blend_alpha_32_sse2(const lh_ui_pixmap_t *self, lh_u32_t *at, lh_byte_cptr alpha, lh_usize_t count,
                                 lh_u32_t src)
{
    lh_usize_t i;

    (void)self;
    /* The block itself, not the exported quad: inlined, its constants are set up
       once for the run instead of once a block. */
    for (i = 0U; i + 4U <= count; i += 4U)
    {
        const __m128i a8 = _mm_cvtsi32_si128(lh_ptr_rcast(const lh_s32_t, alpha + i)[0]);

        _mm_storeu_si128(lh_ptr_rcast(__m128i, at + i),
                         lh_ui_pixmap_blend_quad_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at + i)), a8, src));
    }
    lh_return_if(i == count);
    /* The last one to three pixels are a block too, not a scalar loop that
       branched per pixel on a coverage that is noise to the predictor: a glyph
       row is six to fourteen pixels, so this tail is part of every row a caption
       draws. */
    lh_ui_pixmap_scatter_32(at + i, count - i,
                            lh_ui_pixmap_blend_quad_m128(lh_ui_pixmap_gather_32(at + i, count - i),
                                                         lh_ui_pixmap_gather_alpha(alpha + i, count - i), src));
}

lh_void
lh_ui_pixmap_blend_cover_32_sse2(lh_u32_t *at, lh_usize_t count, lh_u64_t cover, lh_u32_t src)
{
    const __m128i a8 = _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(cover)));
    const __m128i a8_hi = _mm_srli_si128(a8, 4);

    lh_assert_runtime(count >= 1U && count <= 8U);
    if (count < 4U)
    {
        lh_ui_pixmap_scatter_32(at, count, lh_ui_pixmap_blend_quad_m128(lh_ui_pixmap_gather_32(at, count), a8, src));
        return;
    }
    _mm_storeu_si128(lh_ptr_rcast(__m128i, at),
                     lh_ui_pixmap_blend_quad_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at)), a8, src));
    if (count == 8U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, at + 4),
                         lh_ui_pixmap_blend_quad_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at + 4)), a8_hi, src));
    }
    else if (count > 4U)
    {
        lh_ui_pixmap_scatter_32(at + 4, count - 4U,
                                lh_ui_pixmap_blend_quad_m128(lh_ui_pixmap_gather_32(at + 4, count - 4U), a8_hi, src));
    }
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

lh_void
lh_ui_pixmap_blend_alpha_32(const lh_ui_pixmap_t *self, lh_u32_t *at, lh_byte_cptr alpha, lh_usize_t count,
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

/* Eight RGB565 words blended in registers, pixel k at byte k of @p a8. */
LH_COMPILER_ATTRIBUTE_FORCE_INLINE __m128i
lh_ui_pixmap_blend_octet_m128(__m128i in, __m128i a8, lh_u32_t src, lh_bool_t swap)
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
    __m128i r8;
    __m128i g8;
    __m128i b8;

    /* A swapped format keeps the low byte first; one 16-bit byte swap per block
       turns it into the numeric RGB565 the planes below expect, and the same
       swap on the way out puts it back. */
    if (swap)
    {
        in = _mm_or_si128(_mm_slli_epi16(in, 8), _mm_srli_epi16(in, 8));
    }
    LH_UI_PIXMAP_BLEND_PLANE(r8, _mm_and_si128(_mm_srli_epi16(in, 11), m31), src8r, 3, 2, 3);
    LH_UI_PIXMAP_BLEND_PLANE(g8, _mm_and_si128(_mm_srli_epi16(in, 5), m63), src8g, 2, 4, 2);
    LH_UI_PIXMAP_BLEND_PLANE(b8, _mm_and_si128(in, m31), src8b, 3, 2, 3);
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
lh_ui_pixmap_gather_16(const lh_u16_t *at, lh_usize_t count)
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
lh_ui_pixmap_scatter_16(lh_u16_t *at, lh_usize_t count, __m128i v)
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
lh_ui_pixmap_blend_alpha_16_octet_sse2(lh_u16_t *at, lh_byte_cptr alpha, lh_u32_t src, lh_bool_t swap)
{
    _mm_storeu_si128(lh_ptr_rcast(__m128i, at),
                     lh_ui_pixmap_blend_octet_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at)),
                                                   _mm_loadl_epi64(lh_ptr_rcast(const __m128i, alpha)), src, swap));
}

lh_void
lh_ui_pixmap_blend_alpha_16_sse2(const lh_ui_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha, lh_usize_t count,
                                 lh_u32_t src)
{
    const lh_bool_t swap = lh_ui_pixmap_get_format(self) == lh_ui_pixmap_format_rgb565_swapped;
    lh_usize_t i;

    for (i = 0U; i + 8U <= count; i += 8U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, at + i),
                         lh_ui_pixmap_blend_octet_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at + i)),
                                                       _mm_loadl_epi64(lh_ptr_rcast(const __m128i, alpha + i)), src,
                                                       swap));
    }
    lh_return_if(i == count);
    /* The tail is a block too, as in ::lh_ui_pixmap_blend_alpha_32_sse2. */
    lh_ui_pixmap_scatter_16(at + i, count - i,
                            lh_ui_pixmap_blend_octet_m128(lh_ui_pixmap_gather_16(at + i, count - i),
                                                          lh_ui_pixmap_gather_alpha(alpha + i, count - i), src, swap));
}

lh_void
lh_ui_pixmap_blend_cover_16_sse2(lh_u16_t *at, lh_usize_t count, lh_u64_t cover, lh_u32_t src, lh_bool_t swap)
{
    const __m128i a8 = _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(cover)));

    lh_assert_runtime(count >= 1U && count <= 8U, lh_runtime_error_code_invalid_argument);
    if (count == 8U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, at),
                         lh_ui_pixmap_blend_octet_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at)), a8, src, swap));
        return;
    }
    lh_ui_pixmap_scatter_16(at, count, lh_ui_pixmap_blend_octet_m128(lh_ui_pixmap_gather_16(at, count), a8, src, swap));
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

lh_void
lh_ui_pixmap_blend_alpha_16(const lh_ui_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha, lh_usize_t count,
                            lh_u32_t src)
{
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (lh_cpu_simd_has_sse2())
    {
        lh_ui_pixmap_blend_alpha_16_sse2(self, at, alpha, count, src);
        return;
    }
#endif
    lh_ui_pixmap_blend_alpha_16_words(self, at, alpha, count, src);
}

lh_void
lh_ui_pixmap_blend_alpha_16_words(const lh_ui_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha, lh_usize_t count,
                                  lh_u32_t src)
{
    const lh_u32_t solid = lh_ui_pixmap_order_16(self, lh_ui_pixmap_pack_rgb(self, src));
    lh_usize_t i = 0U;

    /* Two pixels a 32-bit load and store once @p at is word-aligned; the pixel
       before that and an odd last one go alone. */
    if (count > 0U && (lh_ptr_to_uaddr(at) & 2U) != 0U)
    {
        at[0] = lh_cast_static(lh_u16_t, lh_ui_pixmap_blend_16_one(self, at[0], src, solid, alpha[0]));
        i = 1U;
    }
    for (; i + 2U <= count; i += 2U)
    {
        lh_ui_pixmap_blend_16_pair_at(self, at + i, alpha + i, src, solid);
    }
    if (i < count)
    {
        at[i] = lh_cast_static(lh_u16_t, lh_ui_pixmap_blend_16_one(self, at[i], src, solid, alpha[i]));
    }
}

lh_void
lh_ui_pixmap_blend_16_pair_at(const lh_ui_pixmap_t *self, lh_u16_t *at, lh_byte_cptr alpha, lh_u32_t src,
                              lh_u32_t solid)
{
    /* Each pixel through its own 16-bit access (a 32-bit lvalue on a 16-bit buffer
       is an aliasing violation; the compiler may merge the two). Joining them into
       one word only for lh_ui_pixmap_blend_16_pair to split it again bought nothing. */
    lh_return_if(alpha[0] == 0U && alpha[1] == 0U);
    at[0] = lh_cast_static(lh_u16_t, lh_ui_pixmap_blend_16_one(self, at[0], src, solid, alpha[0]));
    at[1] = lh_cast_static(lh_u16_t, lh_ui_pixmap_blend_16_one(self, at[1], src, solid, alpha[1]));
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
                              lh_byte_cptr alpha)
{
    lh_byte_ptr at = lh_ui_pixmap_get_address(self, x0, y);

    lh_return_if(x1 <= x0);
    if (lh_ui_pixmap_format_is_16(self->format))
    {
        lh_ui_pixmap_blend_alpha_16(self, lh_ptr_rcast(lh_u16_t, at), alpha, lh_cast_static(lh_usize_t, x1 - x0), src);
        return;
    }
    lh_ui_pixmap_blend_alpha_32(self, lh_ptr_rcast(lh_u32_t, at), alpha, lh_cast_static(lh_usize_t, x1 - x0), src);
}

lh_void
lh_ui_pixmap_blend_cover_8(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, lh_s32_t count, lh_u64_t cover, lh_u32_t src)
{
    lh_byte_t alpha[8];
    lh_s32_t i;

    lh_assert_runtime(count >= 1 && count <= 8, lh_runtime_error_code_invalid_argument);
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (lh_cpu_simd_has_sse2())
    {
        lh_byte_ptr at = lh_ui_pixmap_get_address(self, x, y);

        if (lh_ui_pixmap_format_is_16(self->format))
        {
            lh_ui_pixmap_blend_cover_16_sse2(lh_ptr_rcast(lh_u16_t, at), lh_cast_static(lh_usize_t, count), cover, src,
                                             self->format == lh_ui_pixmap_format_rgb565_swapped);
            return;
        }
        lh_ui_pixmap_blend_cover_32_sse2(lh_ptr_rcast(lh_u32_t, at), lh_cast_static(lh_usize_t, count), cover, src);
        return;
    }
#endif
    for (i = 0; i < count; ++i)
    {
        alpha[i] = lh_cast_static(lh_byte_t, cover >> (8 * i));
    }
    lh_ui_pixmap_blend_alpha_span(self, x, x + count, y, src, alpha);
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
lh_ui_pixmap_fill_colors_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                              const lh_ui_color_t *colors)
{
    const lh_usize_t count = x1 > x0 ? lh_cast_static(lh_usize_t, x1 - x0) : 0U;
    lh_byte_ptr row = lh_ui_pixmap_get_address(self, x0, y);
    lh_usize_t i;

    if (lh_ui_pixmap_format_is_16(self->format))
    {
        lh_u16_t *at = lh_ptr_rcast(lh_u16_t, row);

        for (i = 0U; i < count; ++i)
        {
            at[i] = lh_cast_static(lh_u16_t, lh_ui_pixmap_over_16(self, at[i], colors + i));
        }
        return;
    }
    {
        lh_u32_t *at = lh_ptr_rcast(lh_u32_t, row);

        for (i = 0U; i < count; ++i)
        {
            at[i] = lh_ui_pixmap_over_argb(self, at[i], colors + i);
        }
    }
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
