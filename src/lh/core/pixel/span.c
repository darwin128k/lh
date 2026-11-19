/**
 * @file span.c
 * @brief Implementation of `lh/core/pixel/span.h`.
 */

#include <lh/core/pixel/span.h>

#include <lh/core/cpu/simd.h>
#include <lh/core/memory.h>
#include <lh/core/memory/word.h>
#include <lh/core/numeric/float.h>
#include <lh/core/pixel/rgb565.h>

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
#    include <immintrin.h>
#    if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif
#endif

/** A run of one alpha at least this long is one colour laid over (::lh_pixel_span_blend_argb). */
#define LH_PIXEL_SPAN_CONST_RUN 4U

/** A row shorter than this goes to the SSE2 kernel whole, runs and all. */
#define LH_PIXEL_SPAN_SHORT_ROW 16U

/** The coverages of a translucent colour scaled to alphas at a time, on the stack. */
#define LH_PIXEL_SPAN_SCALE_RUN 64U

lh_void
lh_pixel_span_blend_argb(lh_u32_t *at, lh_usize_t count, lh_u32_t src, lh_u32_t alpha)
{
    lh_return_if(count == 0U || alpha == 0U);
    if (alpha == LH_BYTE_T_MAX)
    {
        lh_memory_set_u32(at, count, lh_bit_or(0xFF000000U, src));
        return;
    }
    const lh_u32_t over = lh_bit_or(lh_bit_shl(alpha, 24), lh_bit_and(src, 0x00FFFFFFU));

    for (lh_usize_t i = 0U; i < count;)
    {
        const lh_u32_t dst = at[i];
        lh_usize_t end = i + 1U;

        while (end < count && at[end] == dst)
        {
            ++end;
        }
        lh_memory_set_u32(at + i, end - i, lh_pixel_argb_over(dst, over));
        i = end;
    }
}

lh_void
lh_pixel_span_blend_rgb565(lh_u16_t *at, lh_usize_t count, lh_u32_t src, lh_u32_t alpha,
                           lh_bool_t swapped)
{
    lh_return_if(count == 0U || alpha == 0U);
    if (alpha == LH_BYTE_T_MAX)
    {
        lh_memory_set_u16(at, count,
                          lh_cast_static(lh_u16_t, lh_pixel_rgb565_order(
                                                       lh_pixel_rgb565_from_argb(src), swapped)));
        return;
    }
    for (lh_usize_t i = 0U; i < count;)
    {
        const lh_u16_t dst = at[i];
        lh_usize_t end = i + 1U;

        while (end < count && at[end] == dst)
        {
            ++end;
        }
        const lh_u32_t mixed = lh_pixel_rgb565_mix(lh_pixel_rgb565_order(dst, swapped), src, alpha);

        lh_memory_set_u16(at + i, end - i,
                          lh_cast_static(lh_u16_t, lh_pixel_rgb565_order(mixed, swapped)));
        i = end;
    }
}

lh_void
lh_pixel_span_cover_rgb565(lh_u16_t *at, const lh_byte_t *cover, lh_usize_t count, lh_u32_t src,
                           lh_u32_t alpha, lh_bool_t swapped)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = lh_pixel_channel_div255_round(cover[i] * alpha);

        if (a != 0U)
        {
            at[i] = lh_cast_static(
                lh_u16_t,
                lh_pixel_rgb565_order(
                    lh_pixel_rgb565_mix(lh_pixel_rgb565_order(at[i], swapped), src, a), swapped));
        }
    }
}

lh_void
lh_pixel_span_blend_each_argb(lh_u32_t *at, const lh_u32_t *src, lh_usize_t count)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = lh_pixel_argb_get_channel(src[i], 24);

        if (a == LH_BYTE_T_MAX)
        {
            at[i] = src[i];
        }
        else if (a != 0U)
        {
            at[i] = lh_pixel_argb_over(at[i], src[i]);
        }
    }
}

lh_void
lh_pixel_span_blend_each_rgb565(lh_u16_t *at, const lh_u32_t *src, lh_usize_t count,
                                lh_bool_t swapped)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = lh_pixel_argb_get_channel(src[i], 24);

        if (a != 0U)
        {
            at[i] = lh_cast_static(
                lh_u16_t, lh_pixel_rgb565_order(
                              lh_pixel_rgb565_mix(lh_pixel_rgb565_order(at[i], swapped), src[i], a),
                              swapped));
        }
    }
}

lh_void
lh_pixel_span_cover_each_argb(lh_u32_t *at, const lh_byte_t *cover, const lh_u32_t *src,
                              lh_usize_t count)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a =
            lh_pixel_channel_div255_round(cover[i] * lh_pixel_argb_get_channel(src[i], 24));
        const lh_u32_t rgb = lh_bit_and(src[i], 0x00FFFFFFU);

        if (a == LH_BYTE_T_MAX)
        {
            at[i] = lh_bit_or(0xFF000000U, rgb);
        }
        else if (a != 0U)
        {
            at[i] = lh_pixel_argb_over(at[i], lh_bit_or(lh_bit_shl(a, 24), rgb));
        }
    }
}

lh_void
lh_pixel_span_cover_each_rgb565(lh_u16_t *at, const lh_byte_t *cover, const lh_u32_t *src,
                                lh_usize_t count, lh_bool_t swapped)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a =
            lh_pixel_channel_div255_round(cover[i] * lh_pixel_argb_get_channel(src[i], 24));

        if (a != 0U)
        {
            at[i] = lh_cast_static(
                lh_u16_t, lh_pixel_rgb565_order(
                              lh_pixel_rgb565_mix(lh_pixel_rgb565_order(at[i], swapped), src[i], a),
                              swapped));
        }
    }
}

lh_void
lh_pixel_span_over_opaque_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count, lh_u32_t dst,
                               lh_u32_t rgb)
{
    /* Taken once: a shadow edge and a glyph row share one destination word. */
    const lh_u32_t dst_rb = lh_bit_and(dst, 0x00FF00FFU);
    const lh_u32_t dst_ga = lh_bit_and(lh_bit_shr(dst, 8), 0x00FF00FFU);
    const lh_u32_t src_rb = lh_bit_and(rgb, 0x00FF00FFU);
    const lh_u32_t src_ga = lh_bit_and(lh_bit_shr(rgb, 8), 0x00FF00FFU);
    const lh_u32_t solid = lh_bit_or(0xFF000000U, rgb);

    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = alpha[i];

        if (a == LH_BYTE_T_MAX)
        {
            at[i] = solid;
        }
        else if (a != 0U)
        {
            const lh_u32_t rb = lh_pixel_channel_mix_pair(dst_rb, src_rb, a);
            const lh_u32_t ga = lh_pixel_channel_mix_pair(dst_ga, src_ga, a);

            at[i] = lh_bit_or(lh_bit_or(0xFF000000U, rb), lh_bit_shl(lh_bit_and(ga, 0xFFU), 8));
        }
    }
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
/* One channel of four pixels, `(s * src_w + d * dst_w + total / 2) / total`: the
   channel at bit @p shift of every pixel, as floats, over its own divisor. */
#    define LH_PIXEL_SPAN_OVER_CHANNEL(pix, shift, src_word, src_w, dst_w, half, total)            \
        _mm_cvttps_epi32(_mm_div_ps(                                                               \
            _mm_add_ps(                                                                            \
                _mm_add_ps(_mm_mul_ps(_mm_set1_ps(lh_cast_static(                                  \
                                          lh_float_t, ((src_word) >> (shift)) & 0xFFU)),           \
                                      (src_w)),                                                    \
                           _mm_mul_ps(_mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(pix, shift),    \
                                                                    _mm_set1_epi32(0xFF))),        \
                                      (dst_w))),                                                   \
                (half)),                                                                           \
            (total)))

/* The translucent "over" of four words in registers: four alphas, one per 32-bit
   lane of @p a32. */
LH_ATTRIBUTE_FORCE_INLINE __m128i
lh_pixel_span_over_quad_m128(__m128i pix, __m128i a32, lh_u32_t src)
{
    const __m128i zero = _mm_setzero_si128();
    const __m128 full = _mm_set1_ps(255.0F);
    const __m128 round = _mm_set1_ps(127.0F);
    const __m128 a = _mm_cvtepi32_ps(a32);
    /* The weights of ::lh_pixel_argb_over_translucent, exact in floats:
       src_w = a * 255, dst_w = dst_a * (255 - a), both at most 65025. */
    const __m128 src_w = _mm_mul_ps(a, full);
    const __m128 dst_w = _mm_mul_ps(_mm_cvtepi32_ps(_mm_srli_epi32(pix, 24)), _mm_sub_ps(full, a));
    const __m128 total = _mm_add_ps(src_w, dst_w);
    const __m128i total32 = _mm_cvttps_epi32(total);
    const __m128 half = _mm_cvtepi32_ps(_mm_srli_epi32(total32, 1));
    const __m128i r = LH_PIXEL_SPAN_OVER_CHANNEL(pix, 16, src, src_w, dst_w, half, total);
    const __m128i g = LH_PIXEL_SPAN_OVER_CHANNEL(pix, 8, src, src_w, dst_w, half, total);
    const __m128i b = LH_PIXEL_SPAN_OVER_CHANNEL(pix, 0, src, src_w, dst_w, half, total);
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
LH_ATTRIBUTE_FORCE_INLINE __m128i
lh_pixel_span_alpha_lanes_32(__m128i a8)
{
    const __m128i zero = _mm_setzero_si128();

    return _mm_unpacklo_epi16(_mm_unpacklo_epi8(a8, zero), zero);
}

/* The first @p count (1 .. 3) words at @p at in the low lanes; the lanes past them
   opaque, so they never send an opaque run down the "over". Read word by word into
   the register: the eight-byte store and sixteen-byte load of a buffer on the stack
   is a store the CPU cannot forward. */
LH_ATTRIBUTE_FORCE_INLINE __m128i
lh_pixel_span_gather_32(const lh_u32_t *at, lh_usize_t count)
{
    const lh_s32_t opaque = lh_cast_static(lh_s32_t, 0xFF000000U);

    return _mm_setr_epi32(lh_cast_static(lh_s32_t, at[0]),
                          count > 1U ? lh_cast_static(lh_s32_t, at[1]) : opaque,
                          count > 2U ? lh_cast_static(lh_s32_t, at[2]) : opaque, opaque);
}

/* The low @p count (1 .. 3) lanes of @p v back to @p at; the rest are not written. */
LH_ATTRIBUTE_FORCE_INLINE lh_void
lh_pixel_span_scatter_32(lh_u32_t *at, lh_usize_t count, __m128i v)
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
LH_ATTRIBUTE_FORCE_INLINE __m128i
lh_pixel_span_gather_alpha(const lh_byte_t *alpha, lh_usize_t count)
{
    lh_u64_t bytes = 0U;

    for (lh_usize_t k = count; k > 0U; --k)
    {
        bytes = lh_bit_or(lh_bit_shl(bytes, 8), alpha[k - 1U]);
    }
    return _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(bytes)));
}

/* Four alphas from @p alpha, any address, into the low bytes of a register. */
LH_ATTRIBUTE_FORCE_INLINE __m128i
lh_pixel_span_load_alpha_4(const lh_byte_t *alpha)
{
    return _mm_cvtsi32_si128(lh_cast_static(lh_s32_t, lh_memory_load_u32(alpha)));
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
/* Four words blended in registers, pixel k at byte k of @p a8. */
LH_ATTRIBUTE_FORCE_INLINE __m128i
lh_pixel_span_blend_quad_m128(__m128i pix, __m128i a8, lh_u32_t src)
{
    const __m128i zero = _mm_setzero_si128();
    const __m128i full = _mm_set1_epi16(255);
    const __m128i round = _mm_set1_epi16(127);
    const __m128i one = _mm_set1_epi16(1);
    const __m128i src_word = _mm_set1_epi32(lh_cast_static(lh_s32_t, src));
    const __m128i src_lo = _mm_unpacklo_epi8(src_word, zero);
    const __m128i src_hi = _mm_unpackhi_epi8(src_word, zero);
    const __m128i alpha_mask = _mm_set1_epi32(lh_cast_static(lh_s32_t, 0xFF000000U));
    /* Each pixel owns four 16-bit lanes in the order B, G, R, A, so its alpha has to
       land in four lanes, not one: a8 is four bytes, so widen it to one lane each,
       duplicate each lane, then pair them up into "the four lanes of pixel 0" and
       "the four lanes of pixel 1". Skipping either duplication blends pixel 0's
       green with pixel 1's alpha. */
    const __m128i a_dup =
        _mm_unpacklo_epi16(_mm_unpacklo_epi8(a8, zero), _mm_unpacklo_epi8(a8, zero));
    const __m128i a_lo = _mm_unpacklo_epi32(a_dup, a_dup);
    const __m128i a_hi = _mm_unpackhi_epi32(a_dup, a_dup);
    __m128i lo = _mm_unpacklo_epi8(pix, zero);
    __m128i hi = _mm_unpackhi_epi8(pix, zero);

    /* A translucent destination takes the general formula, with a divisor per pixel,
       so a block with one in it goes through the "over". The test is on the alpha
       byte of every pixel at once: masking keeps the alpha byte and zeroes the colour
       bytes, so comparing the result with the mask itself is true exactly where that
       alpha is 0xFF, and movemask packs those sixteen comparisons into one test. */
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_and_si128(pix, alpha_mask), alpha_mask)) != 0xFFFF)
    {
        return lh_pixel_span_over_quad_m128(pix, lh_pixel_span_alpha_lanes_32(a8), src);
    }
    /* t = (src * a + dst * (255 - a) + 127) / 255, per channel, in 16-bit lanes.
       Every product and every partial sum is <= 65025, so mullo and add cannot
       wrap; the widest intermediate is 65252 + 254 + 1 = 65507, still inside 16
       bits. */
    lo = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(src_lo, a_lo),
                                     _mm_mullo_epi16(lo, _mm_sub_epi16(full, a_lo))),
                       round);
    hi = _mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(src_hi, a_hi),
                                     _mm_mullo_epi16(hi, _mm_sub_epi16(full, a_hi))),
                       round);
    /* 255 is odd, so x / 255 never ties at .5 and this round-to-nearest is the same
       value as the scalar (x + 127) / 255 for every x <= 65535. */
    lo = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(lo, _mm_srli_epi16(lo, 8)), one), 8);
    hi = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(hi, _mm_srli_epi16(hi, 8)), one), 8);
    return _mm_or_si128(_mm_packus_epi16(lo, hi), alpha_mask);
}

lh_void
lh_pixel_span_over_alphas_sse2_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                    lh_u32_t rgb)
{
    lh_usize_t i = 0U;

    for (; i + 4U <= count; i += 4U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, at + i),
                         lh_pixel_span_over_quad_m128(
                             _mm_loadu_si128(lh_ptr_rcast(const __m128i, at + i)),
                             lh_pixel_span_alpha_lanes_32(lh_pixel_span_load_alpha_4(alpha + i)),
                             rgb));
    }
    lh_return_if(i == count);
    lh_pixel_span_scatter_32(
        at + i, count - i,
        lh_pixel_span_over_quad_m128(
            lh_pixel_span_gather_32(at + i, count - i),
            lh_pixel_span_alpha_lanes_32(lh_pixel_span_gather_alpha(alpha + i, count - i)), rgb));
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

lh_void
lh_pixel_span_blend_vary_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count, lh_u32_t rgb)
{
    for (lh_usize_t i = 0U; i < count;)
    {
        const lh_u32_t dst = at[i];
        lh_usize_t end = i + 1U;

        while (end < count && at[end] == dst)
        {
            ++end;
        }
        if (lh_pixel_argb_get_channel(dst, 24) == LH_BYTE_T_MAX)
        {
            lh_pixel_span_over_opaque_argb(at + i, alpha + i, end - i, dst, rgb);
        }
        else
        {
            for (lh_usize_t pixel = i; pixel < end; ++pixel)
            {
                at[pixel] = lh_pixel_argb_over(dst, lh_bit_or(lh_bit_shl(alpha[pixel], 24), rgb));
            }
        }
        i = end;
    }
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
lh_void
lh_pixel_span_blend_alphas_sse2_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                     lh_u32_t rgb)
{
    lh_usize_t i = 0U;

    for (; i + 4U <= count; i += 4U)
    {
        _mm_storeu_si128(
            lh_ptr_rcast(__m128i, at + i),
            lh_pixel_span_blend_quad_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at + i)),
                                          lh_pixel_span_load_alpha_4(alpha + i), rgb));
    }
    lh_return_if(i == count);
    /* The last one to three pixels are a block too, not a scalar loop that branched
       per pixel on a coverage that is noise to the predictor: a glyph row is six to
       fourteen pixels, so this tail is part of every row a caption draws. */
    lh_pixel_span_scatter_32(
        at + i, count - i,
        lh_pixel_span_blend_quad_m128(lh_pixel_span_gather_32(at + i, count - i),
                                      lh_pixel_span_gather_alpha(alpha + i, count - i), rgb));
}

lh_void
lh_pixel_span_blend_alphas8_sse2_argb(lh_u32_t *at, lh_usize_t count, lh_u64_t alphas, lh_u32_t rgb)
{
    const __m128i a8 = _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(alphas)));
    const __m128i a8_hi = _mm_srli_si128(a8, 4);

    if (count < 4U)
    {
        lh_pixel_span_scatter_32(
            at, count, lh_pixel_span_blend_quad_m128(lh_pixel_span_gather_32(at, count), a8, rgb));
        return;
    }
    _mm_storeu_si128(
        lh_ptr_rcast(__m128i, at),
        lh_pixel_span_blend_quad_m128(_mm_loadu_si128(lh_ptr_rcast(const __m128i, at)), a8, rgb));
    if (count == 8U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, at + 4),
                         lh_pixel_span_blend_quad_m128(
                             _mm_loadu_si128(lh_ptr_rcast(const __m128i, at + 4)), a8_hi, rgb));
    }
    else if (count > 4U)
    {
        lh_pixel_span_scatter_32(
            at + 4, count - 4U,
            lh_pixel_span_blend_quad_m128(lh_pixel_span_gather_32(at + 4, count - 4U), a8_hi, rgb));
    }
}
#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

lh_void
lh_pixel_span_blend_alphas_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                lh_u32_t rgb)
{
    /* The clear pixels at either end of a row -- a glyph's margins, a corner's
       outside -- leave their words: they are not walked at all. */
    while (count > 0U && alpha[0] == 0U)
    {
        ++at;
        ++alpha;
        --count;
    }
    while (count > 0U && alpha[count - 1U] == 0U)
    {
        --count;
    }
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    const lh_bool_t sse = lh_cpu_simd_has_sse2();

    /* A short row -- a corner's, a glyph's -- is a few blocks: looking for runs in it
       costs more than the blocks do. */
    if (sse && count < LH_PIXEL_SPAN_SHORT_ROW)
    {
        lh_pixel_span_blend_alphas_sse2_argb(at, alpha, count, rgb);
        return;
    }
#endif

    for (lh_usize_t i = 0U; i < count;)
    {
        const lh_byte_t coverage = alpha[i];
        lh_usize_t end = i + 1U;

        while (end < count && alpha[end] == coverage)
        {
            ++end;
        }
        /* A run of one alpha is one colour laid over: mixed once per run of equal
           destination words. */
        if (end - i >= LH_PIXEL_SPAN_CONST_RUN)
        {
            lh_pixel_span_blend_argb(at + i, end - i, rgb, coverage);
            i = end;
            continue;
        }
        /* Short runs, as far as the next long one, go through the varying kernel. */
        lh_usize_t stretch = end;

        while (stretch < count)
        {
            const lh_byte_t next = alpha[stretch];
            lh_usize_t run = stretch + 1U;

            while (run < count && alpha[run] == next)
            {
                ++run;
            }
            if (run - stretch >= LH_PIXEL_SPAN_CONST_RUN)
            {
                break;
            }
            stretch = run;
        }
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (sse)
        {
            lh_pixel_span_blend_alphas_sse2_argb(at + i, alpha + i, stretch - i, rgb);
            i = stretch;
            continue;
        }
#endif
        lh_pixel_span_blend_vary_argb(at + i, alpha + i, stretch - i, rgb);
        i = stretch;
    }
}

lh_void
lh_pixel_span_cover_argb(lh_u32_t *at, const lh_byte_t *cover, lh_usize_t count, lh_u32_t src,
                         lh_u32_t alpha)
{
    const lh_u32_t rgb = lh_bit_and(src, 0x00FFFFFFU);

    /* An opaque colour: the coverage is the alpha, as it is. */
    if (alpha == LH_BYTE_T_MAX)
    {
        lh_pixel_span_blend_alphas_argb(at, cover, count, rgb);
        return;
    }
    lh_return_if(alpha == 0U);
    lh_byte_t scaled[LH_PIXEL_SPAN_SCALE_RUN];

    for (lh_usize_t done = 0U; done < count; done += LH_PIXEL_SPAN_SCALE_RUN)
    {
        const lh_usize_t run =
            lh_math_min(count - done, lh_cast_static(lh_usize_t, LH_PIXEL_SPAN_SCALE_RUN));

        for (lh_usize_t i = 0U; i < run; ++i)
        {
            scaled[i] =
                lh_cast_static(lh_byte_t, lh_pixel_channel_div255_round(cover[done + i] * alpha));
        }
        lh_pixel_span_blend_alphas_argb(at + done, scaled, run, rgb);
    }
}
