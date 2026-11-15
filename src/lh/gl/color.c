/**
 * @file color.c
 * @brief 8-bit straight RGBA.
 */
#include <lh/gl/color.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/byte/limits.h>

lh_gl_color_channel_t
lh_gl_color_get_r(const lh_gl_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_gl_color_channel_t
lh_gl_color_get_g(const lh_gl_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->g;
}

lh_gl_color_channel_t
lh_gl_color_get_b(const lh_gl_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->b;
}

lh_gl_color_channel_t
lh_gl_color_get_a(const lh_gl_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->a;
}

lh_u32_t
lh_gl_color_get_word(const lh_gl_color_t *self)
{
    lh_u32_t alpha = lh_gl_color_get_a(self);
    lh_u32_t red = lh_gl_color_get_r(self);
    lh_u32_t green = lh_gl_color_get_g(self);
    lh_u32_t blue = lh_gl_color_get_b(self);

    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

lh_void
lh_gl_color_set_r(lh_gl_color_t *self, lh_gl_color_channel_t r)
{
    lh_assert_runtime_ref(self);
    self->r = r;
}

lh_void
lh_gl_color_set_g(lh_gl_color_t *self, lh_gl_color_channel_t g)
{
    lh_assert_runtime_ref(self);
    self->g = g;
}

lh_void
lh_gl_color_set_b(lh_gl_color_t *self, lh_gl_color_channel_t b)
{
    lh_assert_runtime_ref(self);
    self->b = b;
}

lh_void
lh_gl_color_set_a(lh_gl_color_t *self, lh_gl_color_channel_t a)
{
    lh_assert_runtime_ref(self);
    self->a = a;
}

lh_void
lh_gl_color_init(lh_gl_color_t *self, lh_gl_color_channel_t r, lh_gl_color_channel_t g,
                 lh_gl_color_channel_t b, lh_gl_color_channel_t a)
{
    lh_gl_color_set_r(self, r);
    lh_gl_color_set_g(self, g);
    lh_gl_color_set_b(self, b);
    lh_gl_color_set_a(self, a);
}

lh_void
lh_gl_color_over(const lh_gl_color_t *dst, const lh_gl_color_t *color, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(color);
    lh_assert_runtime_ref(out);
    lh_gl_color_channel_t src_a = lh_gl_color_get_a(color);

    if (src_a == 0 || src_a == LH_BYTE_T_MAX)
    {
        lh_ptr_deref(out) = src_a == 0 ? lh_ptr_deref(dst) : lh_ptr_deref(color);
        return;
    }
    lh_gl_color_channel_t dst_a = lh_gl_color_get_a(dst);

    if (dst_a == LH_BYTE_T_MAX)
    {
        lh_gl_color_init(
            out,
            lh_cast_static(lh_gl_color_channel_t,
                           lh_math_div_round_255(lh_gl_color_get_r(color) * src_a +
                                                     lh_gl_color_get_r(dst) *
                                                         (LH_BYTE_T_MAX - src_a))),
            lh_cast_static(lh_gl_color_channel_t,
                           lh_math_div_round_255(lh_gl_color_get_g(color) * src_a +
                                                     lh_gl_color_get_g(dst) *
                                                         (LH_BYTE_T_MAX - src_a))),
            lh_cast_static(lh_gl_color_channel_t,
                           lh_math_div_round_255(lh_gl_color_get_b(color) * src_a +
                                                     lh_gl_color_get_b(dst) *
                                                         (LH_BYTE_T_MAX - src_a))),
            LH_BYTE_T_MAX);
        return;
    }
    lh_u32_t src_w = lh_cast_static(lh_u32_t, src_a) * LH_BYTE_T_MAX;
    lh_u32_t dst_w = lh_cast_static(lh_u32_t, dst_a) * (LH_BYTE_T_MAX - src_a);
    lh_u32_t total = src_w + dst_w;

    lh_gl_color_init(
        out,
        lh_cast_static(lh_gl_color_channel_t,
                       lh_math_div_round(lh_gl_color_get_r(color) * src_w +
                                             lh_gl_color_get_r(dst) * dst_w,
                                         total)),
        lh_cast_static(lh_gl_color_channel_t,
                       lh_math_div_round(lh_gl_color_get_g(color) * src_w +
                                             lh_gl_color_get_g(dst) * dst_w,
                                         total)),
        lh_cast_static(lh_gl_color_channel_t,
                       lh_math_div_round(lh_gl_color_get_b(color) * src_w +
                                             lh_gl_color_get_b(dst) * dst_w,
                                         total)),
        lh_cast_static(lh_gl_color_channel_t, lh_math_div_round_255(total)));
}

lh_void
lh_gl_color_mix(const lh_gl_color_t *from, const lh_gl_color_t *to, lh_gl_color_channel_t frac,
                lh_gl_color_t *out)
{
    lh_assert_runtime_ref(from);
    lh_assert_runtime_ref(to);
    lh_assert_runtime_ref(out);
    lh_u32_t rest = LH_BYTE_T_MAX - frac;

    lh_gl_color_init(
        out,
        lh_cast_static(lh_gl_color_channel_t,
                       lh_math_div_round_255(lh_gl_color_get_r(from) * rest +
                                                 lh_gl_color_get_r(to) * frac)),
        lh_cast_static(lh_gl_color_channel_t,
                       lh_math_div_round_255(lh_gl_color_get_g(from) * rest +
                                                 lh_gl_color_get_g(to) * frac)),
        lh_cast_static(lh_gl_color_channel_t,
                       lh_math_div_round_255(lh_gl_color_get_b(from) * rest +
                                                 lh_gl_color_get_b(to) * frac)),
        lh_cast_static(lh_gl_color_channel_t,
                       lh_math_div_round_255(lh_gl_color_get_a(from) * rest +
                                                 lh_gl_color_get_a(to) * frac)));
}

lh_void
lh_gl_color_scale_alpha(const lh_gl_color_t *color, lh_gl_color_channel_t cover,
                        lh_gl_color_t *out)
{
    lh_assert_runtime_ref(color);
    lh_assert_runtime_ref(out);
    lh_gl_color_init(out, lh_gl_color_get_r(color), lh_gl_color_get_g(color),
                     lh_gl_color_get_b(color),
                     lh_cast_static(lh_gl_color_channel_t,
                                    lh_math_div_round_255(lh_gl_color_get_a(color) * cover)));
}
