/**
 * @file canvas.c
 * @brief Implementation of `lh/ui/canvas.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/math/rect.h>
#include <lh/numeric/types.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/brush.h>
#include <lh/ui/canvas.h>
#include <lh/ui/gradient.h>
#include <lh/ui/paint.h>
#include <lh/ui/pen.h>
#include <lh/util/addr.h>
#include <lh/util/numeric.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

static lh_bool_t
paint_is_clear(const lh_ui_paint_t *paint)
{
    const lh_ui_gradient_t *gradient;
    lh_return_if(lh_ui_paint_get_kind(paint) == lh_ui_paint_solid,
                 lh_ui_color_get_a(lh_ui_paint_get_color(paint)) == 0);
    gradient = lh_ui_paint_get_gradient(paint);
    return lh_ui_color_get_a(lh_ui_gradient_get_from(gradient)) == 0
        && lh_ui_color_get_a(lh_ui_gradient_get_to(gradient)) == 0;
}

static lh_byte_t
mix_channel(lh_byte_t from, lh_byte_t to, lh_math_coord_t index, lh_math_coord_t span)
{
    const lh_int_t delta = lh_cast_static(lh_int_t, to) - lh_cast_static(lh_int_t, from);
    lh_return_if(span <= 1, from);
    return lh_cast_static(lh_byte_t, lh_cast_static(lh_int_t, from) + delta * index / (span - 1));
}

static lh_ui_color_t
mix_color(const lh_ui_color_t *from, const lh_ui_color_t *to, lh_math_coord_t index, lh_math_coord_t span)
{
    lh_ui_color_t color;
    lh_ui_color_init(lh_addr_of(color), mix_channel(lh_ui_color_get_r(from), lh_ui_color_get_r(to), index, span),
                     mix_channel(lh_ui_color_get_g(from), lh_ui_color_get_g(to), index, span),
                     mix_channel(lh_ui_color_get_b(from), lh_ui_color_get_b(to), index, span),
                     mix_channel(lh_ui_color_get_a(from), lh_ui_color_get_a(to), index, span));
    return color;
}

static lh_ullong_t
isqrt(lh_ullong_t value)
{
    lh_ullong_t rest = value;
    lh_ullong_t result = 0;
    lh_ullong_t bit = lh_cast_static(lh_ullong_t, 1) << 62;
    while (bit > rest)
        bit >>= 2;
    while (bit != 0)
    {
        if (rest >= result + bit)
        {
            rest -= result + bit;
            result = (result >> 1) + bit;
        }
        else
            result >>= 1;
        bit >>= 2;
    }
    return result;
}

static lh_math_coord_t
angle_1024(lh_math_coord_t dx, lh_math_coord_t dy)
{
    const lh_sllong_t sx = dx;
    const lh_sllong_t sy = dy;
    const lh_sllong_t ax = sx < 0 ? -sx : sx;
    const lh_sllong_t ay = sy < 0 ? -sy : sy;
    lh_sllong_t angle;
    lh_return_if(ax + ay == 0, 0);
    angle = (ay * 256) / (ax + ay);
    if (sx >= 0)
        angle = sy >= 0 ? angle : 1024 - angle;
    else
        angle = sy >= 0 ? 512 - angle : 512 + angle;
    lh_return_if(angle >= 1024, 0);
    return lh_cast_static(lh_math_coord_t, angle);
}

static lh_ui_color_t
sample_gradient(const lh_ui_gradient_t *gradient, lh_math_rect_t rect, lh_math_coord_t x, lh_math_coord_t y)
{
    const lh_math_coord_t left = lh_math_rect_get_x(lh_addr_of(rect));
    const lh_math_coord_t top = lh_math_rect_get_y(lh_addr_of(rect));
    const lh_math_coord_t width = lh_math_rect_get_size_width(lh_addr_of(rect));
    const lh_math_coord_t height = lh_math_rect_get_size_height(lh_addr_of(rect));
    const lh_ui_color_t *from = lh_ui_gradient_get_from(gradient);
    const lh_ui_color_t *to = lh_ui_gradient_get_to(gradient);
    lh_math_coord_t index;
    lh_math_coord_t span;
    if (lh_ui_gradient_get_kind(gradient) == lh_ui_gradient_linear)
    {
        if (lh_ui_gradient_get_axis(gradient) == lh_ui_gradient_horizontal)
        {
            index = x - left;
            span = width;
        }
        else
        {
            index = y - top;
            span = height;
        }
        return mix_color(from, to, index, span);
    }
    if (lh_ui_gradient_get_kind(gradient) == lh_ui_gradient_radial)
    {
        const lh_sllong_t dx = lh_cast_static(lh_sllong_t, x - left) * 2 - (width - 1);
        const lh_sllong_t dy = lh_cast_static(lh_sllong_t, y - top) * 2 - (height - 1);
        const lh_ullong_t radius =
            isqrt(lh_cast_static(lh_ullong_t, lh_cast_static(lh_sllong_t, width - 1) * (width - 1)
                                 + lh_cast_static(lh_sllong_t, height - 1) * (height - 1)));
        const lh_ullong_t distance = isqrt(lh_cast_static(lh_ullong_t, dx * dx + dy * dy));
        lh_return_if(radius == 0, lh_ptr_deref(from));
        index = distance >= radius ? 255 : lh_cast_static(lh_math_coord_t, distance * 255 / radius);
        return mix_color(from, to, index, 256);
    }
    index = angle_1024(x - left - width / 2, y - top - height / 2);
    return mix_color(from, to, index, 1024);
}

static lh_ui_color_t
sample_paint(const lh_ui_paint_t *paint, lh_math_rect_t rect, lh_math_coord_t x, lh_math_coord_t y)
{
    lh_return_if(lh_ui_paint_get_kind(paint) == lh_ui_paint_solid, lh_ptr_deref(lh_ui_paint_get_color(paint)));
    return sample_gradient(lh_ui_paint_get_gradient(paint), rect, x, y);
}

static lh_void
paint_band(lh_ui_canvas_t *self, lh_math_rect_t clip, lh_math_rect_t band, lh_math_rect_t rect,
           const lh_ui_brush_t *brush)
{
    const lh_math_rect_t cut = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(band));
    lh_ui_canvas_set_clip(self, cut);
    lh_ui_canvas_fill_rect(self, rect, brush);
}

lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, lh_ui_color_t *pixels, lh_math_coord_t width,
                  lh_math_coord_t height, lh_math_coord_t stride)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pixels);
    lh_assert_runtime_if(width < 0 || height < 0 || stride < width, lh_runtime_error_code_invalid_argument);
    self->pixels = pixels;
    self->width = width;
    self->height = height;
    self->stride = stride;
    self->clip = lh_math_rect_make(0, 0, width, height);
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
    lh_math_rect_t image;
    lh_assert_runtime_ref(self);
    image = lh_math_rect_make(0, 0, lh_ui_canvas_get_width(self), lh_ui_canvas_get_height(self));
    self->clip = lh_math_rect_intersection(lh_addr_of(image), lh_addr_of(clip));
}

const lh_ui_color_t *
lh_ui_canvas_get_pixel(const lh_ui_canvas_t *self, lh_math_coord_t x, lh_math_coord_t y)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(x < 0 || y < 0 || x >= self->width || y >= self->height,
                         lh_runtime_error_code_out_of_range);
    return self->pixels + y * self->stride + x;
}

lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, lh_math_rect_t rect, const lh_ui_brush_t *brush)
{
    lh_math_rect_t clip;
    lh_math_rect_t area;
    lh_math_coord_t x0;
    lh_math_coord_t y0;
    lh_math_coord_t x1;
    lh_math_coord_t y1;
    lh_math_coord_t y;
    const lh_ui_paint_t *paint;
    const lh_byte_t opaque = lh_numeric_limit_umax(lh_byte_t);
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(brush);
    paint = lh_ui_brush_get_paint(brush);
    lh_return_if(paint_is_clear(paint));
    clip = lh_ui_canvas_get_clip(self);
    area = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(rect));
    lh_return_if(lh_math_rect_is_empty(lh_addr_of(area)));
    x0 = lh_math_rect_get_x(lh_addr_of(area));
    y0 = lh_math_rect_get_y(lh_addr_of(area));
    x1 = x0 + lh_math_rect_get_size_width(lh_addr_of(area));
    y1 = y0 + lh_math_rect_get_size_height(lh_addr_of(area));
    for (y = y0; y < y1; ++y)
    {
        lh_ui_color_t *const row = self->pixels + y * self->stride;
        lh_math_coord_t x;
        for (x = x0; x < x1; ++x)
        {
            const lh_ui_color_t color = sample_paint(paint, rect, x, y);
            const lh_byte_t alpha = lh_ui_color_get_a(lh_addr_of(color));
            row[x] = alpha == opaque ? color : lh_ui_color_over(lh_addr_of(row[x]), lh_addr_of(color));
        }
    }
}

lh_void
lh_ui_canvas_stroke_rect(lh_ui_canvas_t *self, lh_math_rect_t rect, const lh_ui_pen_t *pen)
{
    lh_math_coord_t x;
    lh_math_coord_t y;
    lh_math_coord_t w;
    lh_math_coord_t h;
    lh_math_rect_t clip;
    const lh_ui_paint_t *paint;
    lh_math_coord_t width;
    lh_ui_brush_t brush;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pen);
    paint = lh_ui_pen_get_paint(pen);
    width = lh_ui_pen_get_width(pen);
    lh_return_if(paint_is_clear(paint) || width <= 0 || lh_math_rect_is_empty(lh_addr_of(rect)));
    if (lh_ui_paint_get_kind(paint) == lh_ui_paint_solid)
        lh_ui_brush_init(lh_addr_of(brush), lh_ui_paint_get_color(paint));
    else
        lh_ui_brush_init_gradient(lh_addr_of(brush), lh_ui_paint_get_gradient(paint));
    x = lh_math_rect_get_x(lh_addr_of(rect));
    y = lh_math_rect_get_y(lh_addr_of(rect));
    w = lh_math_rect_get_size_width(lh_addr_of(rect));
    h = lh_math_rect_get_size_height(lh_addr_of(rect));
    if (width >= w || width >= h)
    {
        lh_ui_canvas_fill_rect(self, rect, lh_addr_of(brush));
        return;
    }
    clip = lh_ui_canvas_get_clip(self);
    paint_band(self, clip, lh_math_rect_make(x, y, w, width), rect, lh_addr_of(brush));
    paint_band(self, clip, lh_math_rect_make(x, y + h - width, w, width), rect, lh_addr_of(brush));
    paint_band(self, clip, lh_math_rect_make(x, y + width, width, h - width * 2), rect, lh_addr_of(brush));
    paint_band(self, clip, lh_math_rect_make(x + w - width, y + width, width, h - width * 2), rect,
               lh_addr_of(brush));
    lh_ui_canvas_set_clip(self, clip);
}
