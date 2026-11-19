/**
 * @file mask.c
 * @brief Implementation of `lh/core/pixel/mask.h`.
 */

#include <lh/core/pixel/mask.h>

#include <lh/core/assert.h>
#include <lh/core/bit/packed.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>

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
lh_pixel_mask_get_row_coverage(const lh_pixel_mask_t *self, lh_s32_t y, lh_s32_t x, lh_s32_t count,
                               lh_byte_t *out)
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
