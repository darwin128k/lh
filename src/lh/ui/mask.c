/**
 * @file mask.c
 * @brief Implementation of `lh/ui/mask.h`.
 */

#include <lh/byte/ptr.h>
#include <lh/cpu/simd.h>
#include <lh/memory.h>
#include <lh/bit/packed.h>
#include <lh/math.h>
#include <lh/ui/mask.h>
#include <lh/util/return.h>

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
#    include <immintrin.h>
#    if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif
#endif

lh_bool_t
lh_ui_mask_is_bpp(lh_u32_t bpp)
{
    return bpp == 1U || bpp == 2U || bpp == 4U || bpp == 8U ? lh_bool_true : lh_bool_false;
}

lh_void
lh_ui_mask_init(lh_ui_mask_t *self, lh_byte_cptr bits, lh_s32_t width, lh_s32_t height, lh_s32_t row_bytes,
                lh_byte_t bpp)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(bits);
    lh_assert_runtime_ifn(lh_ui_mask_is_bpp(bpp) && width >= 0 && height >= 0,
                          lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(lh_cast_static(lh_u32_t, row_bytes) >=
                              lh_bit_packed_bytes(lh_cast_static(lh_u32_t, width), bpp),
                          lh_runtime_error_code_invalid_argument);
    self->bits = bits;
    self->width = width;
    self->height = height;
    self->row_bytes = row_bytes;
    self->bpp = bpp;
}

lh_s32_t
lh_ui_mask_get_width(const lh_ui_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_s32_t
lh_ui_mask_get_height(const lh_ui_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_byte_t
lh_ui_mask_get_bpp(const lh_ui_mask_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bpp;
}

lh_byte_cptr
lh_ui_mask_get_row(const lh_ui_mask_t *self, lh_s32_t y)
{
    lh_assert_runtime_ref(self);
    return self->bits + y * self->row_bytes;
}

lh_bool_t
lh_ui_mask_has_pixel(const lh_ui_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_assert_runtime_ref(self);
    return x >= 0 && y >= 0 && x < self->width && y < self->height ? lh_bool_true : lh_bool_false;
}

lh_u32_t
lh_ui_mask_get_sample(const lh_ui_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_return_if(!lh_ui_mask_has_pixel(self, x, y), 0U);
    return lh_bit_packed_get(lh_ui_mask_get_row(self, y), lh_cast_static(lh_u32_t, x), self->bpp);
}

lh_byte_t
lh_ui_mask_get_coverage(const lh_ui_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_cast_static(lh_byte_t, lh_math_rescale_u32(lh_ui_mask_get_sample(self, x, y),
                                                         lh_bit_packed_max(lh_ui_mask_get_bpp(self)), 255U));
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
lh_u64_t
lh_ui_mask_unpack_4_sse2(lh_u64_t bytes, lh_bool_t odd)
{
    /* The rescale is a multiply and not a division, because every bpp's maximum
       divides 255 exactly: 255 / 15 == 17, so (v * 255 + 7) / 15 is v * 17 for
       every v from 0 to 15 -- no rounding is even in question. And the two nibbles
       of a byte are the two neighbouring pixels, so one AND, one shift and one
       interleave put eight samples in pixel order.

       Pixels are packed from the high bit down, like every packed field here:
       an even x is the high nibble of its byte and the next is the low one. An
       odd x sits on the low nibble, which puts it half a pair ahead -- there the
       low nibbles lead and the high ones follow one byte later. */
    const __m128i nibble = _mm_set1_epi8(0x0F);
    const __m128i v = _mm_loadl_epi64(lh_ptr_rcast(const __m128i, lh_addr_of(bytes)));
    const __m128i low = _mm_and_si128(v, nibble);
    const __m128i high = _mm_and_si128(_mm_srli_epi16(v, 4), nibble);
    const __m128i pixels = odd ? _mm_unpacklo_epi8(low, _mm_srli_si128(high, 1)) : _mm_unpacklo_epi8(high, low);

    lh_u64_t out;

    _mm_storel_epi64(lh_ptr_rcast(__m128i, lh_addr_of(out)), _mm_mullo_epi16(pixels, _mm_set1_epi16(17)));
    return out;
}
#endif

lh_u64_t
lh_ui_mask_get_coverage_8(const lh_ui_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    lh_byte_t run[8];

    lh_assert_runtime_ref(self);
    lh_return_if(y < 0 || y >= self->height || x >= self->width || x + 8 <= 0, 0U);
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (self->bpp == 4U && x >= 0)
    {
        lh_byte_cptr row = lh_ui_mask_get_row(self, y);
        const lh_s32_t at = x >> 1;
        const lh_s32_t left = self->row_bytes - at;
        const lh_s32_t inside = self->width - x;
        lh_u64_t bytes = 0U;
        lh_u64_t cover;
        lh_s32_t i;

        /* Eight bytes are read where the row has them. A glyph row is three to
           seven, so its end is gathered byte by byte into the register -- the
           row is never read past, and the bytes are not stored to a buffer for
           the pass to load back: eight byte stores read as one load is a store
           the CPU cannot forward, and it cost more than the pass. */
        if (left >= 8)
        {
            bytes = lh_ptr_rcast(const lh_u64_t, row + at)[0];
        }
        else
        {
            for (i = left - 1; i >= 0; --i)
            {
                bytes = (bytes << 8) | row[at + i];
            }
        }
        cover = lh_ui_mask_unpack_4_sse2(bytes, (x & 1) != 0);
        /* Past the width is outside: the padding nibble of an odd width, or the
           next row's first pixels. */
        return inside >= 8 ? cover : cover & ((lh_cast_static(lh_u64_t, 1U) << (8 * inside)) - 1U);
    }
#endif
    lh_ui_mask_coverage_run(self, x, x + 8, y, run);
    return lh_ptr_rcast(const lh_u64_t, run)[0];
}

lh_void
lh_ui_mask_coverage_run(const lh_ui_mask_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_byte_ptr out)
{
    lh_byte_cptr row;
    lh_s32_t x;

    lh_assert_runtime_ref(out);
    lh_return_if(x1 <= x0);

    /* Everything that does not change along the row — the row pointer, the bits
       per pixel, and the rescale that turns a sample into a coverage — is taken
       once here. Asking lh_ui_mask_get_coverage per pixel instead, as the
       callers did, is a cross-module call per pixel of every glyph, and the
       text in a frame is nothing but such pixels.

       A run may start before the mask or end after it, and each end is its own
       answer: only the pixels outside are 0, so the head is cleared and the rest
       still answered. Zeroing the whole run because its start is outside is the
       easy wrong answer, and it was wrong until a test swept the starts. */
    x = x0;
    if (x < 0)
    {
        const lh_s32_t skip = lh_math_min(-x, x1 - x0);

        lh_memory_set(out, lh_cast_static(lh_usize_t, skip), 0U);
        x = 0;
    }
    /* A run that starts before the mask can also end before it, and then there
       is nothing left to ask about. */
    if (x >= x1)
    {
        return;
    }
    if (y < 0 || y >= self->height)
    {
        lh_memory_set(out + (x - x0), lh_cast_static(lh_usize_t, x1 - x), 0U);
        return;
    }
    row = lh_ui_mask_get_row(self, y);

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    /* Four bits per pixel is the baked font's format, and a glyph row is six to
       fourteen pixels of it: one or two reads of eight (::lh_ui_mask_get_coverage_8),
       and no pixel of a glyph falls to the scalar loop below. */
    if (self->bpp == 4U)
    {
        const lh_s32_t end = lh_math_min(x1, self->width);

        while (x < end)
        {
            const lh_s32_t count = lh_math_min(end - x, 8);
            const lh_u64_t cover = lh_ui_mask_get_coverage_8(self, x, y);
            lh_s32_t i;

            if (count == 8)
            {
                lh_ptr_rcast(lh_u64_t, out + (x - x0))[0] = cover;
            }
            else
            {
                for (i = 0; i < count; ++i)
                {
                    out[x - x0 + i] = lh_cast_static(lh_byte_t, cover >> (8 * i));
                }
            }
            x += count;
        }
    }
#endif

    {
        const lh_s32_t width = self->width;
        const lh_u32_t top = lh_bit_packed_max(self->bpp);
        /* The same rescale as above: every bpp up to eight has a maximum that divides
           255, so the division is a multiply -- a division by 15 here was once most
           of what a letter cost. With SSE2 a four-bit mask only reaches this loop
           for the pixels of a run past the mask's width. */
        const lh_u32_t step = (255U % top) == 0U ? 255U / top : 0U;

        for (; x < x1; ++x)
        {
            const lh_u32_t sample =
                x < width ? lh_bit_packed_get(row, lh_cast_static(lh_u32_t, x), self->bpp) : 0U;

            out[x - x0] = lh_cast_static(lh_byte_t, step != 0U ? sample * step : lh_math_rescale_u32(sample, top, 255U));
        }
    }
}

lh_void
lh_ui_mask_get_rect(const lh_ui_mask_t *self, lh_ui_point_t origin, lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_point_get_x(lh_addr_of(origin)), lh_ui_point_get_y(lh_addr_of(origin)),
                    lh_ui_mask_get_width(self), lh_ui_mask_get_height(self));
    *out = rect;
    return;
}
