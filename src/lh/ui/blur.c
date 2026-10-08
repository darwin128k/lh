/**
 * @file blur.c
 * @brief Implementation of `lh/ui/blur.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/blur.h>
#include <lh/ui/rect.h>
#include <lh/util/return.h>

lh_void
lh_ui_blur_window_take(lh_ui_blur_window_t *self, const lh_ui_color_t *pixel)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pixel);
    self->r += lh_ui_color_get_r(pixel);
    self->g += lh_ui_color_get_g(pixel);
    self->b += lh_ui_color_get_b(pixel);
    self->a += lh_ui_color_get_a(pixel);
}

lh_void
lh_ui_blur_window_drop(lh_ui_blur_window_t *self, const lh_ui_color_t *pixel)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pixel);
    self->r -= lh_ui_color_get_r(pixel);
    self->g -= lh_ui_color_get_g(pixel);
    self->b -= lh_ui_color_get_b(pixel);
    self->a -= lh_ui_color_get_a(pixel);
}

lh_u8_t
lh_ui_blur_window_average(const lh_ui_blur_window_t *self, lh_s32_t channel, lh_s32_t count)
{
    lh_s32_t sum;

    lh_assert_runtime_ref(self);
    lh_return_if(channel < 0 || channel > 3 || count <= 0, (lh_u8_t)0);
    switch (channel)
    {
        case 0:
            sum = self->r;
            break;
        case 1:
            sum = self->g;
            break;
        case 2:
            sum = self->b;
            break;
        default:
            sum = self->a;
            break;
    }
    return lh_cast_static(lh_u8_t, lh_math_clamp(lh_math_div(sum + count / 2, count), 0, 255));
}

lh_usize_t
lh_ui_blur_scratch_size(const lh_ui_rect_t *rect)
{
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);

    if (lh_ui_rect_is_empty(rect))
    {
        return 0;
    }
    return (lh_usize_t)(lh_ui_size_get_width(size) * lh_ui_size_get_height(size) * 4);
}

lh_void
lh_ui_blur_rows(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, lh_u8_t *scratch)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_s32_t left = lh_ui_point_get_x(origin);
    const lh_s32_t top = lh_ui_point_get_y(origin);
    const lh_s32_t width = lh_ui_size_get_width(size);
    const lh_s32_t height = lh_ui_size_get_height(size);
    lh_s32_t y;

    lh_return_if(radius <= 0 || lh_null_eq(scratch));
    for (y = top; y < top + height; ++y)
    {
        /* The window at the left edge starts clipped: it reaches from the edge to
           `radius` pixels in, not off the picture. */
        const lh_s32_t first = lh_math_min(radius + 1, width);
        lh_ui_blur_window_t sum = {0, 0, 0, 0};
        lh_u8_t *out = scratch + (y - top) * width * 4;
        lh_s32_t x;

        for (x = 0; x < first; ++x)
        {
            const lh_ui_color_t pixel = lh_ui_pixmap_get_pixel(pixmap, left + x, y);

            lh_ui_blur_window_take(&sum, &pixel);
        }
        for (x = 0; x < width; ++x)
        {
            /* How many pixels this window really holds. At the edges it is fewer
               than `2 * radius + 1`, and dividing by the full width there is
               what makes a flat picture come out darker than it went in. */
            const lh_s32_t count = (lh_math_min(x + radius, width - 1) - (x > radius ? x - radius : 0) + 1);
            lh_s32_t channel;

            for (channel = 0; channel < 4; ++channel)
            {
                out[channel] = lh_ui_blur_window_average(&sum, channel, count);
            }
            out += 4;
            /* Slide: the pixel leaving on the left only leaves once the window
               has actually moved past it — until `x` reaches the radius there is
               nothing to take away, and taking something anyway is what darkens a
               flat picture. */
            if (x >= radius)
            {
                const lh_ui_color_t gone = lh_ui_pixmap_get_pixel(pixmap, left + x - radius, y);

                lh_ui_blur_window_drop(&sum, &gone);
            }
            if (x + radius + 1 < width)
            {
                const lh_ui_color_t in = lh_ui_pixmap_get_pixel(pixmap, left + x + radius + 1, y);

                lh_ui_blur_window_take(&sum, &in);
            }
        }
    }
}

lh_void
lh_ui_blur_columns(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, const lh_u8_t *scratch)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_s32_t left = lh_ui_point_get_x(origin);
    const lh_s32_t top = lh_ui_point_get_y(origin);
    const lh_s32_t width = lh_ui_size_get_width(size);
    const lh_s32_t height = lh_ui_size_get_height(size);
    lh_s32_t x;

    lh_return_if(radius <= 0 || lh_null_eq(scratch));
    for (x = 0; x < width; ++x)
    {
        const lh_s32_t first = lh_math_min(radius + 1, height);
        lh_ui_blur_window_t sum = {0, 0, 0, 0};
        lh_s32_t y;

        for (y = 0; y < first; ++y)
        {
            lh_ui_color_t pixel;

            lh_ui_color_init(&pixel, scratch[(y * width + x) * 4 + 0], scratch[(y * width + x) * 4 + 1],
                             scratch[(y * width + x) * 4 + 2], scratch[(y * width + x) * 4 + 3]);
            lh_ui_blur_window_take(&sum, &pixel);
        }
        for (y = 0; y < height; ++y)
        {
            const lh_s32_t count = (lh_math_min(y + radius, height - 1) - (y > radius ? y - radius : 0) + 1);
            lh_ui_color_t out;

            lh_ui_color_init(&out, lh_ui_blur_window_average(&sum, 0, count),
                             lh_ui_blur_window_average(&sum, 1, count),
                             lh_ui_blur_window_average(&sum, 2, count),
                             lh_ui_blur_window_average(&sum, 3, count));
            lh_ui_pixmap_set_pixel(pixmap, left + x, top + y, &out);
            /* Slide exactly as the row pass does: the pixel leaving on the left
               leaves once the window has moved past it, and the one coming in on
               the right is only taken while there is one — reaching past the last
               row is reading whatever memory follows the scratch, and that is how
               the bottom of a picture goes dark. */
            if (y >= radius)
            {
                lh_ui_color_t gone;

                lh_ui_color_init(&gone, scratch[((y - radius) * width + x) * 4 + 0],
                                 scratch[((y - radius) * width + x) * 4 + 1],
                                 scratch[((y - radius) * width + x) * 4 + 2],
                                 scratch[((y - radius) * width + x) * 4 + 3]);
                lh_ui_blur_window_drop(&sum, &gone);
            }
            if (y + radius + 1 < height)
            {
                lh_ui_color_t in;

                lh_ui_color_init(&in, scratch[((y + radius + 1) * width + x) * 4 + 0],
                                 scratch[((y + radius + 1) * width + x) * 4 + 1],
                                 scratch[((y + radius + 1) * width + x) * 4 + 2],
                                 scratch[((y + radius + 1) * width + x) * 4 + 3]);
                lh_ui_blur_window_take(&sum, &in);
            }
        }
    }
}

lh_void
lh_ui_blur_rect(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_u8_t *scratch)
{
    const lh_ui_rect_t bounds = lh_ui_pixmap_get_bounds(pixmap);
    lh_ui_rect_t cut;

    lh_return_if(lh_null_eq(pixmap) || lh_null_eq(rect) || radius <= 0 || lh_null_eq(scratch));
    cut = lh_ui_rect_intersection(rect, &bounds);
    lh_return_if(lh_ui_blur_scratch_size(lh_addr_of(cut)) == 0);
    lh_ui_blur_rows(pixmap, lh_addr_of(cut), radius, scratch);
    lh_ui_blur_columns(pixmap, lh_addr_of(cut), radius, scratch);
}