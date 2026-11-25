/**
 * @file pixmap.c
 * @brief Implementation of `lh/gl/pixmap.h`: the pixmap itself: its fields (level 1), init, the
 * format and the geometry.
 */

#include <lh/gl/pixmap.h>

#include <lh/core/assert.h>

lh_bool_t
lh_gl_pixmap_format_is_16(lh_gl_pixmap_format_t format)
{
    return format == lh_gl_pixmap_format_rgb565 || format == lh_gl_pixmap_format_rgb565_swapped
               ? lh_bool_true
               : lh_bool_false;
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

lh_s32_t
lh_gl_pixmap_format_get_bytes(lh_gl_pixmap_format_t format)
{
    return lh_gl_pixmap_format_is_16(format) ? 2 : 4;
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

lh_s32_t
lh_gl_pixmap_get_pixel_bytes(const lh_gl_pixmap_t *self)
{
    return lh_gl_pixmap_format_get_bytes(lh_gl_pixmap_get_format(self));
}

lh_void
lh_gl_pixmap_init_empty(lh_gl_pixmap_t *self)
{
    lh_gl_pixmap_init(self, lh_null, 0, 0, 0, lh_gl_pixmap_format_argb8888);
}

lh_byte_ptr
lh_gl_pixmap_get_address(const lh_gl_pixmap_t *self, lh_s32_t x, lh_s32_t y)
{
    return lh_gl_pixmap_get_row(self, y) + x * lh_gl_pixmap_get_pixel_bytes(self);
}
