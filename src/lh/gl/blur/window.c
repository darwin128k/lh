/**
 * @file window.c
 * @brief Sliding sum for ::lh_gl_blur_window_t.
 */

#include <lh/gl/blur/window.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>

lh_s32_t
lh_gl_blur_window_get_r(const lh_gl_blur_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_void
lh_gl_blur_window_set_r(lh_gl_blur_window_t *self, lh_s32_t r)
{
    lh_assert_runtime_ref(self);
    self->r = r;
}

lh_s32_t
lh_gl_blur_window_get_g(const lh_gl_blur_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->g;
}

lh_void
lh_gl_blur_window_set_g(lh_gl_blur_window_t *self, lh_s32_t g)
{
    lh_assert_runtime_ref(self);
    self->g = g;
}

lh_s32_t
lh_gl_blur_window_get_b(const lh_gl_blur_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->b;
}

lh_void
lh_gl_blur_window_set_b(lh_gl_blur_window_t *self, lh_s32_t b)
{
    lh_assert_runtime_ref(self);
    self->b = b;
}

lh_s32_t
lh_gl_blur_window_get_a(const lh_gl_blur_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->a;
}

lh_void
lh_gl_blur_window_set_a(lh_gl_blur_window_t *self, lh_s32_t a)
{
    lh_assert_runtime_ref(self);
    self->a = a;
}

lh_void
lh_gl_blur_window_init(lh_gl_blur_window_t *self)
{
    lh_gl_blur_window_set_r(self, 0);
    lh_gl_blur_window_set_g(self, 0);
    lh_gl_blur_window_set_b(self, 0);
    lh_gl_blur_window_set_a(self, 0);
}

lh_void
lh_gl_blur_window_take(lh_gl_blur_window_t *self, const lh_gl_color_t *pixel)
{
    lh_s32_t alpha;

    lh_assert_runtime_ref(pixel);
    alpha = lh_gl_color_get_a(pixel);
    lh_s32_t red = lh_gl_blur_window_get_r(self) + lh_gl_color_get_r(pixel) * alpha;

    lh_gl_blur_window_set_r(self, red);
    lh_s32_t green = lh_gl_blur_window_get_g(self) + lh_gl_color_get_g(pixel) * alpha;

    lh_gl_blur_window_set_g(self, green);
    lh_s32_t blue = lh_gl_blur_window_get_b(self) + lh_gl_color_get_b(pixel) * alpha;

    lh_gl_blur_window_set_b(self, blue);
    lh_gl_blur_window_set_a(self, lh_gl_blur_window_get_a(self) + alpha);
}

lh_void
lh_gl_blur_window_drop(lh_gl_blur_window_t *self, const lh_gl_color_t *pixel)
{
    lh_s32_t alpha;

    lh_assert_runtime_ref(pixel);
    alpha = lh_gl_color_get_a(pixel);
    lh_s32_t red = lh_gl_blur_window_get_r(self) - lh_gl_color_get_r(pixel) * alpha;

    lh_gl_blur_window_set_r(self, red);
    lh_s32_t green = lh_gl_blur_window_get_g(self) - lh_gl_color_get_g(pixel) * alpha;

    lh_gl_blur_window_set_g(self, green);
    lh_s32_t blue = lh_gl_blur_window_get_b(self) - lh_gl_color_get_b(pixel) * alpha;

    lh_gl_blur_window_set_b(self, blue);
    lh_gl_blur_window_set_a(self, lh_gl_blur_window_get_a(self) - alpha);
}

lh_void
lh_gl_blur_window_take_colors(lh_gl_blur_window_t *self, const lh_gl_color_t *sample,
                              lh_usize_t stride, lh_gl_scalar_t count)
{
    for (; count > 0; --count)
    {
        lh_gl_blur_window_take(self, sample);
        sample += stride;
    }
}

lh_void
lh_gl_blur_window_drop_colors(lh_gl_blur_window_t *self, const lh_gl_color_t *sample,
                              lh_usize_t stride, lh_gl_scalar_t count)
{
    for (; count > 0; --count)
    {
        lh_gl_blur_window_drop(self, sample);
        sample += stride;
    }
}

lh_gl_color_channel_t
lh_gl_blur_window_average(const lh_gl_blur_window_t *self, lh_gl_scalar_t channel,
                          lh_gl_scalar_t count)
{
    if (channel < 0 || channel > 3 || count <= 0)
    {
        return 0;
    }
    if (channel == 3)
    {
        lh_s32_t alpha = lh_math_div_round(lh_gl_blur_window_get_a(self), count);

        return lh_type_cast(lh_gl_color_channel_t, lh_math_clamp(alpha, 0, 255));
    }
    lh_s32_t sum = lh_gl_blur_window_get_b(self);

    if (channel == 0)
    {
        sum = lh_gl_blur_window_get_r(self);
    }
    else if (channel == 1)
    {
        sum = lh_gl_blur_window_get_g(self);
    }
    if (sum == 0 || lh_gl_blur_window_get_a(self) <= 0)
    {
        return 0;
    }
    if (lh_gl_blur_window_get_a(self) == 255 * count)
    {
        lh_s32_t straight = lh_math_div_round(sum / 255, count);

        return lh_type_cast(lh_gl_color_channel_t, lh_math_clamp(straight, 0, 255));
    }
    lh_s32_t weighted = lh_math_div_round(sum, lh_gl_blur_window_get_a(self));

    return lh_type_cast(lh_gl_color_channel_t, lh_math_clamp(weighted, 0, 255));
}

lh_void
lh_gl_blur_window_average_color(const lh_gl_blur_window_t *self, lh_gl_scalar_t count,
                                lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_color_set_r(out, lh_gl_blur_window_average(self, 0, count));
    lh_gl_color_set_g(out, lh_gl_blur_window_average(self, 1, count));
    lh_gl_color_set_b(out, lh_gl_blur_window_average(self, 2, count));
    lh_gl_color_set_a(out, lh_gl_blur_window_average(self, 3, count));
}

lh_void
lh_gl_blur_window_span(lh_gl_scalar_t at, lh_gl_scalar_t radius, lh_gl_scalar_t lo,
                       lh_gl_scalar_t hi, lh_gl_scalar_t *win0, lh_gl_scalar_t *win1)
{
    lh_gl_scalar_t begin = at - radius;
    lh_gl_scalar_t end = at + radius + 1;

    lh_assert_runtime_ref(win0);
    lh_assert_runtime_ref(win1);
    if (begin < lo)
    {
        begin = lo;
    }
    if (end > hi)
    {
        end = hi;
    }
    if (end < begin)
    {
        end = begin;
    }
    lh_ptr_deref(win0) = begin;
    lh_ptr_deref(win1) = end;
}
