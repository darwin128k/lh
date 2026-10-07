/**
 * @file mask.c
 * @brief Implementation of `lh/ui/mask.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cpu/simd.h>
#include <lh/memory.h>
#include <lh/bit/packed.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/math/rescale.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/mask.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
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
lh_ui_mask_init(lh_ui_mask_t *self, const lh_byte_t *bits, lh_s32_t width, lh_s32_t height, lh_s32_t row_bytes,
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

const lh_byte_t *
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

lh_void
lh_ui_mask_coverage_run(const lh_ui_mask_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_byte_t *out)
{
    const lh_byte_t *row;
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
    /* Four bits per pixel is the baked font's format, and a glyph row is eight to
       ten pixels of it — exactly what one SSE2 pass covers, where the scalar loop
       below pays its loop bookkeeping per pixel.

       Two things make this short. The rescale is a multiply and not a division,
       because every bpp's maximum divides 255 exactly: 255 / 15 == 17, so
       (v * 255 + 7) / 15 is v * 17 for every v from 0 to 15 — no rounding is
       even in question. And the two nibbles of a byte are the two neighbouring
       pixels, so one AND, one shift and one interleave put eight samples in
       pixel order. Eight bytes are read from the middle of the row, so the pass
       only runs while eight still fit inside it and the tail takes the loop.

       Pixels are packed from the high bit down, like every packed field here:
       an even x is the high nibble of its byte and the next is the low one. An
       odd x sits on the low nibble, which puts it half a pair ahead — there the
       low nibbles lead and the high ones follow one byte later. */
    if (self->bpp == 4U)
    {
        const __m128i nibble = _mm_set1_epi8(0x0F);
        const __m128i seventeen = _mm_set1_epi16(17);

        while (x + 8 <= x1 && (x >> 1) + 8 <= self->row_bytes)
        {
            const __m128i v = _mm_loadl_epi64(lh_ptr_rcast(const __m128i, row + (x >> 1)));
            const __m128i low = _mm_and_si128(v, nibble);
            const __m128i high = _mm_and_si128(_mm_srli_epi16(v, 4), nibble);
            const __m128i pixels = (x & 1) != 0 ? _mm_unpacklo_epi8(low, _mm_srli_si128(high, 1))
                                               : _mm_unpacklo_epi8(high, low);

            _mm_storel_epi64(lh_ptr_rcast(__m128i, out + (x - x0)), _mm_mullo_epi16(pixels, seventeen));
            x += 8;
        }
    }
#endif

    {
        const lh_s32_t width = self->width;
        const lh_u32_t top = lh_bit_packed_max(self->bpp);

        for (; x < x1; ++x)
        {
            const lh_u32_t sample =
                x < width ? lh_bit_packed_get(row, lh_cast_static(lh_u32_t, x), self->bpp) : 0U;

            out[x - x0] = lh_cast_static(lh_byte_t, lh_math_rescale_u32(sample, top, 255U));
        }
    }
}

lh_ui_rect_t
lh_ui_mask_get_rect(const lh_ui_mask_t *self, lh_ui_point_t origin)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_point_get_x(lh_addr_of(origin)), lh_ui_point_get_y(lh_addr_of(origin)),
                    lh_ui_mask_get_width(self), lh_ui_mask_get_height(self));
    return rect;
}
