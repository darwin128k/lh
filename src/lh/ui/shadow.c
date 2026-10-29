/**
 * @file shadow.c
 * @brief Implementation of `lh/ui/shadow.h`.
 */
#include <lh/assert.h>

#include <lh/math.h>
#include <lh/ui/shadow/shape.h>
#include <lh/ui/radius.h>
#include <lh/util/return.h>

lh_void
lh_ui_shadow_init(lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_color_init(lh_ui_shadow_get_color_mut(self), 0, 0, 0, 0);
    lh_ui_shadow_set_spread(self, 0);
    lh_ui_shadow_set_offset_x(self, 0);
    lh_ui_shadow_set_offset_y(self, 0);
}

lh_void
lh_ui_shadow_copy_color(const lh_ui_shadow_t *self, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);
    lh_ptr_deref(out) = lh_ptr_deref(lh_ui_shadow_get_color(self));
}

lh_bool_t
lh_ui_shadow_is_empty(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_shadow_get_spread(self) <= 0 || lh_ui_color_get_a(lh_ui_shadow_get_color(self)) == 0 ? lh_bool_true
                                                                                : lh_bool_false;
}

lh_void
lh_ui_shadow_set_color(lh_ui_shadow_t *self, lh_ui_color_t color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
}

lh_ui_scalar_t
lh_ui_shadow_get_spread(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->spread;
}

lh_void
lh_ui_shadow_set_spread_v(lh_ui_shadow_t *self, lh_ui_scalar_t spread)
{
    lh_assert_runtime_ref(self);
    lh_ui_shadow_set_spread(self, spread > 0 ? spread : 0);
}

lh_void
lh_ui_shadow_set_offset(lh_ui_shadow_t *self, lh_ui_scalar_t offset_x, lh_ui_scalar_t offset_y)
{
    lh_assert_runtime_ref(self);
    lh_ui_shadow_set_offset_x(self, offset_x);
    lh_ui_shadow_set_offset_y(self, offset_y);
}

lh_ui_scalar_t
lh_ui_shadow_get_offset_x(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_x;
}

lh_ui_scalar_t
lh_ui_shadow_get_offset_y(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_y;
}

lh_ui_scalar_t
lh_ui_shadow_get_outset(const lh_ui_shadow_t *self, const lh_ui_rect_t *rect)
{
    lh_s32_t shift_x;
    lh_s32_t shift_y;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    if (lh_ui_shadow_get_spread(self) <= 0 || lh_ui_rect_is_empty(rect) ||
        lh_ui_color_get_a(lh_ui_shadow_get_color(self)) == 0)
    {
        return 0;
    }
    shift_x = lh_math_abs(lh_ui_shadow_get_offset_x(self));
    shift_y = lh_math_abs(lh_ui_shadow_get_offset_y(self));
    /* The fade reaches `spread` past the shifted edge, and the edge itself is as
       far as the shift carries it. */
    return (lh_ui_scalar_t)(lh_ui_shadow_get_spread(self) + lh_math_max(shift_x, shift_y));
}

lh_s64_t
lh_ui_shadow_signed_distance(lh_s64_t px, lh_s64_t py, const lh_ui_rect_t *rect,
                             lh_ui_scalar_t radius)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);
    const lh_s64_t r = lh_ui_radius_to_fixed(lh_ui_radius_clamp(rect, radius));
    const lh_s64_t lo_x = lh_ui_radius_to_fixed(lh_ui_point_get_x(origin));
    const lh_s64_t hi_x = lo_x + lh_ui_radius_to_fixed(lh_ui_size_get_width(size));
    const lh_s64_t lo_y = lh_ui_radius_to_fixed(lh_ui_point_get_y(origin));
    const lh_s64_t hi_y = lo_y + lh_ui_radius_to_fixed(lh_ui_size_get_height(size));
    const lh_s64_t half_x = (hi_x - lo_x) / 2;
    const lh_s64_t half_y = (hi_y - lo_y) / 2;
    const lh_s64_t dx = lh_math_abs(px - (lo_x + half_x)) - (half_x - r);
    const lh_s64_t dy = lh_math_abs(py - (lo_y + half_y)) - (half_y - r);
    const lh_s64_t ox = lh_math_max(dx, 0);
    const lh_s64_t oy = lh_math_max(dy, 0);
    lh_s64_t inside = lh_math_max(dx, dy);

    inside = inside < 0 ? inside : 0;
    if (ox == 0 && oy == 0)
    {
        return inside - r;
    }
    /* Beside a straight edge only one of the two is outside, and the length of
       (ox, 0) is ox: the square root is the corner's alone. Taking it for every
       pixel of a shadow -- most of which lie along an edge -- was most of the
       shadow's time, for the same number. */
    if (oy == 0)
    {
        return ox + inside - r;
    }
    if (ox == 0)
    {
        return oy + inside - r;
    }
    return (lh_s64_t)lh_math_isqrt_u64(lh_cast_static(lh_u64_t, ox) * lh_cast_static(lh_u64_t, ox) +
                                       lh_cast_static(lh_u64_t, oy) *
                                           lh_cast_static(lh_u64_t, oy)) +
           inside - r;
}

/** @p distance and @p spread are in the same fixed point. The public falloff
   stays in whole pixels and truncates; this one is what a pixel center, which
   sits half a pixel off the integer grid, has to be faded by. */
lh_byte_t
lh_ui_shadow_falloff_sub(lh_s64_t distance, lh_s64_t spread, lh_byte_t peak)
{
    const lh_s64_t span = spread * 2;
    lh_s64_t fade;

    if (spread <= 0 || peak == 0)
    {
        return 0;
    }
    fade = lh_cast_static(lh_s64_t, peak) * (spread - distance);
    if (fade <= 0)
    {
        return 0;
    }
    fade = lh_math_div_round(fade, span);
    if (fade > peak)
    {
        fade = peak;
    }
    return lh_cast_static(lh_byte_t, fade);
}

lh_s32_t
lh_ui_shadow_distance(lh_ui_scalar_t x, lh_ui_scalar_t y, const lh_ui_rect_t *rect,
                      lh_ui_scalar_t radius)
{
    return (lh_s32_t)(lh_ui_shadow_signed_distance(lh_ui_radius_to_fixed(x),
                                                   lh_ui_radius_to_fixed(y), rect, radius) /
                      LH_UI_RADIUS_SUBPIXEL);
}

lh_byte_t
lh_ui_shadow_falloff(lh_s32_t distance, lh_ui_scalar_t spread, lh_byte_t peak)
{
    lh_s32_t fade;

    if (spread <= 0 || peak == 0)
    {
        return 0;
    }
    /* The fade straddles the edge: full inside, nothing a spread past it, and a
       smooth curve between. A flat band for the whole offset is what makes a
       shadow look like a second box instead of like light. */
    fade = peak * (spread - distance) / (2 * spread);
    fade = lh_math_clamp(fade, 0, peak);
    return lh_cast_static(lh_byte_t, fade);
}

lh_byte_t
lh_ui_shadow_alpha_at(const lh_ui_shadow_t *self, lh_ui_scalar_t x, lh_ui_scalar_t y,
                      const lh_ui_rect_t *rect, lh_ui_scalar_t radius)
{
    const lh_ui_size_t *size;
    const lh_ui_point_t *origin;
    const lh_byte_t peak = lh_ui_color_get_a(lh_ui_shadow_get_color(self));
    const lh_s32_t corner = lh_ui_radius_clamp(rect, radius);
    lh_ui_rect_t shifted;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    size = lh_ui_rect_get_size(rect);
    if (lh_ui_shadow_get_spread(self) <= 0 || peak == 0 || lh_ui_rect_is_empty(rect))
    {
        return 0;
    }

    origin = lh_ui_rect_get_origin(rect);
    /* Inside the box the fill owns the pixel, but the edge itself is not the
       fill's: a square box is half-open, and a rounded one asks the distance and
       compares it strictly, so the pixel the edge runs through carries the shadow
       at half strength under a fill that covers half of it. Counting distance zero
       as inside is what leaves a bright line a pixel wide along every straight
       edge, which is the very thing the half-open square avoids. */
    if (corner <= 0 ? (x >= lh_ui_point_get_x(origin) &&
                       x < lh_ui_point_get_x(origin) + lh_ui_size_get_width(size) &&
                       y >= lh_ui_point_get_y(origin) &&
                       y < lh_ui_point_get_y(origin) + lh_ui_size_get_height(size))
                    : lh_ui_shadow_distance(x, y, rect, corner) < 0)
    {
        return 0;
    }

    shifted = lh_ptr_deref(rect);
    /* ::lh_ui_rect_offset hands back the moved rect: it does not move its argument.
       Calling it for its effect left every shadow where its box was, whatever offset
       the style gave it. */
    lh_ui_rect_offset(lh_addr_of(shifted), lh_ui_shadow_get_offset_x(self), lh_ui_shadow_get_offset_y(self), lh_addr_of(shifted));
    return lh_ui_shadow_falloff(lh_ui_shadow_distance(x, y, lh_addr_of(shifted), corner),
                                lh_ui_shadow_get_spread(self), peak);
}

lh_byte_t
lh_ui_shadow_alpha_at_pixel(const lh_ui_shadow_t *self, lh_s32_t x, lh_s32_t y,
                            const lh_ui_rect_t *rect, lh_ui_scalar_t radius)
{
    lh_ui_shadow_shape_t shape;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    /* The one reading of a shadow (::lh_ui_shadow_shape_t), so a backend that draws a
       whole shadow through the shape and the canvas that asks pixel by pixel give the
       same alpha. */
    lh_ui_shadow_shape_init(lh_addr_of(shape), self, rect, radius);
    return lh_ui_shadow_shape_alpha(lh_addr_of(shape), lh_ui_shadow_shape_row(lh_addr_of(shape), y),
                                    x, y);
}

lh_ui_color_t *
lh_ui_shadow_get_color_mut(lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_void
lh_ui_shadow_set_offset_x(lh_ui_shadow_t *self, lh_ui_scalar_t offset_x)
{
    lh_assert_runtime_ref(self);
    self->offset_x = offset_x;
}

lh_void
lh_ui_shadow_set_offset_y(lh_ui_shadow_t *self, lh_ui_scalar_t offset_y)
{
    lh_assert_runtime_ref(self);
    self->offset_y = offset_y;
}

const lh_ui_color_t *
lh_ui_shadow_get_color(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_void
lh_ui_shadow_set_spread(lh_ui_shadow_t *self, lh_ui_scalar_t spread)
{
    lh_assert_runtime_ref(self);
    self->spread = spread;
}
