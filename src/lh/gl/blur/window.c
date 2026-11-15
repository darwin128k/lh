/**
 * @file window.c
 * @brief Sliding sum for ::lh_gl_blur_window_t.
 */

#include <lh/gl/blur/window.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>

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
lh_gl_blur_window_average_sum(lh_s32_t sum, lh_s32_t alpha_sum, lh_gl_scalar_t count,
                              lh_bool_t is_alpha)
{
    if (count <= 0)
    {
        return 0;
    }
    if (is_alpha == lh_bool_true)
    {
        lh_s32_t alpha = lh_math_div_round(sum, count);

        return lh_type_cast(lh_gl_color_channel_t, lh_math_clamp(alpha, 0, 255));
    }
    if (sum == 0 || alpha_sum <= 0)
    {
        return 0;
    }
    if (alpha_sum == 255 * count)
    {
        lh_s32_t straight = lh_math_div_round(sum / 255, count);

        return lh_type_cast(lh_gl_color_channel_t, lh_math_clamp(straight, 0, 255));
    }
    lh_s32_t weighted = lh_math_div_round(sum, alpha_sum);

    return lh_type_cast(lh_gl_color_channel_t, lh_math_clamp(weighted, 0, 255));
}

lh_gl_color_channel_t
lh_gl_blur_window_average(const lh_gl_blur_window_t *self, lh_gl_scalar_t channel,
                          lh_gl_scalar_t count)
{
    if (channel < 0 || channel > 3)
    {
        return 0;
    }
    if (channel == 3)
    {
        return lh_gl_blur_window_average_sum(lh_gl_blur_window_get_a(self),
                                             lh_gl_blur_window_get_a(self), count, lh_bool_true);
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
    return lh_gl_blur_window_average_sum(sum, lh_gl_blur_window_get_a(self), count, lh_bool_false);
}

lh_gl_color_channel_t
lh_gl_blur_window_average_opaque(lh_s32_t sum, lh_gl_scalar_t count)
{
    if (count <= 0 || sum <= 0)
    {
        return 0;
    }

    /* (sum * 0x80808081) >> 39 is sum / 255 for every sum up to 255*255*400. */
    lh_s32_t straight = lh_cast_static(
        lh_s32_t, (lh_cast_static(lh_u64_t, sum) * lh_cast_static(lh_u64_t, 0x80808081)) >> 39);

    if (count > 400)
    {
        straight = sum / 255;
    }
    straight = lh_math_div_round(straight, count);
    return lh_type_cast(lh_gl_color_channel_t, lh_math_clamp(straight, 0, 255));
}

lh_void
lh_gl_blur_window_write_average(lh_s32_t red, lh_s32_t green, lh_s32_t blue, lh_s32_t alpha,
                                lh_gl_scalar_t count, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    if (count > 0 && count <= 400 && alpha == 255 * count)
    {
        lh_gl_color_set_a(out, 255);
        lh_gl_color_set_r(out, lh_gl_blur_window_average_opaque(red, count));
        lh_gl_color_set_g(out, lh_gl_blur_window_average_opaque(green, count));
        lh_gl_color_set_b(out, lh_gl_blur_window_average_opaque(blue, count));
        return;
    }
    lh_gl_color_set_r(out, lh_gl_blur_window_average_sum(red, alpha, count, lh_bool_false));
    lh_gl_color_set_g(out, lh_gl_blur_window_average_sum(green, alpha, count, lh_bool_false));
    lh_gl_color_set_b(out, lh_gl_blur_window_average_sum(blue, alpha, count, lh_bool_false));
    lh_gl_color_set_a(out, lh_gl_blur_window_average_sum(alpha, alpha, count, lh_bool_true));
}

lh_void
lh_gl_blur_window_round_factor(lh_gl_scalar_t count, lh_u64_t *mul, lh_gl_scalar_t *shift)
{
    lh_assert_runtime_ref(mul);
    lh_assert_runtime_ref(shift);
    lh_assert_runtime(count > 0 && count <= 400, lh_runtime_error_code_out_of_range);
    lh_gl_scalar_t bits = 0;

    if (count > 1)
    {
        lh_gl_scalar_t value = count - 1;

        while (value > 0)
        {
            value >>= 1;
            ++bits;
        }
    }
    lh_gl_scalar_t places = bits + 17;
    lh_u64_t scale = lh_cast_static(lh_u64_t, 1) << places;

    lh_ptr_deref(shift) = places;
    lh_ptr_deref(mul) =
        (scale + lh_cast_static(lh_u64_t, count) - 1U) / lh_cast_static(lh_u64_t, count);
}

lh_gl_color_channel_t
lh_gl_blur_window_round_apply(lh_s32_t sum, lh_u64_t mul, lh_gl_scalar_t shift, lh_gl_scalar_t add)
{
    if (sum <= 0)
    {
        return 0;
    }
    lh_u64_t numer = lh_cast_static(lh_u64_t, sum) + lh_cast_static(lh_u64_t, add);
    lh_s32_t value = lh_cast_static(lh_s32_t, (numer * mul) >> shift);

    return lh_type_cast(lh_gl_color_channel_t, lh_math_clamp(value, 0, 255));
}

lh_void
lh_gl_blur_window_write_straight(lh_s32_t red, lh_s32_t green, lh_s32_t blue, lh_u64_t mul,
                                 lh_gl_scalar_t shift, lh_gl_scalar_t add, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_color_set_a(out, 255);
    lh_gl_color_set_r(out, lh_gl_blur_window_round_apply(red, mul, shift, add));
    lh_gl_color_set_g(out, lh_gl_blur_window_round_apply(green, mul, shift, add));
    lh_gl_color_set_b(out, lh_gl_blur_window_round_apply(blue, mul, shift, add));
}

lh_void
lh_gl_blur_window_average_color(const lh_gl_blur_window_t *self, lh_gl_scalar_t count,
                                lh_gl_color_t *out)
{
    lh_gl_blur_window_write_average(lh_gl_blur_window_get_r(self), lh_gl_blur_window_get_g(self),
                                    lh_gl_blur_window_get_b(self), lh_gl_blur_window_get_a(self),
                                    count, out);
}

lh_void
lh_gl_blur_window_slide(const lh_gl_color_t *samples, lh_usize_t stride, lh_gl_scalar_t lo,
                        lh_gl_scalar_t hi, lh_gl_scalar_t radius, lh_gl_scalar_t first,
                        lh_gl_scalar_t count, lh_gl_color_t *out, lh_usize_t out_stride)
{
    lh_return_if(count <= 0 || lh_ptr_is_null(samples) || lh_ptr_is_null(out));
    lh_gl_scalar_t win0;
    lh_gl_scalar_t win1;

    lh_gl_blur_window_span(first, radius, lo, hi, lh_addr_of(win0), lh_addr_of(win1));
    lh_s32_t red = 0;
    lh_s32_t green = 0;
    lh_s32_t blue = 0;
    lh_s32_t alpha = 0;

    for (lh_gl_scalar_t i = win0; i < win1; ++i)
    {
        const lh_gl_color_t *pixel = samples + lh_type_cast(lh_usize_t, i - lo) * stride;
        lh_s32_t weight = lh_gl_color_get_a(pixel);

        red += lh_gl_color_get_r(pixel) * weight;
        green += lh_gl_color_get_g(pixel) * weight;
        blue += lh_gl_color_get_b(pixel) * weight;
        alpha += weight;
    }
    lh_gl_scalar_t head = lo + radius - first;
    lh_gl_scalar_t body = hi - radius - first;
    lh_gl_scalar_t full = radius * 2 + 1;

    if (head < 0)
    {
        head = 0;
    }
    if (head > count)
    {
        head = count;
    }
    if (body < head)
    {
        body = head;
    }
    if (body > count)
    {
        body = count;
    }
    for (lh_gl_scalar_t n = 0; n < count;)
    {
        if (radius > 0 && n == head && n + 1 < body && win1 - win0 == full)
        {
            lh_bool_t straight = lh_bool_false;

            if (full <= 400 && alpha == 255 * full)
            {
                straight = lh_bool_true;
                red /= 255;
                green /= 255;
                blue /= 255;
            }
            lh_u64_t mul = 0;
            lh_gl_scalar_t places = 0;
            lh_gl_scalar_t add = full / 2;

            if (straight == lh_bool_true)
            {
                lh_gl_blur_window_round_factor(full, lh_addr_of(mul), lh_addr_of(places));
            }
            for (; n + 1 < body; ++n)
            {
                const lh_gl_color_t *leaving =
                    samples + lh_type_cast(lh_usize_t, win0 - lo) * stride;
                const lh_gl_color_t *entering =
                    samples + lh_type_cast(lh_usize_t, win1 - lo) * stride;
                lh_s32_t leave = lh_gl_color_get_a(leaving);
                lh_s32_t enter = lh_gl_color_get_a(entering);

                if (straight == lh_bool_true && (leave != 255 || enter != 255))
                {
                    red *= 255;
                    green *= 255;
                    blue *= 255;
                    alpha = 255 * full;
                    straight = lh_bool_false;
                }
                if (straight == lh_bool_true)
                {
                    lh_gl_blur_window_write_straight(red, green, blue, mul, places, add, out);
                    out += out_stride;
                    red -= lh_gl_color_get_r(leaving);
                    green -= lh_gl_color_get_g(leaving);
                    blue -= lh_gl_color_get_b(leaving);
                    red += lh_gl_color_get_r(entering);
                    green += lh_gl_color_get_g(entering);
                    blue += lh_gl_color_get_b(entering);
                    ++win0;
                    ++win1;
                    continue;
                }
                lh_gl_blur_window_write_average(red, green, blue, alpha, full, out);
                out += out_stride;
                red -= lh_gl_color_get_r(leaving) * leave;
                green -= lh_gl_color_get_g(leaving) * leave;
                blue -= lh_gl_color_get_b(leaving) * leave;
                alpha -= leave;
                red += lh_gl_color_get_r(entering) * enter;
                green += lh_gl_color_get_g(entering) * enter;
                blue += lh_gl_color_get_b(entering) * enter;
                alpha += enter;
                ++win0;
                ++win1;
            }
            if (straight == lh_bool_true)
            {
                red *= 255;
                green *= 255;
                blue *= 255;
                alpha = 255 * full;
            }
            continue;
        }

        lh_gl_blur_window_write_average(red, green, blue, alpha, win1 - win0, out);
        out += out_stride;

        lh_gl_scalar_t next0;
        lh_gl_scalar_t next1;

        lh_gl_blur_window_span(first + n + 1, radius, lo, hi, lh_addr_of(next0),
                               lh_addr_of(next1));
        for (; win0 < next0; ++win0)
        {
            const lh_gl_color_t *pixel = samples + lh_type_cast(lh_usize_t, win0 - lo) * stride;
            lh_s32_t weight = lh_gl_color_get_a(pixel);

            red -= lh_gl_color_get_r(pixel) * weight;
            green -= lh_gl_color_get_g(pixel) * weight;
            blue -= lh_gl_color_get_b(pixel) * weight;
            alpha -= weight;
        }
        for (; win1 < next1; ++win1)
        {
            const lh_gl_color_t *pixel = samples + lh_type_cast(lh_usize_t, win1 - lo) * stride;
            lh_s32_t weight = lh_gl_color_get_a(pixel);

            red += lh_gl_color_get_r(pixel) * weight;
            green += lh_gl_color_get_g(pixel) * weight;
            blue += lh_gl_color_get_b(pixel) * weight;
            alpha += weight;
        }
        ++n;
    }
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
