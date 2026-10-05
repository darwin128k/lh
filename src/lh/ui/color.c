/**
 * @file color.c
 * @brief Implementation of `lh/ui/color.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/ui/color.h>
#include <lh/util/addr.h>
#include <lh/util/numeric.h>

lh_void
lh_ui_color_init(lh_ui_color_t *self, lh_byte_t r, lh_byte_t g, lh_byte_t b, lh_byte_t a)
{
    lh_assert_runtime_ref(self);
    self->r = r;
    self->g = g;
    self->b = b;
    self->a = a;
}

lh_byte_t
lh_ui_color_get_r(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->r;
}

lh_byte_t
lh_ui_color_get_g(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->g;
}

lh_byte_t
lh_ui_color_get_b(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->b;
}

lh_byte_t
lh_ui_color_get_a(const lh_ui_color_t *self)
{
    lh_assert_runtime_ref(self);
    return self->a;
}

lh_ui_color_t
lh_ui_color_over(const lh_ui_color_t *dst, const lh_ui_color_t *color)
{
    const lh_uint_t max = lh_numeric_limit_umax(lh_byte_t);
    lh_uint_t a;
    lh_uint_t keep;
    lh_ui_color_t out;
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(color);
    a = lh_ui_color_get_a(color);
    keep = max - a;
    lh_ui_color_init(lh_addr_of(out),
        lh_cast_static(lh_byte_t, (lh_ui_color_get_r(color) * a + lh_ui_color_get_r(dst) * keep + max / 2U) / max),
        lh_cast_static(lh_byte_t, (lh_ui_color_get_g(color) * a + lh_ui_color_get_g(dst) * keep + max / 2U) / max),
        lh_cast_static(lh_byte_t, (lh_ui_color_get_b(color) * a + lh_ui_color_get_b(dst) * keep + max / 2U) / max),
        lh_cast_static(lh_byte_t, a + (lh_ui_color_get_a(dst) * keep + max / 2U) / max));
    return out;
}
