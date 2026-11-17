/**
 * @file pixmap.c
 * @brief Implementation of `lh/gx/pixmap.h`.
 */

#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>
#include <lh/gx/pixmap.h>

lh_u32_t
lh_gx_pixmap_bytes_per_pixel(lh_gx_pixmap_format_t format)
{
    switch (format)
    {
        case lh_gx_pixmap_format_argb8888:
            return 4u;
        case lh_gx_pixmap_format_rgb565:
        case lh_gx_pixmap_format_rgb565_swapped:
            return 2u;
        default:
            return 0u;
    }
}

lh_void
lh_gx_pixmap_init(lh_gx_pixmap_t *self, const lh_memory_bounds_t *bits, lh_u32_t width,
                  lh_u32_t height, lh_gx_pixmap_format_t format)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(bits);

    lh_gx_pixmap_set_bits(self, bits);
    lh_gx_pixmap_set_format(self, format);
    lh_gx_pixmap_set_width(self, width);
    lh_gx_pixmap_set_height(self, height);
}

lh_void
lh_gx_pixmap_init_stride(lh_gx_pixmap_t *self, const lh_memory_bounds_t *bits, lh_u32_t width,
                         lh_u32_t height, lh_u32_t stride, lh_gx_pixmap_format_t format)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(bits);

    lh_gx_pixmap_set_bits(self, bits);
    lh_gx_pixmap_set_format(self, format);
    lh_gx_pixmap_set_width(self, width);
    lh_gx_pixmap_set_stride(self, stride);
    lh_gx_pixmap_set_height(self, height);
}

const lh_memory_bounds_t *
lh_gx_pixmap_get_bits(const lh_gx_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->bits);
}

lh_void
lh_gx_pixmap_set_bits(lh_gx_pixmap_t *self, const lh_memory_bounds_t *bits)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(bits);
    lh_memory_bounds_assign(lh_addr_of(self->bits), bits);
}

lh_u32_t
lh_gx_pixmap_get_width(const lh_gx_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_gx_pixmap_set_width(lh_gx_pixmap_t *self, lh_u32_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
    self->stride = width * lh_gx_pixmap_bytes_per_pixel(self->format);
}

lh_u32_t
lh_gx_pixmap_get_height(const lh_gx_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_gx_pixmap_set_height(lh_gx_pixmap_t *self, lh_u32_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_u32_t
lh_gx_pixmap_get_stride(const lh_gx_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stride;
}

lh_void
lh_gx_pixmap_set_stride(lh_gx_pixmap_t *self, lh_u32_t stride)
{
    lh_assert_runtime_ref(self);
    self->stride = stride;
}

lh_gx_pixmap_format_t
lh_gx_pixmap_get_format(const lh_gx_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->format;
}

lh_void
lh_gx_pixmap_set_format(lh_gx_pixmap_t *self, lh_gx_pixmap_format_t format)
{
    lh_assert_runtime_ref(self);
    self->format = format;
}

lh_u64_t
lh_gx_pixmap_size_bytes(const lh_gx_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return (lh_u64_t)lh_gx_pixmap_get_stride(self) * (lh_u64_t)lh_gx_pixmap_get_height(self);
}

lh_bool_t
lh_gx_pixmap_is_valid(const lh_gx_pixmap_t *self)
{
    const lh_memory_bounds_t *bits;

    lh_assert_runtime_ref(self);
    bits = lh_gx_pixmap_get_bits(self);
    if (lh_gx_pixmap_get_width(self) == 0u || lh_gx_pixmap_get_height(self) == 0u)
    {
        return lh_bool_false;
    }
    return (lh_u64_t)lh_memory_bounds_get_size(bits) >= lh_gx_pixmap_size_bytes(self)
               ? lh_bool_true
               : lh_bool_false;
}
