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

lh_s32_t
lh_ui_shadow_distance(lh_ui_scalar_t x, lh_ui_scalar_t y, const lh_ui_rect_t *rect, lh_ui_scalar_t radius)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_s32_t left = lh_ui_point_get_x(origin);
    const lh_s32_t top = lh_ui_point_get_y(origin);
    const lh_s32_t width = lh_ui_size_get_width(size);
    const lh_s32_t height = lh_ui_size_get_height(size);
    const lh_s32_t corner = lh_ui_radius_clamp(rect, radius);
    /* From the centre outwards, minus the corner: negative where the pixel is
       still in the box proper, and the corner is the part that bulges. */
    const lh_s32_t dx = lh_math_abs(x - (left + width / 2)) - (width / 2 - corner);
    const lh_s32_t dy = lh_math_abs(y - (top + height / 2)) - (height / 2 - corner);
    const lh_s32_t ox = lh_math_max(dx, 0);
    const lh_s32_t oy = lh_math_max(dy, 0);
    lh_s32_t inside = lh_math_max(dx, dy);

    inside = inside < 0 ? inside : 0;
    if (ox == 0 && oy == 0)
    {
        return inside - corner;
    }
    /* One root per pixel: the values are a few pixels wide, so this is a table
       lookup on any machine that has one. */
    return (lh_s32_t)lh_math_isqrt_u64((lh_u64_t)((lh_s64_t)ox * ox + (lh_s64_t)oy * oy)) + inside - corner;
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
