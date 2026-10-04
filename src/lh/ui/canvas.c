#include <lh/ui/canvas.h>
#include <lh/assert.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/runtime/error/code.h>
#include <lh/util/addr.h>
#include <lh/util/numeric.h>

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
