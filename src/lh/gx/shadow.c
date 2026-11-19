/**
 * @file shadow.c
 * @brief Implementation of `lh/gx/shadow.h`.
 */

#include <lh/gx/shadow.h>

#include <lh/core/pixel/argb.h>

lh_gx_color_t
lh_gx_shadow_get_color(const lh_gx_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->color;
}

lh_void
lh_gx_shadow_set_color(lh_gx_shadow_t *self, lh_gx_color_t color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
}

lh_gx_scalar_t
lh_gx_shadow_get_spread(const lh_gx_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->spread;
}

lh_void
lh_gx_shadow_set_spread(lh_gx_shadow_t *self, lh_gx_scalar_t spread)
{
    lh_assert_runtime_ref(self);
    self->spread = spread;
}

lh_gx_scalar_t
lh_gx_shadow_get_offset_x(const lh_gx_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_x;
}

lh_void
lh_gx_shadow_set_offset_x(lh_gx_shadow_t *self, lh_gx_scalar_t offset_x)
{
    lh_assert_runtime_ref(self);
    self->offset_x = offset_x;
}

lh_gx_scalar_t
lh_gx_shadow_get_offset_y(const lh_gx_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_y;
}

lh_void
lh_gx_shadow_set_offset_y(lh_gx_shadow_t *self, lh_gx_scalar_t offset_y)
{
    lh_assert_runtime_ref(self);
    self->offset_y = offset_y;
}

lh_void
lh_gx_shadow_init(lh_gx_shadow_t *self, lh_gx_color_t color, lh_gx_scalar_t spread,
                  lh_gx_scalar_t offset_x, lh_gx_scalar_t offset_y)
{
    lh_gx_shadow_set_color(self, color);
    lh_gx_shadow_set_spread(self, spread);
    lh_gx_shadow_set_offset_x(self, offset_x);
    lh_gx_shadow_set_offset_y(self, offset_y);
}

lh_void
lh_gx_shadow_get_bounds(const lh_gx_shadow_t *self, const lh_gx_rect_t *box, lh_gx_rect_t *out)
{
    lh_gx_rect_t shifted;
    const lh_gx_scalar_t grow = lh_gx_shadow_get_spread(self) + 1;

    lh_gx_rect_offset(box, lh_gx_shadow_get_offset_x(self), lh_gx_shadow_get_offset_y(self),
                      lh_addr_of(shifted));
    lh_gx_rect_inset(lh_addr_of(shifted), -grow, -grow, out);
}

lh_void
lh_gx_shadow_get_pixel_shadow(const lh_gx_shadow_t *self, const lh_gx_rect_t *box,
                              lh_gx_scalar_t radius, lh_pixel_shadow_t *out)
{
    const lh_s64_t one = LH_PIXEL_ROUND_SUBPIXEL;
    lh_math_rect_t px;

    lh_gx_rect_to_rect(box, lh_addr_of(px));
    const lh_s64_t lo_x = lh_math_point_get_x(lh_math_rect_get_origin(lh_addr_of(px))) * one;
    const lh_s64_t lo_y = lh_math_point_get_y(lh_math_rect_get_origin(lh_addr_of(px))) * one;
    lh_pixel_round_box_t round;

    lh_pixel_round_box_init(
        lh_addr_of(round), lo_x, lo_y,
        lo_x + lh_math_size_get_width(lh_math_rect_get_size(lh_addr_of(px))) * one,
        lo_y + lh_math_size_get_height(lh_math_rect_get_size(lh_addr_of(px))) * one,
        lh_cast_static(lh_s64_t, radius * one));
    lh_pixel_shadow_init(
        out, lh_addr_of(round), lh_cast_static(lh_s64_t, lh_gx_shadow_get_offset_x(self) * one),
        lh_cast_static(lh_s64_t, lh_gx_shadow_get_offset_y(self) * one),
        lh_cast_static(lh_s64_t, lh_gx_shadow_get_spread(self) * one),
        lh_cast_static(lh_byte_t, lh_pixel_argb_get_channel(lh_gx_shadow_get_color(self), 24)));
}
