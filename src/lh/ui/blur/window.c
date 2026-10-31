#include <lh/ui/blur/window.h>
#include <lh/math.h>
#include <lh/ui/blur.h>
#include <lh/ui/radius.h>
#include <lh/util/return.h>
#include <lh/assert.h>

lh_void
lh_ui_blur_window_take(lh_ui_blur_window_t *self, const lh_ui_color_t *pixel)
{
    const lh_s32_t a = lh_ui_color_get_a(pixel);

    lh_assert_runtime_ref(pixel);
    /* Colour times alpha, not the colour alone: a pixel nobody can see still
       holds bytes, and those bytes are not part of the picture. */
    lh_ui_blur_window_set_r(self, lh_ui_blur_window_get_r(self) + (lh_ui_color_get_r(pixel) * a));
    lh_ui_blur_window_set_g(self, lh_ui_blur_window_get_g(self) + (lh_ui_color_get_g(pixel) * a));
    lh_ui_blur_window_set_b(self, lh_ui_blur_window_get_b(self) + (lh_ui_color_get_b(pixel) * a));
    lh_ui_blur_window_set_a(self, lh_ui_blur_window_get_a(self) + (a));
}

lh_void
lh_ui_blur_window_drop(lh_ui_blur_window_t *self, const lh_ui_color_t *pixel)
{
    const lh_s32_t a = lh_ui_color_get_a(pixel);

    lh_assert_runtime_ref(pixel);
    lh_ui_blur_window_set_r(self, lh_ui_blur_window_get_r(self) - (lh_ui_color_get_r(pixel) * a));
    lh_ui_blur_window_set_g(self, lh_ui_blur_window_get_g(self) - (lh_ui_color_get_g(pixel) * a));
    lh_ui_blur_window_set_b(self, lh_ui_blur_window_get_b(self) - (lh_ui_color_get_b(pixel) * a));
    lh_ui_blur_window_set_a(self, lh_ui_blur_window_get_a(self) - (a));
}

lh_u8_t
lh_ui_blur_window_average(const lh_ui_blur_window_t *self, lh_s32_t channel, lh_s32_t count)
{
    lh_s32_t sum;

    lh_return_if(channel < 0 || channel > 3 || count <= 0, lh_cast_static(lh_u8_t, 0));
    if (channel == 3)
    {
        return lh_cast_static(
            lh_u8_t,
            lh_math_clamp(lh_math_div_round(lh_ui_blur_window_get_a(self), count), 0, 255));
    }
    switch (channel)
    {
    case 0:
        sum = lh_ui_blur_window_get_r(self);
        break;
    case 1:
        sum = lh_ui_blur_window_get_g(self);
        break;
    default:
        sum = lh_ui_blur_window_get_b(self);
        break;
    }
    lh_return_if(sum == 0 || lh_ui_blur_window_get_a(self) <= 0, lh_cast_static(lh_u8_t, 0));
    /* Every pixel opaque is the picture a frame actually blurs, and that average
       was already (sum of the channel + count / 2) / count. Dividing the
       premultiplied sum by the alpha sum rounds the other way once the window
       holds an odd number of opaque pixels, so that case stays on the old
       divisor. Anything else divides the weights back out by the alpha. */
    if (lh_ui_blur_window_get_a(self) == 255 * count)
    {
        return lh_cast_static(lh_u8_t, lh_math_clamp(lh_math_div_round(sum / 255, count), 0, 255));
    }
    return lh_cast_static(
        lh_u8_t, lh_math_clamp(lh_math_div_round(sum, lh_ui_blur_window_get_a(self)), 0, 255));
}

lh_void
lh_ui_blur_window_take_samples(lh_ui_blur_window_t *self, const lh_u8_t *sample, lh_usize_t stride,
                               lh_s32_t count)
{
    lh_ui_color_t pixel;

    for (; count > 0; --count, sample += stride)
    {
        lh_ui_blur_sample_color(sample, lh_addr_of(pixel));
        lh_ui_blur_window_take(self, lh_addr_of(pixel));
    }
}

lh_void
lh_ui_blur_window_drop_samples(lh_ui_blur_window_t *self, const lh_u8_t *sample, lh_usize_t stride,
                               lh_s32_t count)
{
    lh_ui_color_t pixel;

    for (; count > 0; --count, sample += stride)
    {
        lh_ui_blur_sample_color(sample, lh_addr_of(pixel));
        lh_ui_blur_window_drop(self, lh_addr_of(pixel));
    }
}

lh_void
lh_ui_blur_window_average_color(const lh_ui_blur_window_t *self, lh_s32_t count, lh_ui_color_t *out)
{
    lh_ui_color_init(
        out, lh_ui_blur_window_average(self, 0, count), lh_ui_blur_window_average(self, 1, count),
        lh_ui_blur_window_average(self, 2, count), lh_ui_blur_window_average(self, 3, count));
}

lh_void
lh_ui_blur_window_span(lh_s32_t at, lh_s32_t radius, lh_s32_t lo, lh_s32_t hi, lh_s32_t *win0,
                       lh_s32_t *win1)
{
    lh_assert_runtime_ref(win0);
    lh_assert_runtime_ref(win1);
    lh_ptr_deref(win0) = lh_math_max(at - radius, lo);
    lh_ptr_deref(win1) = lh_math_min(at + radius + 1, hi);
}

lh_s32_t
lh_ui_blur_window_get_r(const lh_ui_blur_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_void
lh_ui_blur_window_set_r(lh_ui_blur_window_t *self, lh_s32_t r)
{
    lh_assert_runtime_ref(self);
    self->r = r;
}

lh_s32_t
lh_ui_blur_window_get_g(const lh_ui_blur_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->g;
}

lh_void
lh_ui_blur_window_set_g(lh_ui_blur_window_t *self, lh_s32_t g)
{
    lh_assert_runtime_ref(self);
    self->g = g;
}

lh_s32_t
lh_ui_blur_window_get_b(const lh_ui_blur_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->b;
}

lh_void
lh_ui_blur_window_set_b(lh_ui_blur_window_t *self, lh_s32_t b)
{
    lh_assert_runtime_ref(self);
    self->b = b;
}

lh_s32_t
lh_ui_blur_window_get_a(const lh_ui_blur_window_t *self)
{
    lh_assert_runtime_ref(self);
    return self->a;
}

lh_void
lh_ui_blur_window_set_a(lh_ui_blur_window_t *self, lh_s32_t a)
{
    lh_assert_runtime_ref(self);
    self->a = a;
}
