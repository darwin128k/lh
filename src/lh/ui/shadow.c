#include <lh/ui/shadow.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_int_t
lh_ui_shadow_isqrt(lh_int_t value)
{
    lh_int_t root;
    lh_int_t next;
    if (value <= 0)
    {
        return 0;
    }
    root = value;
    next = (root + 1) / 2;
    while (next < root)
    {
        root = next;
        next = (root + value / root) / 2;
    }
    return root;
}

lh_int_t
lh_ui_shadow_distance(lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top, lh_int_t right,
                      lh_int_t bottom, lh_int_t corner)
{
    const lh_int_t cx = left + (right - left) / 2;
    const lh_int_t cy = top + (bottom - top) / 2;
    lh_int_t hx = (right - left) / 2 - corner;
    lh_int_t hy = (bottom - top) / 2 - corner;
    lh_int_t dx;
    lh_int_t dy;
    lh_int_t ox;
    lh_int_t oy;
    lh_int_t inside;
    if (hx < 0)
    {
        hx = 0;
    }
    if (hy < 0)
    {
        hy = 0;
    }
    dx = x >= cx ? x - cx : cx - x;
    dy = y >= cy ? y - cy : cy - y;
    dx -= hx;
    dy -= hy;
    ox = dx > 0 ? dx : 0;
    oy = dy > 0 ? dy : 0;
    inside = dx > dy ? dx : dy;
    if (inside > 0)
    {
        inside = 0;
    }
    return lh_ui_shadow_isqrt(ox * ox + oy * oy) + inside - corner;
}

/* True when (@p x, @p y) lies off an edge that @p sides casts. */
lh_bool_t
lh_ui_shadow_on_side(lh_ui_shadow_side_t sides, lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top,
                     lh_int_t right, lh_int_t bottom)
{
    lh_bool_t hit = lh_bool_false;
    if (x < left && (sides & LH_UI_SHADOW_SIDE_LEFT) != 0)
    {
        hit = lh_bool_true;
    }
    if (x >= right && (sides & LH_UI_SHADOW_SIDE_RIGHT) != 0)
    {
        hit = lh_bool_true;
    }
    if (y < top && (sides & LH_UI_SHADOW_SIDE_TOP) != 0)
    {
        hit = lh_bool_true;
    }
    if (y >= bottom && (sides & LH_UI_SHADOW_SIDE_BOTTOM) != 0)
    {
        hit = lh_bool_true;
    }
    if (x >= left && x < right && y >= top && y < bottom)
    {
        if (x < left + (right - left) / 2)
        {
            if ((sides & LH_UI_SHADOW_SIDE_LEFT) != 0)
            {
                hit = lh_bool_true;
            }
        }
        else if ((sides & LH_UI_SHADOW_SIDE_RIGHT) != 0)
        {
            hit = lh_bool_true;
        }
        if (y < top + (bottom - top) / 2)
        {
            if ((sides & LH_UI_SHADOW_SIDE_TOP) != 0)
            {
                hit = lh_bool_true;
            }
        }
        else if ((sides & LH_UI_SHADOW_SIDE_BOTTOM) != 0)
        {
            hit = lh_bool_true;
        }
    }
    return hit;
}

/* True when the fill owns (@p x, @p y). A square box is half-open: the row
 * @p y == @p bottom and the column @p x == @p right are not painted, and the
 * distance of those pixels is 0, the same as the first row and column that
 * are painted. Treating 0 as inside leaves that row the background color, a
 * light line between the box and the shadow. A rounded box keeps distance
 * <= 0, which is the edge the window already antialiases. */
lh_bool_t
lh_ui_shadow_inside(lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top, lh_int_t right,
                    lh_int_t bottom, lh_int_t corner)
{
    if (corner <= 0)
    {
        if (x >= left && y >= top && x < right && y < bottom)
        {
            return lh_bool_true;
        }
        return lh_bool_false;
    }
    if (lh_ui_shadow_distance(x, y, left, top, right, bottom, corner) <= 0)
    {
        return lh_bool_true;
    }
    return lh_bool_false;
}

/* Smoothstep of a signed distance. Zero at +@p spread, the full @p peak at
 * -@p spread, so the fade straddles the shifted edge instead of holding a
 * flat band for the whole offset. */
lh_int_t
lh_ui_shadow_falloff(lh_int_t signed_dist, lh_int_t spread, lh_int_t peak)
{
    const lh_int_t span = spread * 2;
    lh_int_t u;
    lh_int_t t;
    lh_int_t tt;
    lh_int_t curve;
    if (signed_dist >= spread)
    {
        return 0;
    }
    if (signed_dist <= -spread)
    {
        return peak;
    }
    u = spread - signed_dist;
    t = u * 256 / span;
    tt = t * t / 256;
    curve = tt * (768 - 2 * t) / 256;
    return peak * curve / 256;
}

lh_int_t
lh_ui_shadow_alpha(const lh_ui_shadow_t *self, lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top,
                   lh_int_t right, lh_int_t bottom, lh_int_t corner)
{
    const lh_uint_t argb = lh_ui_color_to_argb(self->color);
    const lh_int_t peak = lh_cast_static(lh_int_t, (argb >> 24) & 0xFFU);
    const lh_int_t shift_x = self->offset_x;
    const lh_int_t shift_y = self->offset_y;
    lh_int_t spread;
    lh_int_t dist;
    lh_assert_runtime_ref(self);
    spread = self->spread;
    if (spread <= 0 || peak <= 0 || right <= left || bottom <= top)
    {
        return 0;
    }
    if (corner < 0)
    {
        corner = 0;
    }
    if (!lh_ui_shadow_on_side(self->sides, x, y, left, top, right, bottom))
    {
        return 0;
    }
    /* The box itself is not a shadow. Outside it, the alpha follows the
       shifted edge: dark near that edge, gone one spread away. */
    if (lh_ui_shadow_inside(x, y, left, top, right, bottom, corner))
    {
        return 0;
    }
    dist = lh_ui_shadow_distance(x, y, left + shift_x, top + shift_y, right + shift_x,
                                 bottom + shift_y, corner);
    return lh_ui_shadow_falloff(dist, spread, peak);
}

lh_void
lh_ui_shadow_draw(const lh_ui_effect_t *self, lh_ui_canvas_t *canvas, lh_math_rect_t box,
                  lh_int_t corner)
{
    const lh_ui_shadow_t *const shadow = lh_ptr_rcast(const lh_ui_shadow_t, self);
    const lh_int_t left = lh_math_rect_get_x(lh_addr_of(box));
    const lh_int_t top = lh_math_rect_get_y(lh_addr_of(box));
    const lh_int_t right = left + lh_math_rect_get_size_width(lh_addr_of(box));
    const lh_int_t bottom = top + lh_math_rect_get_size_height(lh_addr_of(box));
    const lh_int_t pad = lh_ui_shadow_outset(shadow);
    const lh_uint_t rgb = lh_ui_color_to_argb(shadow->color) & 0x00FFFFFFU;
    lh_int_t y;
    if (pad <= 0)
    {
        return;
    }
    for (y = top - pad; y < bottom + pad; ++y)
    {
        lh_int_t x;
        for (x = left - pad; x < right + pad; ++x)
        {
            const lh_int_t alpha =
                lh_ui_shadow_alpha(shadow, x, y, left, top, right, bottom, corner);
            if (alpha > 0)
            {
                lh_ui_canvas_blend_pixel(canvas, x, y,
                                         lh_ui_color_from_argb((lh_cast_static(lh_uint_t, alpha)
                                                                << 24) |
                                                               rgb));
            }
        }
    }
}

lh_int_t
lh_ui_shadow_outset_of(const lh_ui_effect_t *self)
{
    return lh_ui_shadow_outset(lh_ptr_rcast(const lh_ui_shadow_t, self));
}

const lh_ui_effect_class_t lh_ui_shadow_class = {lh_ui_shadow_draw, lh_ui_shadow_outset_of};

lh_void
lh_ui_shadow_init(lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_effect_init(lh_ptr_rcast(lh_ui_effect_t, self), lh_addr_of(lh_ui_shadow_class));
    self->color = lh_ui_color_make(0, 0, 0, 0);
    self->spread = 0;
    self->offset_x = 0;
    self->offset_y = 0;
    self->sides = LH_UI_SHADOW_SIDE_ALL;
}

const lh_ui_effect_t *
lh_ui_shadow_effect(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ptr_rcast(const lh_ui_effect_t, self);
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

lh_int_t
lh_ui_shadow_get_spread(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->spread;
}

lh_void
lh_ui_shadow_set_spread(lh_ui_shadow_t *self, lh_int_t spread)
{
    lh_assert_runtime_ref(self);
    self->spread = spread < 0 ? 0 : spread;
}

lh_int_t
lh_ui_shadow_get_offset_x(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_x;
}

lh_int_t
lh_ui_shadow_get_offset_y(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset_y;
}

lh_void
lh_ui_shadow_set_offset(lh_ui_shadow_t *self, lh_int_t offset_x, lh_int_t offset_y)
{
    lh_assert_runtime_ref(self);
    self->offset_x = offset_x;
    self->offset_y = offset_y;
}

lh_ui_shadow_side_t
lh_ui_shadow_get_sides(const lh_ui_shadow_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sides;
}

lh_void
lh_ui_shadow_set_sides(lh_ui_shadow_t *self, lh_ui_shadow_side_t sides)
{
    lh_assert_runtime_ref(self);
    self->sides = sides;
}

lh_int_t
lh_ui_shadow_outset(const lh_ui_shadow_t *self)
{
    lh_int_t ox;
    lh_int_t oy;
    lh_int_t extra;
    lh_assert_runtime_ref(self);
    if (self->spread <= 0)
    {
        return 0;
    }
    ox = self->offset_x < 0 ? -self->offset_x : self->offset_x;
    oy = self->offset_y < 0 ? -self->offset_y : self->offset_y;
    extra = ox > oy ? ox : oy;
    return self->spread + extra;
}
