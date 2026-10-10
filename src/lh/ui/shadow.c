/**
 * @file shadow.c
 * @brief Implementation of `lh/ui/shadow.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/math/isqrt.h>
#include <lh/null.h>
#include <lh/ui/color.h>
#include <lh/ui/point.h>
#include <lh/ui/radius.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/ui/shadow.h>
#include <lh/util/return.h>

lh_void
lh_ui_shadow_init(lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_color_init(&self->color, 0, 0, 0, 0);
    self->spread = 0;
    self->offset_x = 0;
    self->offset_y = 0;
}

lh_ui_color_t
lh_ui_shadow_get_color(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->color;
}

lh_bool_t
lh_ui_shadow_is_empty(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->spread <= 0 || lh_ui_color_get_a(&self->color) == 0 ? lh_bool_true : lh_bool_false;
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
lh_ui_shadow_set_spread(lh_ui_shadow_t *self, lh_ui_scalar_t spread)
{
    lh_assert_runtime_ref(self);
    self->spread = spread > 0 ? spread : 0;
}

lh_void
lh_ui_shadow_set_offset(lh_ui_shadow_t *self, lh_ui_scalar_t offset_x, lh_ui_scalar_t offset_y)
{
    lh_assert_runtime_ref(self);
    self->offset_x = offset_x;
    self->offset_y = offset_y;
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
    if (self->spread <= 0 || lh_ui_rect_is_empty(rect) || lh_ui_color_get_a(&self->color) == 0)
    {
        return 0;
    }
    shift_x = lh_math_abs(self->offset_x);
    shift_y = lh_math_abs(self->offset_y);
    /* The fade reaches `spread` past the shifted edge, and the edge itself is as
       far as the shift carries it. */
    return (lh_ui_scalar_t)(self->spread + lh_math_max(shift_x, shift_y));
}

/* Signed distance, in the same fixed point as ::lh_ui_radius_coverage, from the
   sample (@p px, @p py) to the rounded edge. The centre is the middle of the
   two edges, not `width / 2` of a truncated corner: an odd width then leans a
   pixel to one side, and the shadow on the right of a box is heavier than the
   shadow on the left of it. */
static lh_s64_t
shadow_sd(lh_s64_t px, lh_s64_t py, const lh_ui_rect_t *rect, lh_ui_scalar_t radius)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
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
    return (lh_s64_t)lh_math_isqrt_u64((lh_u64_t)ox * (lh_u64_t)ox + (lh_u64_t)oy * (lh_u64_t)oy) + inside - r;
}

/* @p distance and @p spread are in the same fixed point. The public falloff
   stays in whole pixels and truncates; this one is what a pixel center, which
   sits half a pixel off the integer grid, has to be faded by. */
static lh_byte_t
shadow_falloff_sub(lh_s64_t distance, lh_s64_t spread, lh_byte_t peak)
{
    const lh_s64_t span = spread * 2;
    lh_s64_t fade;

    if (spread <= 0 || peak == 0)
    {
        return 0;
    }
    fade = (lh_s64_t)peak * (spread - distance);
    if (fade <= 0)
    {
        return 0;
    }
    fade = (fade + span / 2) / span;
    if (fade > peak)
    {
        fade = peak;
    }
    return lh_cast_static(lh_byte_t, fade);
}

lh_s32_t
lh_ui_shadow_distance(lh_ui_scalar_t x, lh_ui_scalar_t y, const lh_ui_rect_t *rect, lh_ui_scalar_t radius)
{
    return (lh_s32_t)(shadow_sd(lh_ui_radius_to_fixed(x), lh_ui_radius_to_fixed(y), rect, radius) /
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
lh_ui_shadow_alpha_at(const lh_ui_shadow_t *self, lh_ui_scalar_t x, lh_ui_scalar_t y, const lh_ui_rect_t *rect,
                      lh_ui_scalar_t radius)
{
    const lh_ui_size_t *size;
    const lh_ui_point_t *origin;
    const lh_byte_t peak = lh_ui_color_get_a(&self->color);
    const lh_s32_t corner = lh_ui_radius_clamp(rect, radius);
    lh_ui_rect_t shifted;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    size = lh_ui_rect_get_size_as_const(rect);
    if (self->spread <= 0 || peak == 0 || lh_ui_rect_is_empty(rect))
    {
        return 0;
    }

    origin = lh_ui_rect_get_origin_as_const(rect);
    /* Inside the box the fill owns the pixel, but the edge itself is not the
       fill's: a square box is half-open, and a rounded one asks the distance and
       compares it strictly, so the pixel the edge runs through carries the shadow
       at half strength under a fill that covers half of it. Counting distance zero
       as inside is what leaves a bright line a pixel wide along every straight
       edge, which is the very thing the half-open square avoids. */
    if (corner <= 0
            ? (x >= lh_ui_point_get_x(origin) && x < lh_ui_point_get_x(origin) + lh_ui_size_get_width(size) &&
               y >= lh_ui_point_get_y(origin) && y < lh_ui_point_get_y(origin) + lh_ui_size_get_height(size))
            : lh_ui_shadow_distance(x, y, rect, corner) < 0)
    {
        return 0;
    }

    shifted = *rect;
    lh_ui_rect_offset(&shifted, self->offset_x, self->offset_y);
    return lh_ui_shadow_falloff(lh_ui_shadow_distance(x, y, &shifted, corner), self->spread, peak);
}

lh_byte_t
lh_ui_shadow_alpha_at_pixel(const lh_ui_shadow_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_rect_t *rect,
                            lh_ui_scalar_t radius)
{
    const lh_byte_t peak = lh_ui_color_get_a(&self->color);
    const lh_ui_scalar_t corner = lh_ui_radius_clamp(rect, radius);
    lh_ui_rect_t shifted;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    if (self->spread <= 0 || peak == 0 || lh_ui_rect_is_empty(rect))
    {
        return 0;
    }
    /* The fill owns a pixel it covers wholly, and it decides that from the
       pixel's center (::lh_ui_radius_coverage). Asking the integer corner of
       the pixel instead puts the shadow's edge a different place from the
       fill's, and on a rounded corner the two no longer meet. */
    if (lh_ui_radius_coverage(rect, corner, x, y) == 255U)
    {
        return 0;
    }
    shifted = *rect;
    lh_ui_rect_offset(&shifted, self->offset_x, self->offset_y);
    return shadow_falloff_sub(shadow_sd(lh_ui_radius_pixel_center(x), lh_ui_radius_pixel_center(y), &shifted, corner),
                              lh_ui_radius_to_fixed(self->spread), peak);
}
