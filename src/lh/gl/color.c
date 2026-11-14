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
                           lh_math_div_round(lh_gl_color_get_r(color) * src_a +
                                                 lh_gl_color_get_r(dst) * (LH_BYTE_T_MAX - src_a),
                                             LH_BYTE_T_MAX)),
            lh_cast_static(lh_gl_color_channel_t,
                           lh_math_div_round(lh_gl_color_get_g(color) * src_a +
                                                 lh_gl_color_get_g(dst) * (LH_BYTE_T_MAX - src_a),
                                             LH_BYTE_T_MAX)),
            lh_cast_static(lh_gl_color_channel_t,
                           lh_math_div_round(lh_gl_color_get_b(color) * src_a +
                                                 lh_gl_color_get_b(dst) * (LH_BYTE_T_MAX - src_a),
                                             LH_BYTE_T_MAX)),
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
        lh_cast_static(lh_gl_color_channel_t, lh_math_div_round(total, LH_BYTE_T_MAX)));
}
