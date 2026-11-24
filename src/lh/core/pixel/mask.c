/**
 * @file mask.c
 * @brief Implementation of `lh/core/pixel/mask.h`.
 */

#include <lh/core/pixel/mask.h>

#include <lh/core/assert.h>
#include <lh/core/bit/packed.h>
#include <lh/core/bit/swar.h>
#include <lh/core/cpu/simd.h>
#include <lh/core/math.h>
#include <lh/core/memory/word.h>
#include <lh/core/util/return.h>

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
#    include <immintrin.h>
#    if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif
#endif

lh_bool_t
lh_pixel_mask_is_bpp(lh_u32_t bpp)
{
    return bpp == 1U || bpp == 2U || bpp == 4U || bpp == 8U ? lh_bool_true : lh_bool_false;
}

lh_s32_t
lh_pixel_mask_get_width(const lh_pixel_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_s32_t
lh_pixel_mask_get_height(const lh_pixel_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_byte_t
lh_pixel_mask_get_bpp(const lh_pixel_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bpp;
}

const lh_byte_t *
lh_pixel_mask_get_bits(const lh_pixel_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bits;
}

lh_void
lh_pixel_mask_set_bits(lh_pixel_mask_t *self, const lh_byte_t *bits)
{
    lh_assert_runtime_ref(self);
    self->bits = bits;
}

lh_void
lh_pixel_mask_set_width(lh_pixel_mask_t *self, lh_s32_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_void
lh_pixel_mask_set_height(lh_pixel_mask_t *self, lh_s32_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_s32_t
lh_pixel_mask_get_row_bytes(const lh_pixel_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->row_bytes;
}

lh_void
lh_pixel_mask_set_row_bytes(lh_pixel_mask_t *self, lh_s32_t row_bytes)
{
    lh_assert_runtime_ref(self);
    self->row_bytes = row_bytes;
}

lh_void
lh_pixel_mask_set_bpp(lh_pixel_mask_t *self, lh_byte_t bpp)
{
    lh_assert_runtime_ref(self);
    self->bpp = bpp;
}

lh_u64_t
lh_pixel_mask_unpack_4_words(lh_u64_t bytes, lh_bool_t odd)
{
    /* Nibble k of the row counts from the high nibble of the lowest byte; pixel i
       is nibble i, or i + 1 when the row starts on a low nibble. Two 32-bit halves
       keep a 32-bit core off 64-bit shifts. */
    const lh_u32_t first = odd ? 1U : 0U;
    lh_u32_t half[2] = {0U, 0U};

    for (lh_u32_t i = 0U; i < 8U; ++i)
    {
        const lh_u32_t k = i + first;
        const lh_u32_t byte = lh_cast_static(lh_u32_t, bytes >> (8U * (k >> 1))) & 0xFFU;
        const lh_u32_t sample = ((k & 1U) != 0U) ? (byte & 0x0FU) : (byte >> 4);

        half[i >> 2] |= (sample * 17U) << (8U * (i & 3U));
    }
    return (lh_cast_static(lh_u64_t, half[1]) << 32) | half[0];
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
lh_u64_t
lh_pixel_mask_unpack_4_sse2(lh_u64_t bytes, lh_bool_t odd)
{
    /* The rescale is a multiply and not a division, because every depth's maximum
       divides 255 exactly: 255 / 15 == 17. And the two nibbles of a byte are two
       neighbouring pixels, so one AND, one shift and one interleave put eight
       samples in pixel order.

       Pixels are packed from the high bit down: an even x is the high nibble of its
       byte and the next is the low one. An odd x sits on the low nibble, which puts
       it half a pair ahead -- there the low nibbles lead and the high ones follow
       one byte later. */
    const __m128i nibble = _mm_set1_epi8(0x0F);
    const __m128i v = _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(bytes)));
    const __m128i low = _mm_and_si128(v, nibble);
    const __m128i high = _mm_and_si128(_mm_srli_epi16(v, 4), nibble);
    const __m128i pixels =
        odd ? _mm_unpacklo_epi8(low, _mm_srli_si128(high, 1)) : _mm_unpacklo_epi8(high, low);
    lh_u64_t out;

    _mm_storel_epi64(lh_ptr_rcast(__m128i, lh_addr_of(out)),
                     _mm_mullo_epi16(pixels, _mm_set1_epi16(17)));
    return out;
}
#endif

lh_void
lh_pixel_mask_init(lh_pixel_mask_t *self, lh_byte_cptr bits, lh_s32_t width, lh_s32_t height,
                   lh_s32_t row_bytes, lh_byte_t bpp)
{
    lh_assert_runtime_ref(bits);
    lh_assert_runtime_ifn(lh_pixel_mask_is_bpp(bpp) && width >= 0 && height >= 0,
                          lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(lh_cast_static(lh_u32_t, row_bytes) >=
                              lh_bit_packed_bytes(lh_cast_static(lh_u32_t, width), bpp),
                          lh_runtime_error_code_invalid_argument);
    lh_pixel_mask_set_bits(self, bits);
    lh_pixel_mask_set_width(self, width);
    lh_pixel_mask_set_height(self, height);
    lh_pixel_mask_set_row_bytes(self, row_bytes);
    lh_pixel_mask_set_bpp(self, bpp);
}

lh_byte_cptr
lh_pixel_mask_get_row(const lh_pixel_mask_t *self, lh_s32_t y)
{
    return lh_pixel_mask_get_bits(self) + y * lh_pixel_mask_get_row_bytes(self);
}

lh_bool_t
lh_pixel_mask_has_pixel(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    return x >= 0 && y >= 0 && x < lh_pixel_mask_get_width(self) &&
                   y < lh_pixel_mask_get_height(self)
               ? lh_bool_true
               : lh_bool_false;
}

lh_u32_t
lh_pixel_mask_get_sample(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_return_if(!lh_pixel_mask_has_pixel(self, x, y), 0U);
    return lh_bit_packed_get(lh_pixel_mask_get_row(self, y), lh_cast_static(lh_u32_t, x),
                             lh_pixel_mask_get_bpp(self));
}

lh_void
lh_pixel_mask_get_row_coverage_by_samples(const lh_pixel_mask_t *self, lh_s32_t y, lh_s32_t x,
                                          lh_s32_t count, lh_byte_t *out)
{
    const lh_s32_t width = lh_pixel_mask_get_width(self);
    const lh_u32_t bpp = lh_pixel_mask_get_bpp(self);
    const lh_u32_t scale = 255U / lh_bit_packed_max(bpp);
    const lh_bool_t inside =
        lh_cast_static(lh_bool_t, y >= 0 && y < lh_pixel_mask_get_height(self));
    const lh_byte_t *row = inside ? lh_pixel_mask_get_row(self, y) : lh_null;

    for (lh_s32_t i = 0; i < count; ++i)
    {
        const lh_s32_t column = x + i;

        out[i] = inside && column >= 0 && column < width
                     ? lh_cast_static(
                           lh_byte_t,
                           lh_bit_packed_get(row, lh_cast_static(lh_u32_t, column), bpp) * scale)
                     : 0U;
    }
}

lh_byte_t
lh_pixel_mask_get_coverage(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_cast_static(
        lh_byte_t, lh_math_rescale_u32(lh_pixel_mask_get_sample(self, x, y),
                                       lh_bit_packed_max(lh_pixel_mask_get_bpp(self)), 255U));
}

lh_u64_t
lh_pixel_mask_get_coverage_8(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_return_if(y < 0 || y >= lh_pixel_mask_get_height(self) ||
                     x >= lh_pixel_mask_get_width(self) || x + 8 <= 0,
                 0U);
    if (lh_pixel_mask_get_bpp(self) != 4U || x < 0)
    {
        lh_byte_t run[8];

        lh_pixel_mask_get_row_coverage_by_samples(self, y, x, 8, run);
        return lh_memory_load_u64(run);
    }
    const lh_byte_t *row = lh_pixel_mask_get_row(self, y);
    const lh_s32_t at = x >> 1;
    const lh_s32_t left = lh_pixel_mask_get_row_bytes(self) - at;
    const lh_s32_t inside = lh_pixel_mask_get_width(self) - x;
    lh_u64_t bytes = 0U;

    /* Eight bytes are read where the row has them. A glyph row is three to seven,
       so its end is gathered byte by byte into the register -- the row is never
       read past, and the bytes are not stored to a buffer for the pass to load
       back: eight byte stores read as one load is a store the CPU cannot forward,
       and it cost more than the pass. */
    if (left >= 8)
    {
        bytes = lh_memory_load_u64(row + at);
    }
    else
    {
        for (lh_s32_t i = left - 1; i >= 0; --i)
        {
            bytes = (bytes << 8) | row[at + i];
        }
    }
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    const lh_u64_t cover = lh_pixel_mask_unpack_4_sse2(bytes, (x & 1) != 0);
#else
    const lh_u64_t cover = lh_pixel_mask_unpack_4_words(bytes, (x & 1) != 0);
#endif

    /* Past the width is outside: the padding nibble of an odd width, or the next
       row's first pixels. */
    return inside >= 8 ? cover : cover & ((lh_cast_static(lh_u64_t, 1U) << (8 * inside)) - 1U);
}

lh_void
lh_pixel_mask_get_row_coverage(const lh_pixel_mask_t *self, lh_s32_t y, lh_s32_t x, lh_s32_t count,
                               lh_byte_t *out)
{
    lh_s32_t done = 0;

    /* A four-bit row from inside the mask, eight pixels a step; the bytes leave the
       register lowest first, pixel by pixel, so the last step writes only what was
       asked for. */
    if (lh_pixel_mask_get_bpp(self) == 4U && x >= 0)
    {
        for (; done < count; done += 8)
        {
            const lh_u64_t cover = lh_pixel_mask_get_coverage_8(self, x + done, y);
            const lh_s32_t take = lh_math_min(count - done, 8);

            /* Pixel i is byte i of the register, so on a little-endian core a whole
               step is one store. */
            if (take == 8 && !LH_BIT_SWAR_BIG_ENDIAN)
            {
                lh_memory_store_u64(out + done, cover);
                continue;
            }
            for (lh_s32_t i = 0; i < take; ++i)
            {
                out[done + i] = lh_cast_static(lh_byte_t, cover >> (8 * i));
            }
        }
        return;
    }
    lh_pixel_mask_get_row_coverage_by_samples(self, y, x, count, out);
}
