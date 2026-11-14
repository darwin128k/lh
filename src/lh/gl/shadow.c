/**
 * @file shadow.c
 * @brief Coverage of a square shadow.
 */
#include <lh/gl/shadow.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/math/isqrt.h>

const lh_gl_color_t *
lh_gl_shadow_get_color(const lh_gl_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_gl_color_t *
lh_gl_shadow_get_color_mut(lh_gl_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_void
lh_gl_shadow_set_color(lh_gl_shadow_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    self->color = lh_ptr_deref(color);
}

lh_gl_scalar_t
lh_gl_shadow_get_spread(const lh_gl_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->spread;
}

lh_void
lh_gl_shadow_set_spread(lh_gl_shadow_t *self, lh_gl_scalar_t spread)
{
    lh_assert_runtime_ref(self);
    self->spread = spread;
}

lh_gl_scalar_t
lh_gl_shadow_get_offset_x(const lh_gl_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_x;
}

lh_gl_scalar_t
lh_gl_shadow_get_offset_y(const lh_gl_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_y;
}

lh_void
lh_gl_shadow_set_offset_x(lh_gl_shadow_t *self, lh_gl_scalar_t x)
{
    lh_assert_runtime_ref(self);
    self->offset_x = x;
}

lh_void
lh_gl_shadow_set_offset_y(lh_gl_shadow_t *self, lh_gl_scalar_t y)
{
    lh_assert_runtime_ref(self);
    self->offset_y = y;
}

lh_void
lh_gl_shadow_set_offset(lh_gl_shadow_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y)
{
    lh_gl_shadow_set_offset_x(self, x);
    lh_gl_shadow_set_offset_y(self, y);
}

lh_bool_t
lh_gl_shadow_is_empty(const lh_gl_shadow_t *self)
{
    return lh_gl_shadow_get_spread(self) <= 0 || lh_gl_color_get_a(lh_gl_shadow_get_color(self)) == 0
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_gl_shadow_init(lh_gl_shadow_t *self)
{
    lh_gl_color_t clear;

    lh_gl_color_init(lh_addr_of(clear), 0, 0, 0, 0);
    lh_gl_shadow_set_color(self, lh_addr_of(clear));
    lh_gl_shadow_set_spread(self, 0);
    lh_gl_shadow_set_offset(self, 0, 0);
}

lh_gl_scalar_t
lh_gl_shadow_distance(lh_gl_scalar_t x, lh_gl_scalar_t y, const lh_gl_rect_t *rect)
{
    lh_assert_runtime_ref(rect);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(rect);
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(rect));
    lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(rect));
    lh_gl_scalar_t dx = lh_math_abs(x - (lh_gl_point_get_x(origin) + width / 2)) - width / 2;
    lh_gl_scalar_t dy = lh_math_abs(y - (lh_gl_point_get_y(origin) + height / 2)) - height / 2;
    lh_gl_scalar_t ax = lh_math_max(dx, 0);
    lh_gl_scalar_t ay = lh_math_max(dy, 0);
    lh_gl_scalar_t inside = lh_math_min(lh_math_max(dx, dy), 0);

    if (ax == 0 && ay == 0)
    {
        return inside;
    }
    if (ay == 0)
    {
        return ax + inside;
    }
    if (ax == 0)
    {
        return ay + inside;
    }
    return lh_cast_static(lh_gl_scalar_t,
                          lh_math_isqrt_u64(lh_cast_static(lh_u64_t, ax) * lh_cast_static(lh_u64_t, ax) +
                                            lh_cast_static(lh_u64_t, ay) * lh_cast_static(lh_u64_t, ay))) +
           inside;
}

lh_gl_color_channel_t
lh_gl_shadow_falloff(lh_gl_scalar_t distance, lh_gl_scalar_t spread, lh_gl_color_channel_t peak)
{
    if (spread <= 0 || peak == 0)
    {
        return 0;
    }
    lh_gl_scalar_t fade = peak * (spread - distance) / (spread * 2);

    fade = lh_math_clamp(fade, 0, lh_cast_static(lh_gl_scalar_t, peak));
    return lh_cast_static(lh_gl_color_channel_t, fade);
}

lh_gl_color_channel_t
lh_gl_shadow_alpha_at(const lh_gl_shadow_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y,
                      const lh_gl_rect_t *rect)
{
    lh_assert_runtime_ref(rect);
    lh_gl_color_channel_t peak = lh_gl_color_get_a(lh_gl_shadow_get_color(self));
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), x, y);
    if (lh_gl_shadow_is_empty(self) || lh_gl_rect_is_empty(rect) ||
        lh_gl_rect_contains_point(rect, point) == lh_bool_true)
    {
        return 0;
    }
    lh_gl_rect_t shifted;

    lh_gl_rect_offset(rect, lh_gl_shadow_get_offset_x(self), lh_gl_shadow_get_offset_y(self),
                      lh_addr_of(shifted));
    return lh_gl_shadow_falloff(lh_gl_shadow_distance(x, y, lh_addr_of(shifted)),
                                lh_gl_shadow_get_spread(self), peak);
}
