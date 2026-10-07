/**
 * @file color.c
 * @brief Implementation of `lh/ui/color.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/bool.h>
#include <lh/byte/limits.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/color.h>
#include <lh/util/addr.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_void
lh_ui_color_init(lh_ui_color_t *self, lh_ui_color_channel_t r, lh_ui_color_channel_t g,
                 lh_ui_color_channel_t b, lh_ui_color_channel_t a)
{
    lh_assert_runtime_ref(self);
    self->r = r;
    self->g = g;
    self->b = b;
    self->a = a;
}

lh_void
lh_ui_color_init_hex(lh_ui_color_t *self, lh_u32_t hex)
{
    lh_ui_color_init(self, lh_ui_color_hex_channel(hex, 24), lh_ui_color_hex_channel(hex, 16),
                     lh_ui_color_hex_channel(hex, 8), lh_ui_color_hex_channel(hex, 0));
}

lh_ui_color_channel_t
lh_ui_color_hex_channel(lh_u32_t hex, lh_u32_t shift)
{
    return lh_cast_static(lh_ui_color_channel_t, (hex >> shift) & lh_cast_static(lh_u32_t, LH_BYTE_T_MAX));
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
    return (lh_cast_static(lh_u32_t, self->a) << 24) | (lh_cast_static(lh_u32_t, self->r) << 16) |
           (lh_cast_static(lh_u32_t, self->g) << 8) | lh_cast_static(lh_u32_t, self->b);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

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
    return lh_cast_static(lh_bool_t, lh_math_eq(self->r, other->r) && lh_math_eq(self->g, other->g) &&
                                     lh_math_eq(self->b, other->b) && lh_math_eq(self->a, other->a));
}

/* ── Blend ───────────────────────────────────────────────────────────────── */

lh_u32_t
lh_ui_color_over_src_weight(const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(color);
    return lh_cast_static(lh_u32_t, color->a) * LH_BYTE_T_MAX;
}

lh_u32_t
lh_ui_color_over_dst_weight(const lh_ui_color_t *dst, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(color);
    return lh_cast_static(lh_u32_t, dst->a) * (LH_BYTE_T_MAX - color->a);
}

lh_ui_color_channel_t
lh_ui_color_blend_channel(lh_u32_t src, lh_u32_t dst, lh_u32_t src_w, lh_u32_t dst_w)
{
    const lh_u32_t total = src_w + dst_w;
    return lh_cast_static(lh_ui_color_channel_t, (src * src_w + dst * dst_w + total / 2U) / total);
}

lh_ui_color_t
lh_ui_color_blend(const lh_ui_color_t *dst, const lh_ui_color_t *color, lh_u32_t src_w, lh_u32_t dst_w)
{
    lh_ui_color_t out;
    lh_ui_color_init(lh_addr_of(out), lh_ui_color_blend_channel(color->r, dst->r, src_w, dst_w),
                     lh_ui_color_blend_channel(color->g, dst->g, src_w, dst_w),
                     lh_ui_color_blend_channel(color->b, dst->b, src_w, dst_w),
                     lh_ui_color_weight_to_alpha(src_w + dst_w));
    return out;
}

lh_ui_color_channel_t
lh_ui_color_weight_to_alpha(lh_u32_t weight)
{
    return lh_cast_static(lh_ui_color_channel_t, (weight + LH_BYTE_T_MAX / 2U) / LH_BYTE_T_MAX);
}

lh_ui_color_channel_t
lh_ui_color_over_opaque_channel(lh_u32_t src, lh_u32_t dst, lh_u32_t a)
{
    return lh_cast_static(lh_ui_color_channel_t,
                          (src * a + dst * (LH_BYTE_T_MAX - a) + LH_BYTE_T_MAX / 2U) / LH_BYTE_T_MAX);
}

lh_ui_color_t
lh_ui_color_over_opaque(const lh_ui_color_t *dst, const lh_ui_color_t *color)
{
    lh_ui_color_t out;

    lh_ui_color_init(lh_addr_of(out), lh_ui_color_over_opaque_channel(color->r, dst->r, color->a),
                     lh_ui_color_over_opaque_channel(color->g, dst->g, color->a),
                     lh_ui_color_over_opaque_channel(color->b, dst->b, color->a), LH_BYTE_T_MAX);
    return out;
}

lh_ui_color_t
lh_ui_color_over_translucent(const lh_ui_color_t *dst, const lh_ui_color_t *color)
{
    const lh_u32_t src_w = lh_ui_color_over_src_weight(color);
    const lh_u32_t dst_w = lh_ui_color_over_dst_weight(dst, color);
    lh_ui_color_t clear;
    lh_ui_color_init(lh_addr_of(clear), 0, 0, 0, 0);
    return src_w + dst_w == 0U ? clear : lh_ui_color_blend(dst, color, src_w, dst_w);
}

lh_ui_color_t
lh_ui_color_over(const lh_ui_color_t *dst, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(dst);
    return dst->a == LH_BYTE_T_MAX ? lh_ui_color_over_opaque(dst, color)
                                   : lh_ui_color_over_translucent(dst, color);
}

lh_ui_color_t
lh_ui_color_with_coverage(const lh_ui_color_t *color, lh_byte_t coverage)
{
    lh_ui_color_t out;
    lh_assert_runtime_ref(color);
    out = *color;
    out.a = lh_cast_static(lh_ui_color_channel_t, (color->a * coverage + LH_BYTE_T_MAX / 2U) / LH_BYTE_T_MAX);
    return out;
}
