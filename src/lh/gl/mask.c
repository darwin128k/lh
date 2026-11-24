/**
 * @file mask.c
 * @brief Implementation of `lh/gl/mask.h`.
 */
#include <lh/gl/mask.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_mask_is_bpp(lh_u32_t bpp)
{
    return lh_pixel_mask_is_bpp(bpp);
}

lh_void
lh_gl_mask_init(lh_gl_mask_t *self, lh_byte_cptr bits, lh_s32_t width, lh_s32_t height,
                lh_s32_t row_bytes, lh_byte_t bpp)
{
    lh_pixel_mask_init(self, bits, width, height, row_bytes, bpp);
}

lh_s32_t
lh_gl_mask_get_width(const lh_gl_mask_t *self)
{
    return lh_pixel_mask_get_width(self);
}

lh_s32_t
lh_gl_mask_get_height(const lh_gl_mask_t *self)
{
    return lh_pixel_mask_get_height(self);
}

lh_byte_t
lh_gl_mask_get_bpp(const lh_gl_mask_t *self)
{
    return lh_pixel_mask_get_bpp(self);
}

lh_byte_cptr
lh_gl_mask_get_row(const lh_gl_mask_t *self, lh_s32_t y)
{
    return lh_pixel_mask_get_row(self, y);
}

lh_bool_t
lh_gl_mask_has_pixel(const lh_gl_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_pixel_mask_has_pixel(self, x, y);
}

lh_u32_t
lh_gl_mask_get_sample(const lh_gl_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_pixel_mask_get_sample(self, x, y);
}

lh_byte_t
lh_gl_mask_get_coverage(const lh_gl_mask_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_pixel_mask_get_coverage(self, x, y);
}

lh_void
lh_gl_mask_coverage_run(const lh_gl_mask_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                        lh_byte_ptr out)
{
    lh_assert_runtime_ref(out);
    lh_return_if(x1 <= x0);
    lh_pixel_mask_get_row_coverage(self, y, x0, x1 - x0, out);
}

const lh_byte_t *
lh_gl_mask_get_bits(const lh_gl_mask_t *self)
{
    return lh_pixel_mask_get_bits(self);
}

lh_void
lh_gl_mask_set_bits(lh_gl_mask_t *self, const lh_byte_t *bits)
{
    lh_pixel_mask_set_bits(self, bits);
}

lh_void
lh_gl_mask_set_width(lh_gl_mask_t *self, lh_s32_t width)
{
    lh_pixel_mask_set_width(self, width);
}

lh_void
lh_gl_mask_set_height(lh_gl_mask_t *self, lh_s32_t height)
{
    lh_pixel_mask_set_height(self, height);
}

lh_s32_t
lh_gl_mask_get_row_bytes(const lh_gl_mask_t *self)
{
    return lh_pixel_mask_get_row_bytes(self);
}

lh_void
lh_gl_mask_set_row_bytes(lh_gl_mask_t *self, lh_s32_t row_bytes)
{
    lh_pixel_mask_set_row_bytes(self, row_bytes);
}

lh_void
lh_gl_mask_set_bpp(lh_gl_mask_t *self, lh_byte_t bpp)
{
    lh_pixel_mask_set_bpp(self, bpp);
}

lh_void
lh_gl_mask_get_rect(const lh_gl_mask_t *self, lh_gl_point_t origin, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), lh_gl_point_get_x(lh_addr_of(origin)),
                    lh_gl_point_get_y(lh_addr_of(origin)), lh_gl_mask_get_width(self),
                    lh_gl_mask_get_height(self));
    lh_ptr_deref(out) = rect;
}
