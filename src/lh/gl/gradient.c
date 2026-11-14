/**
 * @file gradient.c
 * @brief Mix two stored colors along `0..255`.
 */
#include <lh/gl/gradient.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/math/isqrt.h>
#include <lh/gl/gradient/sine.h>

const lh_gl_color_t *
lh_gl_gradient_get_from(const lh_gl_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->from);
}

lh_gl_color_t *
lh_gl_gradient_get_from_mut(lh_gl_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->from);
}

const lh_gl_color_t *
lh_gl_gradient_get_to(const lh_gl_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->to);
}

lh_gl_color_t *
lh_gl_gradient_get_to_mut(lh_gl_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->to);
}

lh_void
lh_gl_gradient_set_from(lh_gl_gradient_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    self->from = lh_ptr_deref(color);
}

lh_void
lh_gl_gradient_set_to(lh_gl_gradient_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    self->to = lh_ptr_deref(color);
}

lh_void
lh_gl_gradient_init(lh_gl_gradient_t *self, const lh_gl_color_t *from, const lh_gl_color_t *to)
{
    lh_gl_gradient_set_from(self, from);
    lh_gl_gradient_set_to(self, to);
}

lh_void
lh_gl_gradient_color_at(const lh_gl_gradient_t *self, lh_gl_color_channel_t frac,
                        lh_gl_color_t *out)
{
    lh_gl_color_mix(lh_gl_gradient_get_from(self), lh_gl_gradient_get_to(self), frac, out);
}

lh_gl_color_channel_t
lh_gl_gradient_linear_frac(lh_gl_scalar_t angle, lh_gl_scalar_t width, lh_gl_scalar_t height,
                           lh_gl_scalar_t x, lh_gl_scalar_t y)
{
    if (width <= 0 || height <= 0)
    {
        return 0;
    }
    lh_s64_t across = width;
    lh_s64_t down = height;
    lh_s64_t dx = 2 * lh_cast_static(lh_s64_t, x) + 1 - across;
    lh_s64_t dy = 2 * lh_cast_static(lh_s64_t, y) + 1 - down;
    lh_s64_t sine = lh_gl_gradient_sine(lh_cast_static(lh_s32_t, angle));
    lh_s64_t cosine = lh_gl_gradient_sine(lh_cast_static(lh_s32_t, angle + 90));
    lh_s64_t along = dx * sine - dy * cosine;
    lh_s64_t length = (across * lh_math_abs(sine) + down * lh_math_abs(cosine)) * 2;

    if (length == 0)
    {
        return 0;
    }
    lh_s64_t frac = ((2 * along + length) * 255) / (2 * length);

    frac = lh_math_clamp(frac, 0, 255);
    return lh_cast_static(lh_gl_color_channel_t, frac);
}

lh_gl_color_channel_t
lh_gl_gradient_radial_frac(lh_gl_scalar_t width, lh_gl_scalar_t height, lh_gl_scalar_t x,
                           lh_gl_scalar_t y)
{
    if (width <= 0 || height <= 0)
    {
        return 0;
    }
    lh_s64_t dx = x;
    dx = dx * 2 + 1 - width;
    lh_s64_t dy = y;
    dy = dy * 2 + 1 - height;
    lh_s64_t ux = dx * 1024 / width;
    lh_s64_t uy = dy * 1024 / height;
    lh_u64_t square = lh_cast_static(lh_u64_t, ux * ux) + lh_cast_static(lh_u64_t, uy * uy);
    lh_s64_t frac;

    /* 1448 is floor(1024 * sqrt(2)): the corner of this ellipse. */
    frac = lh_cast_static(lh_s64_t, lh_math_isqrt_u64(square)) * 255 / 1448;

    frac = lh_math_clamp(frac, 0, 255);
    return lh_cast_static(lh_gl_color_channel_t, frac);
}
