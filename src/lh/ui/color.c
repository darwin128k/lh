/**
 * @file color.c
 * @brief Implementation of `lh/ui/color.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/byte/limits.h>
#include <lh/cast/static.h>
#include <lh/numeric/parse/bytes.h>
#include <lh/ui/color.h>
#include <lh/util/addr.h>

/* ── Constructors ────────────────────────────────────────────────────────── */

lh_void
lh_ui_color_init(lh_ui_color_t *self, lh_ui_color_channel_t r, lh_ui_color_channel_t g,
                 lh_ui_color_channel_t b, lh_ui_color_channel_t a)
{
    lh_ui_color_set_r(self, r);
    lh_ui_color_set_g(self, g);
    lh_ui_color_set_b(self, b);
    lh_ui_color_set_a(self, a);
}

lh_void
lh_ui_color_init_hex(lh_ui_color_t *self, lh_uint_t hex)
{
    lh_ui_color_channels_t c;
    lh_numeric_parse_bytes(hex, c, LH_UI_COLOR_CHANNELS_SIZE);
    lh_ui_color_init(self, c[0], c[1], c[2], c[3]);
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

/* ── Blend ───────────────────────────────────────────────────────────────── */

lh_ui_color_t
lh_ui_color_over(const lh_ui_color_t *dst, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(color);

    const lh_uint_t max = LH_BYTE_T_MAX;
    lh_uint_t a;
    lh_uint_t keep;
    lh_ui_color_t out;
    a = lh_ui_color_get_a(color);
    keep = max - a;
    lh_ui_color_init(
        lh_addr_of(out),
        lh_cast_static(lh_ui_color_channel_t,
                       (lh_ui_color_get_r(color) * a + lh_ui_color_get_r(dst) * keep + max / 2U) /
                           max),
        lh_cast_static(lh_ui_color_channel_t,
                       (lh_ui_color_get_g(color) * a + lh_ui_color_get_g(dst) * keep + max / 2U) /
                           max),
        lh_cast_static(lh_ui_color_channel_t,
                       (lh_ui_color_get_b(color) * a + lh_ui_color_get_b(dst) * keep + max / 2U) /
                           max),
        lh_cast_static(lh_ui_color_channel_t, a + (lh_ui_color_get_a(dst) * keep + max / 2U) / max));
    return out;
}
