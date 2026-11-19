/**
 * @file shadow.c
 * @brief Implementation of `lh/gx/backend/sw/shadow.h`.
 */

#include <lh/gx/backend/sw/shadow.h>

#include <lh/core/assert.h>

const lh_byte_t *
lh_gx_backend_sw_shadow_get_bits(const lh_gx_backend_sw_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bits;
}

lh_byte_t *
lh_gx_backend_sw_shadow_get_bits_mut(lh_gx_backend_sw_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bits;
}

lh_void
lh_gx_backend_sw_shadow_set_bits(lh_gx_backend_sw_shadow_t *self, lh_byte_t *bits)
{
    lh_assert_runtime_ref(self);
    self->bits = bits;
}

lh_usize_t
lh_gx_backend_sw_shadow_get_capacity(const lh_gx_backend_sw_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_void
lh_gx_backend_sw_shadow_set_capacity(lh_gx_backend_sw_shadow_t *self, lh_usize_t capacity)
{
    lh_assert_runtime_ref(self);
    self->capacity = capacity;
}

const lh_pixel_shadow_t *
lh_gx_backend_sw_shadow_get_key(const lh_gx_backend_sw_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->key);
}

lh_void
lh_gx_backend_sw_shadow_set_key(lh_gx_backend_sw_shadow_t *self, const lh_pixel_shadow_t *key)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(key);
    self->key = lh_ptr_deref(key);
}

lh_s32_t
lh_gx_backend_sw_shadow_get_width(const lh_gx_backend_sw_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_gx_backend_sw_shadow_set_width(lh_gx_backend_sw_shadow_t *self, lh_s32_t width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

lh_s32_t
lh_gx_backend_sw_shadow_get_height(const lh_gx_backend_sw_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_gx_backend_sw_shadow_set_height(lh_gx_backend_sw_shadow_t *self, lh_s32_t height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}

lh_bool_t
lh_gx_backend_sw_shadow_is_filled(const lh_gx_backend_sw_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->filled;
}

lh_void
lh_gx_backend_sw_shadow_set_filled(lh_gx_backend_sw_shadow_t *self, lh_bool_t filled)
{
    lh_assert_runtime_ref(self);
    self->filled = filled;
}

lh_void
lh_gx_backend_sw_shadow_init(lh_gx_backend_sw_shadow_t *self, lh_byte_t *bits, lh_usize_t capacity)
{
    lh_gx_backend_sw_shadow_set_bits(self, bits);
    lh_gx_backend_sw_shadow_set_capacity(self, capacity);
    lh_gx_backend_sw_shadow_set_width(self, 0);
    lh_gx_backend_sw_shadow_set_height(self, 0);
    lh_gx_backend_sw_shadow_set_filled(self, lh_bool_false);
}

lh_bool_t
lh_gx_backend_sw_shadow_can_hold(const lh_gx_backend_sw_shadow_t *self, lh_s32_t width,
                                 lh_s32_t height)
{
    return width > 0 && height > 0 &&
           lh_cast_static(lh_usize_t, width) * lh_cast_static(lh_usize_t, height) <=
               lh_gx_backend_sw_shadow_get_capacity(self);
}

lh_bool_t
lh_gx_backend_sw_shadow_is_for(const lh_gx_backend_sw_shadow_t *self, const lh_pixel_shadow_t *key,
                               lh_s32_t width, lh_s32_t height)
{
    return lh_gx_backend_sw_shadow_is_filled(self) &&
           lh_gx_backend_sw_shadow_get_width(self) == width &&
           lh_gx_backend_sw_shadow_get_height(self) == height &&
           lh_pixel_shadow_is_equal(lh_gx_backend_sw_shadow_get_key(self), key);
}

lh_void
lh_gx_backend_sw_shadow_fill(lh_gx_backend_sw_shadow_t *self, const lh_pixel_shadow_t *key,
                             lh_s32_t x, lh_s32_t y, lh_s32_t width, lh_s32_t height)
{
    lh_pixel_shadow_ramp_t ramp;
    lh_byte_t *bits = lh_gx_backend_sw_shadow_get_bits_mut(self);

    lh_pixel_shadow_ramp_init(lh_addr_of(ramp), key);
    for (lh_s32_t row = 0; row < height; ++row)
    {
        lh_pixel_shadow_get_span(key, lh_addr_of(ramp), y + row, x, x + width,
                                 bits + lh_cast_static(lh_usize_t, row) *
                                            lh_cast_static(lh_usize_t, width));
    }
    lh_gx_backend_sw_shadow_set_key(self, key);
    lh_gx_backend_sw_shadow_set_width(self, width);
    lh_gx_backend_sw_shadow_set_height(self, height);
    lh_gx_backend_sw_shadow_set_filled(self, lh_bool_true);
}
