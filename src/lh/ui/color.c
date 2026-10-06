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
    const lh_u32_t mask = LH_BYTE_T_MAX;
    lh_ui_color_init(self, lh_cast_static(lh_ui_color_channel_t, (hex >> 24) & mask),
                     lh_cast_static(lh_ui_color_channel_t, (hex >> 16) & mask),
                     lh_cast_static(lh_ui_color_channel_t, (hex >> 8) & mask),
                     lh_cast_static(lh_ui_color_channel_t, hex & mask));
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

/*
 * One channel of "over" in straight alpha, everything scaled by 255:
 * (src * a * 255 + dst * dst_a * (255 - a)) / out_a255, rounded.
 * Largest numerator is 2 * 255^3, which fits in 32 bits.
 */
static lh_ui_color_channel_t
lh_ui_color_over_channel(lh_u32_t src, lh_u32_t dst, lh_u32_t src_w, lh_u32_t dst_w,
                         lh_u32_t out_a255)
{
    return lh_cast_static(lh_ui_color_channel_t, (src * src_w + dst * dst_w + out_a255 / 2U) / out_a255);
}

lh_ui_color_t
lh_ui_color_over(const lh_ui_color_t *dst, const lh_ui_color_t *color)
{
    const lh_u32_t max = LH_BYTE_T_MAX;
    lh_u32_t src_w;
    lh_u32_t dst_w;
    lh_u32_t out_a255;
    lh_ui_color_t out;

    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(color);

    /* Weights already carry the extra factor of 255 shared with out_a255. */
    src_w = lh_cast_static(lh_u32_t, color->a) * max;
    dst_w = lh_cast_static(lh_u32_t, dst->a) * (max - color->a);
    out_a255 = src_w + dst_w;
    if (out_a255 == 0U)
    {
        lh_ui_color_init(lh_addr_of(out), 0, 0, 0, 0);
        return out;
    }
    lh_ui_color_init(lh_addr_of(out),
                     lh_ui_color_over_channel(color->r, dst->r, src_w, dst_w, out_a255),
                     lh_ui_color_over_channel(color->g, dst->g, src_w, dst_w, out_a255),
                     lh_ui_color_over_channel(color->b, dst->b, src_w, dst_w, out_a255),
                     lh_cast_static(lh_ui_color_channel_t, (out_a255 + max / 2U) / max));
    return out;
}
