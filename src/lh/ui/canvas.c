#include <lh/ui/canvas.h>
#include <lh/assert.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/config.h>
#include <lh/numeric/types.h>
#include <lh/runtime/error/code.h>
#include <lh/util/addr.h>
#include <lh/util/numeric.h>

#include <math.h>

/* @p color over @p dst, straight alpha: each channel moves from dst toward the
 * color by alpha / the channel maximum, and the result is at least as opaque
 * as either. */
static lh_ui_color_t
lh_ui_canvas_blend(lh_ui_color_t dst, lh_ui_color_t color)
{
    const lh_uint_t max = lh_numeric_limit_umax(lh_byte_t);
    const lh_uint_t a = color.a;
    const lh_uint_t keep = max - a;
    return lh_ui_color_make(lh_cast_static(lh_byte_t, (color.r * a + dst.r * keep + max / 2U) / max),
                            lh_cast_static(lh_byte_t, (color.g * a + dst.g * keep + max / 2U) / max),
                            lh_cast_static(lh_byte_t, (color.b * a + dst.b * keep + max / 2U) / max),
                            lh_cast_static(lh_byte_t, a + (dst.a * keep + max / 2U) / max));
}

lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, lh_ui_color_t *pixels, lh_math_coord_t width,
                  lh_math_coord_t height, lh_math_coord_t stride)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pixels);
    lh_assert_runtime_if(width < 0 || height < 0 || stride < width,
                         lh_runtime_error_code_invalid_argument);
    self->pixels = pixels;
    self->width = width;
    self->height = height;
    self->stride = stride;
    self->clip = lh_math_rect_make(0, 0, width, height);
    self->depth = lh_null;
    self->draw_z = 0.0f;
}

lh_void
lh_ui_canvas_set_depth(lh_ui_canvas_t *self, lh_float_t *depth)
{
    lh_assert_runtime_ref(self);
    self->depth = depth;
}

lh_void
lh_ui_canvas_set_draw_z(lh_ui_canvas_t *self, lh_float_t z)
{
    lh_assert_runtime_ref(self);
    self->draw_z = z;
}

/* Nothing is closer than this, so the next fragment in a cleared sample passes. */
static const lh_float_t lh_ui_canvas_depth_far = -3.402823466e+38f;

lh_void
lh_ui_canvas_clear_depth(lh_ui_canvas_t *self, lh_math_rect_t area)
{
    lh_assert_runtime_ref(self);
    if (!self->depth)
    {
        return;
    }
    const lh_math_rect_t image = lh_math_rect_make(0, 0, self->width, self->height);
    const lh_math_rect_t cleared = lh_math_rect_intersection(lh_addr_of(image), lh_addr_of(area));
    if (lh_math_rect_is_empty(lh_addr_of(cleared)))
    {
        return;
    }
    const lh_math_coord_t x0 = lh_math_rect_get_x(lh_addr_of(cleared));
    const lh_math_coord_t y0 = lh_math_rect_get_y(lh_addr_of(cleared));
    const lh_math_coord_t x1 = x0 + lh_math_rect_get_size_width(lh_addr_of(cleared));
    const lh_math_coord_t y1 = y0 + lh_math_rect_get_size_height(lh_addr_of(cleared));
    for (lh_math_coord_t y = y0; y < y1; ++y)
    {
        lh_float_t *const row = self->depth + y * self->stride;
        for (lh_math_coord_t x = x0; x < x1; ++x)
        {
            row[x] = lh_ui_canvas_depth_far;
        }
    }
}

/* Keep the fragment when it is closer than or level with the stored sample,
 * and record its depth. No plane: always keep. */
static lh_bool_t
lh_ui_canvas_depth_pass(lh_ui_canvas_t *self, lh_math_coord_t x, lh_math_coord_t y)
{
    if (!self->depth)
    {
        return lh_bool_true;
    }
    lh_float_t *const sample = self->depth + y * self->stride + x;
    if (self->draw_z < *sample)
    {
        return lh_bool_false;
    }
    *sample = self->draw_z;
    return lh_bool_true;
}

lh_math_coord_t
lh_ui_canvas_get_width(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_math_coord_t
lh_ui_canvas_get_height(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_math_rect_t
lh_ui_canvas_get_clip(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clip;
}

lh_void
lh_ui_canvas_set_clip(lh_ui_canvas_t *self, lh_math_rect_t clip)
{
    const lh_math_rect_t image =
        lh_math_rect_make(0, 0, lh_ui_canvas_get_width(self), lh_ui_canvas_get_height(self));
    self->clip = lh_math_rect_intersection(lh_addr_of(image), lh_addr_of(clip));
}

lh_ui_color_t
lh_ui_canvas_get_pixel(const lh_ui_canvas_t *self, lh_math_coord_t x, lh_math_coord_t y)
{
    lh_assert_runtime_ifn(x >= 0 && y >= 0 && x < lh_ui_canvas_get_width(self) &&
                              y < lh_ui_canvas_get_height(self),
                          lh_runtime_error_code_out_of_range);
    return self->pixels[y * self->stride + x];
}

lh_void
lh_ui_canvas_blend_pixel(lh_ui_canvas_t *self, lh_math_coord_t x, lh_math_coord_t y,
                         lh_ui_color_t color)
{
    const lh_math_rect_t clip = lh_ui_canvas_get_clip(self);
    if (!lh_math_rect_contains_point(lh_addr_of(clip), lh_math_point_make(x, y)))
    {
        return;
    }
    if (!lh_ui_canvas_depth_pass(self, x, y))
    {
        return;
    }
    lh_ui_color_t *const pixel = self->pixels + y * self->stride + x;
    *pixel = lh_ui_canvas_blend(*pixel, color);
}

lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, lh_math_rect_t rect, lh_ui_color_t color)
{
    const lh_math_rect_t clip = lh_ui_canvas_get_clip(self);
    const lh_math_rect_t area = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(rect));
    if (lh_math_rect_is_empty(lh_addr_of(area)) || color.a == 0U)
    {
        return;
    }

    const lh_math_coord_t x0 = lh_math_rect_get_x(lh_addr_of(area));
    const lh_math_coord_t y0 = lh_math_rect_get_y(lh_addr_of(area));
    const lh_math_coord_t x1 = x0 + lh_math_rect_get_size_width(lh_addr_of(area));
    const lh_math_coord_t y1 = y0 + lh_math_rect_get_size_height(lh_addr_of(area));
    for (lh_math_coord_t y = y0; y < y1; ++y)
    {
        lh_ui_color_t *const row = self->pixels + y * self->stride;
        for (lh_math_coord_t x = x0; x < x1; ++x)
        {
            if (!lh_ui_canvas_depth_pass(self, x, y))
            {
                continue;
            }
            row[x] = color.a == 255U ? color : lh_ui_canvas_blend(row[x], color);
        }
    }
}

lh_int_t
lh_ui_canvas_isqrt(lh_sllong_t value)
{
    lh_sllong_t root;
    lh_sllong_t next;
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
    return (lh_int_t)root;
}

lh_byte_t
lh_ui_canvas_coverage_from(lh_int_t signed_dist)
{
    const lh_int_t span = LH_LIBRARY_OPTION_UI_COVER;
    lh_int_t half;
    lh_int_t cover;
    if (span <= 0)
    {
        return signed_dist <= 0 ? 255 : 0;
    }
    half = span / 2;
    if (signed_dist <= -half)
    {
        return 255;
    }
    if (signed_dist >= half)
    {
        return 0;
    }
    cover = (lh_int_t)(((lh_sllong_t)(half - signed_dist) * 255) / span);
    if (cover >= 255)
    {
        return 255;
    }
    if (cover <= 0)
    {
        return 0;
    }
    return (lh_byte_t)cover;
}

lh_byte_t
lh_ui_canvas_disc_coverage(lh_int_t x, lh_int_t y, lh_int_t cx, lh_int_t cy, lh_int_t radius)
{
    const lh_sllong_t dx = ((lh_sllong_t)x - cx) * 256 + 128;
    const lh_sllong_t dy = ((lh_sllong_t)y - cy) * 256 + 128;
    lh_int_t dist;
    if (radius <= 0)
    {
        return 0;
    }
    dist = lh_ui_canvas_isqrt(dx * dx + dy * dy);
    return lh_ui_canvas_coverage_from(dist - radius * 256);
}

lh_byte_t
lh_ui_canvas_round_coverage(lh_int_t x, lh_int_t y, lh_int_t left, lh_int_t top, lh_int_t right,
                            lh_int_t bottom, lh_int_t radius)
{
    const lh_int_t width = right - left;
    const lh_int_t height = bottom - top;
    lh_int_t rad = radius;
    lh_int_t sx;
    lh_int_t sy;
    lh_int_t dx;
    lh_int_t dy;
    lh_int_t ox;
    lh_int_t oy;
    lh_int_t inside;
    lh_int_t dist;
    if (width <= 0 || height <= 0)
    {
        return 0;
    }
    if (x < left - 1 || y < top - 1 || x >= right + 1 || y >= bottom + 1)
    {
        return 0;
    }
    if (rad < 0)
    {
        rad = 0;
    }
    if (rad > width / 2)
    {
        rad = width / 2;
    }
    if (rad > height / 2)
    {
        rad = height / 2;
    }
    if (rad == 0)
    {
        if (x >= left && y >= top && x < right && y < bottom)
        {
            return 255;
        }
        return 0;
    }
    if (x >= left + rad + 1 && x < right - rad - 1 && y >= top + rad + 1 && y < bottom - rad - 1)
    {
        return 255;
    }
    sx = (x - left) * 256 + 128;
    sy = (y - top) * 256 + 128;
    dx = sx >= width * 128 ? sx - width * 128 : width * 128 - sx;
    dy = sy >= height * 128 ? sy - height * 128 : height * 128 - sy;
    dx -= width * 128 - rad * 256;
    dy -= height * 128 - rad * 256;
    ox = dx > 0 ? dx : 0;
    oy = dy > 0 ? dy : 0;
    inside = dx > dy ? dx : dy;
    if (inside > 0)
    {
        inside = 0;
    }
    dist = lh_ui_canvas_isqrt((lh_sllong_t)ox * ox + (lh_sllong_t)oy * oy);
    return lh_ui_canvas_coverage_from(dist + inside - rad * 256);
}

lh_void
lh_ui_canvas_blend_coverage(lh_ui_canvas_t *self, lh_int_t x, lh_int_t y, lh_ui_color_t color,
                            lh_byte_t coverage)
{
    const lh_uint_t argb = lh_ui_color_to_argb(color);
    const lh_uint_t alpha = ((argb >> 24) * coverage) / 255U;
    if (alpha == 0U)
    {
        return;
    }
    lh_ui_canvas_blend_pixel(self, x, y, lh_ui_color_from_argb((argb & 0x00FFFFFFU) | (alpha << 24)));
}

lh_void
lh_ui_canvas_fill_disc(lh_ui_canvas_t *self, lh_int_t cx, lh_int_t cy, lh_int_t radius,
                       lh_ui_color_t color)
{
    lh_int_t y;
    lh_assert_runtime_ref(self);
    if (radius <= 0 || color.a == 0U)
    {
        return;
    }
    for (y = cy - radius - 1; y <= cy + radius + 1; ++y)
    {
        lh_int_t x;
        for (x = cx - radius - 1; x <= cx + radius + 1; ++x)
        {
            lh_ui_canvas_blend_coverage(self, x, y, color,
                                        lh_ui_canvas_disc_coverage(x, y, cx, cy, radius));
        }
    }
}

lh_void
lh_ui_canvas_fill_round(lh_ui_canvas_t *self, lh_math_rect_t rect, lh_int_t radius,
                        lh_ui_color_t color)
{
    const lh_int_t left = lh_math_rect_get_x(lh_addr_of(rect));
    const lh_int_t top = lh_math_rect_get_y(lh_addr_of(rect));
    const lh_int_t right = left + lh_math_rect_get_size_width(lh_addr_of(rect));
    const lh_int_t bottom = top + lh_math_rect_get_size_height(lh_addr_of(rect));
    lh_int_t y;
    lh_assert_runtime_ref(self);
    if (color.a == 0U || right <= left || bottom <= top)
    {
        return;
    }
    for (y = top - 1; y <= bottom; ++y)
    {
        lh_int_t x;
        for (x = left - 1; x <= right; ++x)
        {
            lh_ui_canvas_blend_coverage(
                self, x, y, color,
                lh_ui_canvas_round_coverage(x, y, left, top, right, bottom, radius));
        }
    }
}

lh_void
lh_ui_canvas_fill_arc(lh_ui_canvas_t *self, lh_int_t cx, lh_int_t cy, lh_int_t outer,
                      lh_int_t inner, lh_float_t start, lh_float_t end, lh_ui_color_t color)
{
    const lh_float_t turn = 6.2831853f;
    lh_int_t y;
    lh_assert_runtime_ref(self);
    if (color.a == 0U || outer <= 0 || inner >= outer || end == start)
    {
        return;
    }
    if (inner < 0)
    {
        inner = 0;
    }
    if (end < start)
    {
        end += turn;
    }
    if (end > start + turn)
    {
        end = start + turn;
    }
    for (y = cy - outer - 1; y <= cy + outer + 1; ++y)
    {
        lh_int_t x;
        for (x = cx - outer - 1; x <= cx + outer + 1; ++x)
        {
            const lh_int_t dx = x - cx;
            const lh_int_t dy = y - cy;
            const lh_byte_t outside = lh_ui_canvas_disc_coverage(x, y, cx, cy, outer);
            const lh_byte_t hole = lh_ui_canvas_disc_coverage(x, y, cx, cy, inner);
            lh_float_t angle;
            lh_int_t cover;
            if (outside <= hole || (dx == 0 && dy == 0))
            {
                continue;
            }
            angle = atan2f((lh_float_t)dy, (lh_float_t)dx);
            if (angle < start)
            {
                angle += turn;
            }
            if (angle < start || angle > end)
            {
                continue;
            }
            cover = (lh_int_t)outside - (lh_int_t)hole;
            lh_ui_canvas_blend_coverage(self, x, y, color, lh_cast_static(lh_byte_t, cover));
        }
    }
}
