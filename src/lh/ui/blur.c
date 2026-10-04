#include <lh/ui/blur.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/runtime/allocator.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_ui_color_t
lh_ui_blur_sample(const lh_ui_color_t *pixels, lh_int_t width, lh_int_t height, lh_int_t x,
                  lh_int_t y, lh_int_t radius, lh_bool_t horizontal)
{
    lh_int_t sum_a = 0;
    lh_int_t sum_r = 0;
    lh_int_t sum_g = 0;
    lh_int_t sum_b = 0;
    lh_int_t count = 0;
    lh_int_t step;
    for (step = -radius; step <= radius; ++step)
    {
        const lh_int_t sx = horizontal ? x + step : x;
        const lh_int_t sy = horizontal ? y : y + step;
        lh_int_t clamped_x = sx;
        lh_int_t clamped_y = sy;
        lh_uint_t argb;
        if (clamped_x < 0)
        {
            clamped_x = 0;
        }
        if (clamped_y < 0)
        {
            clamped_y = 0;
        }
        if (clamped_x >= width)
        {
            clamped_x = width - 1;
        }
        if (clamped_y >= height)
        {
            clamped_y = height - 1;
        }
        argb = lh_ui_color_to_argb(pixels[clamped_y * width + clamped_x]);
        sum_a += lh_cast_static(lh_int_t, (argb >> 24) & 0xFFU);
        sum_r += lh_cast_static(lh_int_t, (argb >> 16) & 0xFFU);
        sum_g += lh_cast_static(lh_int_t, (argb >> 8) & 0xFFU);
        sum_b += lh_cast_static(lh_int_t, argb & 0xFFU);
        count += 1;
    }
    return lh_ui_color_from_argb((lh_cast_static(lh_uint_t, sum_a / count) << 24) |
                                 (lh_cast_static(lh_uint_t, sum_r / count) << 16) |
                                 (lh_cast_static(lh_uint_t, sum_g / count) << 8) |
                                 lh_cast_static(lh_uint_t, sum_b / count));
}

lh_void
lh_ui_blur_pass(const lh_ui_color_t *src, lh_ui_color_t *dst, lh_int_t width, lh_int_t height,
                lh_int_t radius, lh_bool_t horizontal)
{
    lh_int_t y;
    for (y = 0; y < height; ++y)
    {
        lh_int_t x;
        for (x = 0; x < width; ++x)
        {
            dst[y * width + x] = lh_ui_blur_sample(src, width, height, x, y, radius, horizontal);
        }
    }
}

lh_void
lh_ui_blur_apply(lh_ui_canvas_t *canvas, lh_math_rect_t rect, lh_int_t radius)
{
    const lh_math_rect_t clip = lh_ui_canvas_get_clip(canvas);
    const lh_math_rect_t area = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(rect));
    const lh_int_t width = lh_math_rect_get_size_width(lh_addr_of(area));
    const lh_int_t height = lh_math_rect_get_size_height(lh_addr_of(area));
    const lh_int_t x0 = lh_math_rect_get_x(lh_addr_of(area));
    const lh_int_t y0 = lh_math_rect_get_y(lh_addr_of(area));
    const lh_usize_t count = lh_cast_static(lh_usize_t, width) * lh_cast_static(lh_usize_t, height);
    lh_ui_color_t *src;
    lh_ui_color_t *dst;
    lh_int_t y;
    lh_assert_runtime_ref(canvas);
    if (radius <= 0 || lh_math_rect_is_empty(lh_addr_of(area)))
    {
        return;
    }
    src = lh_ptr_rcast(lh_ui_color_t, lh_runtime_allocator_alloc(count * sizeof(lh_ui_color_t)));
    dst = lh_ptr_rcast(lh_ui_color_t, lh_runtime_allocator_alloc(count * sizeof(lh_ui_color_t)));
    if (lh_ptr_is_null(src) || lh_ptr_is_null(dst))
    {
        lh_runtime_allocator_free(src);
        lh_runtime_allocator_free(dst);
        return;
    }
    for (y = 0; y < height; ++y)
    {
        lh_int_t x;
        for (x = 0; x < width; ++x)
        {
            src[y * width + x] = lh_ui_canvas_get_pixel(canvas, x0 + x, y0 + y);
        }
    }
    lh_ui_blur_pass(src, dst, width, height, radius, lh_bool_true);
    lh_ui_blur_pass(dst, src, width, height, radius, lh_bool_false);
    for (y = 0; y < height; ++y)
    {
        lh_int_t x;
        for (x = 0; x < width; ++x)
        {
            lh_ui_canvas_blend_pixel(canvas, x0 + x, y0 + y, src[y * width + x]);
        }
    }
    lh_runtime_allocator_free(src);
    lh_runtime_allocator_free(dst);
}

lh_void
lh_ui_blur_draw(const lh_ui_effect_t *self, lh_ui_canvas_t *canvas, lh_math_rect_t box,
                lh_int_t corner)
{
    (void)corner;
    lh_ui_blur_apply(canvas, box, lh_ptr_rcast(const lh_ui_blur_t, self)->radius);
}

lh_int_t
lh_ui_blur_outset(const lh_ui_effect_t *self)
{
    (void)self;
    return 0;
}

const lh_ui_effect_class_t lh_ui_blur_class = {lh_ui_blur_draw, lh_ui_blur_outset};

lh_void
lh_ui_blur_init(lh_ui_blur_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_effect_init(lh_ptr_rcast(lh_ui_effect_t, self), lh_addr_of(lh_ui_blur_class));
    self->radius = 0;
}

const lh_ui_effect_t *
lh_ui_blur_effect(const lh_ui_blur_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ptr_rcast(const lh_ui_effect_t, self);
}

lh_int_t
lh_ui_blur_get_radius(const lh_ui_blur_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_ui_blur_set_radius(lh_ui_blur_t *self, lh_int_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
}
