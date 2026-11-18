/**
 * @file color.c
 * @brief Implementation of `lh/gl/color.h`.
 */

#include <lh/core/assert/runtime.h>
#include <lh/gl/color.h>
#include <lh/core/pixel/argb.h>

#if LH_GL_COLOR_HAVE_SMLAD
#    include <arm_acle.h>
#    include <stdint.h>
#endif

/* -- Constructors ---------------------------------------------------------- */

lh_void
lh_gl_color_init(lh_gl_color_t *self, lh_gl_color_channel_t r, lh_gl_color_channel_t g,
                 lh_gl_color_channel_t b, lh_gl_color_channel_t a)
{
    lh_gl_color_set_r(self, r);
    lh_gl_color_set_g(self, g);
    lh_gl_color_set_b(self, b);
    lh_gl_color_set_a(self, a);
}

lh_gl_color_channel_t
lh_gl_color_hex_channel(lh_u32_t hex, lh_u32_t shift)
{
    return lh_cast_static(lh_gl_color_channel_t, lh_pixel_argb_get_channel(hex, shift));
}

lh_void
lh_gl_color_init_hex(lh_gl_color_t *self, lh_u32_t hex)
{
    lh_gl_color_init(self, lh_gl_color_hex_channel(hex, 24), lh_gl_color_hex_channel(hex, 16),
                     lh_gl_color_hex_channel(hex, 8), lh_gl_color_hex_channel(hex, 0));
}

lh_void
lh_gl_color_init_argb(lh_gl_color_t *self, lh_u32_t argb)
{
    lh_gl_color_init(self, lh_gl_color_hex_channel(argb, 16), lh_gl_color_hex_channel(argb, 8),
                     lh_gl_color_hex_channel(argb, 0), lh_gl_color_hex_channel(argb, 24));
}

lh_u32_t
lh_gl_color_get_argb(const lh_gl_color_t *self)
{
    return lh_bit_or(lh_bit_or(lh_bit_shl(lh_cast_static(lh_u32_t, lh_gl_color_get_a(self)), 24),
                               lh_bit_shl(lh_cast_static(lh_u32_t, lh_gl_color_get_r(self)), 16)),
                     lh_bit_or(lh_bit_shl(lh_cast_static(lh_u32_t, lh_gl_color_get_g(self)), 8),
                               lh_cast_static(lh_u32_t, lh_gl_color_get_b(self))));
}

lh_gl_color_channel_t
lh_gl_color_expand_bits(lh_u32_t value, lh_u32_t bits)
{
    return lh_cast_static(lh_gl_color_channel_t, lh_pixel_channel_expand_bits(value, bits));
}

lh_void
lh_gl_color_init_rgb565(lh_gl_color_t *self, lh_u32_t rgb565)
{
    lh_gl_color_init(self, lh_gl_color_expand_bits(lh_bit_and(lh_bit_shr(rgb565, 11), 0x1FU), 5U),
                     lh_gl_color_expand_bits(lh_bit_and(lh_bit_shr(rgb565, 5), 0x3FU), 6U),
                     lh_gl_color_expand_bits(lh_bit_and(rgb565, 0x1FU), 5U), LH_BYTE_T_MAX);
}

lh_u32_t
lh_gl_color_get_rgb565(const lh_gl_color_t *self)
{
    return lh_bit_or(
        lh_bit_or(lh_bit_shl(lh_bit_shr(lh_cast_static(lh_u32_t, lh_gl_color_get_r(self)), 3), 11),
                  lh_bit_shl(lh_bit_shr(lh_cast_static(lh_u32_t, lh_gl_color_get_g(self)), 2), 5)),
        lh_bit_shr(lh_cast_static(lh_u32_t, lh_gl_color_get_b(self)), 3));
}

/* -- Accessors ------------------------------------------------------------- */

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

lh_bool_t
lh_gl_color_equals(const lh_gl_color_t *self, const lh_gl_color_t *other)
{
    lh_assert_runtime_ref(other);
    return lh_cast_static(lh_bool_t,
                          lh_math_eq(lh_gl_color_get_r(self), lh_gl_color_get_r(other)) &&
                              lh_math_eq(lh_gl_color_get_g(self), lh_gl_color_get_g(other)) &&
                              lh_math_eq(lh_gl_color_get_b(self), lh_gl_color_get_b(other)) &&
                              lh_math_eq(lh_gl_color_get_a(self), lh_gl_color_get_a(other)));
}

/* -- Blend ----------------------------------------------------------------- */

lh_u32_t
lh_gl_color_over_src_weight(const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(color);
    return lh_cast_static(lh_u32_t, lh_gl_color_get_a(color)) * LH_BYTE_T_MAX;
}

lh_u32_t
lh_gl_color_over_dst_weight(const lh_gl_color_t *dst, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(color);
    return lh_cast_static(lh_u32_t, lh_gl_color_get_a(dst)) *
           (LH_BYTE_T_MAX - lh_gl_color_get_a(color));
}

lh_gl_color_channel_t
lh_gl_color_blend_channel(lh_u32_t src, lh_u32_t dst, lh_u32_t src_w, lh_u32_t dst_w)
{
    return lh_cast_static(lh_gl_color_channel_t, lh_pixel_channel_blend(src, dst, src_w, dst_w));
}

lh_gl_color_channel_t
lh_gl_color_weight_to_alpha(lh_u32_t weight)
{
    return lh_cast_static(lh_gl_color_channel_t, lh_pixel_channel_weight_to_alpha(weight));
}

lh_void
lh_gl_color_blend(const lh_gl_color_t *dst, const lh_gl_color_t *color, lh_u32_t src_w,
                  lh_u32_t dst_w, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_color_t result;
    lh_gl_color_init(
        lh_addr_of(result),
        lh_gl_color_blend_channel(lh_gl_color_get_r(color), lh_gl_color_get_r(dst), src_w, dst_w),
        lh_gl_color_blend_channel(lh_gl_color_get_g(color), lh_gl_color_get_g(dst), src_w, dst_w),
        lh_gl_color_blend_channel(lh_gl_color_get_b(color), lh_gl_color_get_b(dst), src_w, dst_w),
        lh_gl_color_weight_to_alpha(src_w + dst_w));
    lh_ptr_deref(out) = result;
}

lh_u32_t
lh_gl_color_div255_round(lh_u32_t x)
{
    return lh_pixel_channel_div255_round(x);
}

lh_u32_t
lh_gl_color_div255_round_pair(lh_u32_t x)
{
    return lh_pixel_channel_div255_round_pair(x);
}

lh_u32_t
lh_gl_color_dual_mac(lh_u32_t x, lh_u32_t y, lh_u32_t acc)
{
#if LH_GL_COLOR_HAVE_SMLAD
    return lh_cast_static(lh_u32_t,
                          __smlad(lh_cast_static(int16x2_t, x), lh_cast_static(int16x2_t, y),
                                  lh_cast_static(int32_t, acc)));
#else
    return acc + lh_bit_and(x, 0xFFFFU) * lh_bit_and(y, 0xFFFFU) +
           lh_bit_shr(x, 16) * lh_bit_shr(y, 16);
#endif
}

lh_u32_t
lh_gl_color_mix_pair(lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    return lh_pixel_channel_mix_pair(dst, src, a);
}

lh_gl_color_channel_t
lh_gl_color_over_opaque_channel(lh_u32_t src, lh_u32_t dst, lh_u32_t a)
{
#if LH_GL_COLOR_HAVE_SMLAD
    /* src * a + dst * (255 - a) is one dual multiply-accumulate: SMLAD on a core
       with the DSP extension (Cortex-M4 and up). */
    return lh_cast_static(lh_gl_color_channel_t,
                          lh_gl_color_div255_round(lh_gl_color_dual_mac(
                              lh_bit_or(src, lh_bit_shl(dst, 16)),
                              lh_bit_or(a, lh_bit_shl(LH_BYTE_T_MAX - a, 16)), 0U)));
#else
    /* Without SMLAD the two products are written out: packing them into halfwords
       only for lh_gl_color_dual_mac to unpack them again costs shifts, masks and
       ORs a channel on cores without the extension (ESP32, RISC-V). */
    return lh_cast_static(lh_gl_color_channel_t,
                          lh_gl_color_div255_round(src * a + dst * (LH_BYTE_T_MAX - a)));
#endif
}

lh_u32_t
lh_gl_color_argb_make(lh_u32_t r, lh_u32_t g, lh_u32_t b, lh_u32_t a)
{
    return lh_pixel_argb_make(r, g, b, a);
}

lh_u32_t
lh_gl_color_argb_over_opaque(lh_u32_t dst, lh_u32_t src)
{
    return lh_pixel_argb_over_opaque(dst, src);
}

lh_void
lh_gl_color_argb_over_opaque_span(lh_u32_t *at, const lh_gl_color_channel_t *alpha, lh_u32_t count,
                                  lh_u32_t dst, lh_u32_t rgb)
{
    /* Taken once: a shadow edge and a glyph row share one destination word. */
    {
        const lh_u32_t dst_rb = lh_bit_and(dst, 0x00FF00FFU);
        const lh_u32_t dst_ga = lh_bit_and(lh_bit_shr(dst, 8), 0x00FF00FFU);
        const lh_u32_t src_rb = lh_bit_and(rgb, 0x00FF00FFU);
        const lh_u32_t src_ga = lh_bit_and(lh_bit_shr(rgb, 8), 0x00FF00FFU);
        const lh_u32_t solid = lh_bit_or(0xFF000000U, rgb);

        for (lh_u32_t i = 0U; i < count; ++i)
        {
            const lh_u32_t a = alpha[i];

            if (a == 0U)
            {
                continue;
            }
            if (a == LH_BYTE_T_MAX)
            {
                at[i] = solid;
                continue;
            }
            {
                const lh_u32_t rb = lh_gl_color_mix_pair(dst_rb, src_rb, a);
                const lh_u32_t ga = lh_gl_color_mix_pair(dst_ga, src_ga, a);

                at[i] = lh_bit_or(lh_bit_or(0xFF000000U, rb), lh_bit_shl(lh_bit_and(ga, 0xFFU), 8));
            }
        }
    }
}

lh_u32_t
lh_gl_color_argb_over_translucent(lh_u32_t dst, lh_u32_t src)
{
    return lh_pixel_argb_over_translucent(dst, src);
}

lh_u32_t
lh_gl_color_argb_over(lh_u32_t dst, lh_u32_t src)
{
    return lh_pixel_argb_over(dst, src);
}

lh_void
lh_gl_color_over_opaque(const lh_gl_color_t *dst, const lh_gl_color_t *color, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_color_t result;

    lh_gl_color_init(
        lh_addr_of(result),
        lh_gl_color_over_opaque_channel(lh_gl_color_get_r(color), lh_gl_color_get_r(dst),
                                        lh_gl_color_get_a(color)),
        lh_gl_color_over_opaque_channel(lh_gl_color_get_g(color), lh_gl_color_get_g(dst),
                                        lh_gl_color_get_a(color)),
        lh_gl_color_over_opaque_channel(lh_gl_color_get_b(color), lh_gl_color_get_b(dst),
                                        lh_gl_color_get_a(color)),
        LH_BYTE_T_MAX);
    lh_ptr_deref(out) = result;
}

lh_void
lh_gl_color_over_translucent(const lh_gl_color_t *dst, const lh_gl_color_t *color,
                             lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_u32_t src_w = lh_gl_color_over_src_weight(color);
    const lh_u32_t dst_w = lh_gl_color_over_dst_weight(dst, color);
    lh_gl_color_t clear;
    lh_gl_color_init(lh_addr_of(clear), 0, 0, 0, 0);
    if (src_w + dst_w == 0U)
    {
        lh_ptr_deref(out) = clear;
    }
    else
    {
        lh_gl_color_blend(dst, color, src_w, dst_w, out);
    }
}

lh_void
lh_gl_color_over(const lh_gl_color_t *dst, const lh_gl_color_t *color, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(dst);
    if (lh_gl_color_get_a(dst) == LH_BYTE_T_MAX)
    {
        lh_gl_color_over_opaque(dst, color, out);
    }
    else
    {
        lh_gl_color_over_translucent(dst, color, out);
    }
}

lh_void
lh_gl_color_with_coverage(const lh_gl_color_t *color, lh_byte_t coverage, lh_gl_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(color);
    lh_gl_color_t result = lh_ptr_deref(color);
    lh_gl_color_set_a(
        lh_addr_of(result),
        lh_cast_static(lh_gl_color_channel_t,
                       lh_math_div_round(lh_gl_color_get_a(color) * coverage, LH_BYTE_T_MAX)));
    lh_ptr_deref(out) = result;
}
