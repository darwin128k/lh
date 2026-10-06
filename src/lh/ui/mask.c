/**
 * @file mask.c
 * @brief Implementation of `lh/ui/mask.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bit/packed.h>
#include <lh/cast/static.h>
#include <lh/math/rescale.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/mask.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

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

lh_ui_rect_t
lh_ui_mask_get_rect(const lh_ui_mask_t *self, lh_ui_point_t origin)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), lh_ui_point_get_x(lh_addr_of(origin)), lh_ui_point_get_y(lh_addr_of(origin)),
                    lh_ui_mask_get_width(self), lh_ui_mask_get_height(self));
    return rect;
}
