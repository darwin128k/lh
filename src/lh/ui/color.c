/**
 * @file color.c
 * @brief Implementation of `lh/ui/color.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/byte/limits.h>
#include <lh/math.h>
#include <lh/ui/color.h>
#include <lh/util/return.h>

#if LH_UI_COLOR_HAVE_SMLAD
#    include <arm_acle.h>
#    include <stdint.h>
#endif

/* -- Constructors ---------------------------------------------------------- */

lh_void
lh_ui_color_init(lh_ui_color_t *self, lh_ui_color_channel_t r, lh_ui_color_channel_t g,
                 lh_ui_color_channel_t b, lh_ui_color_channel_t a)
{
    lh_assert_runtime_ref(self);
    lh_ui_color_set_r(self, r);
    lh_ui_color_set_g(self, g);
    lh_ui_color_set_b(self, b);
    lh_ui_color_set_a(self, a);
}

lh_ui_color_channel_t
lh_ui_color_hex_channel(lh_u32_t hex, lh_u32_t shift)
{
    return lh_cast_static(lh_ui_color_channel_t,
                          (hex >> shift) & lh_cast_static(lh_u32_t, LH_BYTE_T_MAX));
}

lh_void
lh_ui_color_init_hex(lh_ui_color_t *self, lh_u32_t hex)
{
    lh_ui_color_init(self, lh_ui_color_hex_channel(hex, 24), lh_ui_color_hex_channel(hex, 16),
                     lh_ui_color_hex_channel(hex, 8), lh_ui_color_hex_channel(hex, 0));
}

lh_void
lh_ui_color_init_argb(lh_ui_color_t *self, lh_u32_t argb)
{
    lh_ui_color_init(self, lh_ui_color_hex_channel(argb, 16), lh_ui_color_hex_channel(argb, 8),
                     lh_ui_color_hex_channel(argb, 0), lh_ui_color_hex_channel(argb, 24));
}

lh_u32_t
lh_ui_color_get_argb(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return (lh_cast_static(lh_u32_t, lh_ui_color_get_a(self)) << 24) | (lh_cast_static(lh_u32_t, lh_ui_color_get_r(self)) << 16) |
           (lh_cast_static(lh_u32_t, lh_ui_color_get_g(self)) << 8) | lh_cast_static(lh_u32_t, lh_ui_color_get_b(self));
}

lh_ui_color_channel_t
lh_ui_color_expand_bits(lh_u32_t value, lh_u32_t bits)
{
    /* Replicate the top bits into the bottom: 0 → 0, all ones → 255. */
    return lh_cast_static(lh_ui_color_channel_t,
                          (value << (8U - bits)) | (value >> (2U * bits - 8U)));
}

lh_void
lh_ui_color_init_rgb565(lh_ui_color_t *self, lh_u32_t rgb565)
{
    lh_ui_color_init(self, lh_ui_color_expand_bits((rgb565 >> 11) & 0x1FU, 5U),
                     lh_ui_color_expand_bits((rgb565 >> 5) & 0x3FU, 6U),
                     lh_ui_color_expand_bits(rgb565 & 0x1FU, 5U), LH_BYTE_T_MAX);
}

lh_u32_t
lh_ui_color_get_rgb565(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return ((lh_cast_static(lh_u32_t, lh_ui_color_get_r(self)) >> 3) << 11) |
           ((lh_cast_static(lh_u32_t, lh_ui_color_get_g(self)) >> 2) << 5) |
           (lh_cast_static(lh_u32_t, lh_ui_color_get_b(self)) >> 3);
}

/* -- Accessors ------------------------------------------------------------- */

lh_ui_color_channel_t
lh_ui_color_get_r(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_ui_color_channel_t
lh_ui_color_get_g(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->g;
}

lh_ui_color_channel_t
lh_ui_color_get_b(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->b;
}

lh_ui_color_channel_t
lh_ui_color_get_a(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->a;
}

lh_void
lh_ui_color_set_r(lh_ui_color_t *self, lh_ui_color_channel_t r)
{
    lh_assert_runtime_ref(self);
    self->r = r;
}

lh_void
lh_ui_color_set_g(lh_ui_color_t *self, lh_ui_color_channel_t g)
{
    lh_assert_runtime_ref(self);
    self->g = g;
}

lh_void
lh_ui_color_set_b(lh_ui_color_t *self, lh_ui_color_channel_t b)
{
    lh_assert_runtime_ref(self);
    self->b = b;
}

lh_void
lh_ui_color_set_a(lh_ui_color_t *self, lh_ui_color_channel_t a)
{
    lh_assert_runtime_ref(self);
    self->a = a;
}

lh_bool_t
lh_ui_color_equals(const lh_ui_color_t *self, const lh_ui_color_t *other)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);
    return lh_cast_static(lh_bool_t,
                          lh_math_eq(lh_ui_color_get_r(self), lh_ui_color_get_r(other)) && lh_math_eq(lh_ui_color_get_g(self), lh_ui_color_get_g(other)) &&
                              lh_math_eq(lh_ui_color_get_b(self), lh_ui_color_get_b(other)) && lh_math_eq(lh_ui_color_get_a(self), lh_ui_color_get_a(other)));
}

/* -- Blend ----------------------------------------------------------------- */

lh_u32_t
lh_ui_color_over_src_weight(const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(color);
    return lh_cast_static(lh_u32_t, lh_ui_color_get_a(color)) * LH_BYTE_T_MAX;
}

lh_u32_t
lh_ui_color_over_dst_weight(const lh_ui_color_t *dst, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(color);
    return lh_cast_static(lh_u32_t, lh_ui_color_get_a(dst)) * (LH_BYTE_T_MAX - lh_ui_color_get_a(color));
}

lh_ui_color_channel_t
lh_ui_color_blend_channel(lh_u32_t src, lh_u32_t dst, lh_u32_t src_w, lh_u32_t dst_w)
{
    const lh_u32_t total = src_w + dst_w;
    return lh_cast_static(lh_ui_color_channel_t,
                          lh_math_div_round(src * src_w + dst * dst_w, total));
}

lh_ui_color_channel_t
lh_ui_color_weight_to_alpha(lh_u32_t weight)
{
    return lh_cast_static(lh_ui_color_channel_t, lh_math_div_round(weight, LH_BYTE_T_MAX));
}

lh_void
lh_ui_color_blend(const lh_ui_color_t *dst, const lh_ui_color_t *color, lh_u32_t src_w,
                  lh_u32_t dst_w, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_color_t result;
    lh_ui_color_init(lh_addr_of(result), lh_ui_color_blend_channel(lh_ui_color_get_r(color), lh_ui_color_get_r(dst), src_w, dst_w),
                     lh_ui_color_blend_channel(lh_ui_color_get_g(color), lh_ui_color_get_g(dst), src_w, dst_w),
                     lh_ui_color_blend_channel(lh_ui_color_get_b(color), lh_ui_color_get_b(dst), src_w, dst_w),
                     lh_ui_color_weight_to_alpha(src_w + dst_w));
    lh_ptr_deref(out) = result;
}

lh_u32_t
lh_ui_color_div255_round(lh_u32_t x)
{
    /* (x + 127) / 255 for x up to 255 * 255, with no divide: 255 is odd, so x / 255
       never ties at .5 and rounding to nearest is t = x + 128, (t + t / 256) / 256.
       (x + 1 + (x >> 8)) >> 8 is the truncating form, not this one. */
    const lh_u32_t t = x + 128U;

    return (t + (t >> 8)) >> 8;
}

lh_u32_t
lh_ui_color_div255_round_pair(lh_u32_t x)
{
    /* ::lh_ui_color_div255_round in each 16-bit lane at once. A lane holds at most
       255 * 255 + 128, so t + (t >> 8) stays below 2^16 and never carries into the
       lane above; the mask drops what the shift brings down from it. */
    const lh_u32_t t = x + 0x00800080U;

    return ((t + ((t >> 8) & 0x00FF00FFU)) >> 8) & 0x00FF00FFU;
}

lh_u32_t
lh_ui_color_dual_mac(lh_u32_t x, lh_u32_t y, lh_u32_t acc)
{
#if LH_UI_COLOR_HAVE_SMLAD
    return lh_cast_static(lh_u32_t,
                          __smlad(lh_cast_static(int16x2_t, x), lh_cast_static(int16x2_t, y),
                                  lh_cast_static(int32_t, acc)));
#else
    return acc + (x & 0xFFFFU) * (y & 0xFFFFU) + (x >> 16) * (y >> 16);
#endif
}

lh_u32_t
lh_ui_color_mix_pair(lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    /* Two channels a word (0x00FF00FF lanes): one multiply blends both, and the
       products of a lane (255 * 255 at most) never reach the next one. */
    return lh_ui_color_div255_round_pair(src * a + dst * (LH_BYTE_T_MAX - a));
}

lh_ui_color_channel_t
lh_ui_color_over_opaque_channel(lh_u32_t src, lh_u32_t dst, lh_u32_t a)
{
#if LH_UI_COLOR_HAVE_SMLAD
    /* src * a + dst * (255 - a) is one dual multiply-accumulate: SMLAD on a core
       with the DSP extension (Cortex-M4 and up). */
    return lh_cast_static(lh_ui_color_channel_t,
                          lh_ui_color_div255_round(lh_ui_color_dual_mac(
                              src | (dst << 16), a | ((LH_BYTE_T_MAX - a) << 16), 0U)));
#else
    /* Without SMLAD the two products are written out: packing them into halfwords
       only for lh_ui_color_dual_mac to unpack them again costs shifts, masks and
       ORs a channel on cores without the extension (ESP32, RISC-V). */
    return lh_cast_static(lh_ui_color_channel_t,
                          lh_ui_color_div255_round(src * a + dst * (LH_BYTE_T_MAX - a)));
#endif
}

lh_u32_t
lh_ui_color_argb_make(lh_u32_t r, lh_u32_t g, lh_u32_t b, lh_u32_t a)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

lh_u32_t
lh_ui_color_argb_over_opaque(lh_u32_t dst, lh_u32_t src)
{
    const lh_u32_t a = lh_ui_color_hex_channel(src, 24);
    /* R and B in one word, G (and the alpha lane, which an opaque result does not
       read) in another: two multiplies a pair instead of one a channel. */
    const lh_u32_t rb = lh_ui_color_mix_pair(dst & 0x00FF00FFU, src & 0x00FF00FFU, a);
    const lh_u32_t ga = lh_ui_color_mix_pair((dst >> 8) & 0x00FF00FFU, (src >> 8) & 0x00FF00FFU, a);

    return 0xFF000000U | rb | ((ga & 0xFFU) << 8);
}

lh_u32_t
lh_ui_color_argb_over_translucent(lh_u32_t dst, lh_u32_t src)
{
    const lh_u32_t a = lh_ui_color_hex_channel(src, 24);
    const lh_u32_t src_w = a * LH_BYTE_T_MAX;
    const lh_u32_t dst_w =
        lh_cast_static(lh_u32_t, lh_ui_color_hex_channel(dst, 24)) * (LH_BYTE_T_MAX - a);

    lh_return_if(src_w + dst_w == 0U, 0U);
    return lh_ui_color_argb_make(
        lh_ui_color_blend_channel(lh_ui_color_hex_channel(src, 16),
                                  lh_ui_color_hex_channel(dst, 16), src_w, dst_w),
        lh_ui_color_blend_channel(lh_ui_color_hex_channel(src, 8), lh_ui_color_hex_channel(dst, 8),
                                  src_w, dst_w),
        lh_ui_color_blend_channel(lh_ui_color_hex_channel(src, 0), lh_ui_color_hex_channel(dst, 0),
                                  src_w, dst_w),
        lh_ui_color_weight_to_alpha(src_w + dst_w));
}

lh_u32_t
lh_ui_color_argb_over(lh_u32_t dst, lh_u32_t src)
{
    return lh_ui_color_hex_channel(dst, 24) == LH_BYTE_T_MAX
               ? lh_ui_color_argb_over_opaque(dst, src)
               : lh_ui_color_argb_over_translucent(dst, src);
}

lh_void
lh_ui_color_over_opaque(const lh_ui_color_t *dst, const lh_ui_color_t *color, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_color_t result;

    lh_ui_color_init(lh_addr_of(result),
                     lh_ui_color_over_opaque_channel(lh_ui_color_get_r(color), lh_ui_color_get_r(dst), lh_ui_color_get_a(color)),
                     lh_ui_color_over_opaque_channel(lh_ui_color_get_g(color), lh_ui_color_get_g(dst), lh_ui_color_get_a(color)),
                     lh_ui_color_over_opaque_channel(lh_ui_color_get_b(color), lh_ui_color_get_b(dst), lh_ui_color_get_a(color)), LH_BYTE_T_MAX);
    lh_ptr_deref(out) = result;
}

lh_void
lh_ui_color_over_translucent(const lh_ui_color_t *dst, const lh_ui_color_t *color,
                             lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_u32_t src_w = lh_ui_color_over_src_weight(color);
    const lh_u32_t dst_w = lh_ui_color_over_dst_weight(dst, color);
    lh_ui_color_t clear;
    lh_ui_color_init(lh_addr_of(clear), 0, 0, 0, 0);
    if (src_w + dst_w == 0U)
    {
        lh_ptr_deref(out) = clear;
    }
    else
    {
        lh_ui_color_blend(dst, color, src_w, dst_w, out);
    }
}

lh_void
lh_ui_color_over(const lh_ui_color_t *dst, const lh_ui_color_t *color, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(dst);
    if (lh_ui_color_get_a(dst) == LH_BYTE_T_MAX)
    {
        lh_ui_color_over_opaque(dst, color, out);
    }
    else
    {
        lh_ui_color_over_translucent(dst, color, out);
    }
}

lh_void
lh_ui_color_with_coverage(const lh_ui_color_t *color, lh_byte_t coverage, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_color_t result;
    lh_assert_runtime_ref(color);
    result = lh_ptr_deref(color);
    lh_ui_color_set_a(lh_addr_of(result), lh_cast_static(lh_ui_color_channel_t, lh_math_div_round(lh_ui_color_get_a(color) * coverage, LH_BYTE_T_MAX)));
    lh_ptr_deref(out) = result;
}
