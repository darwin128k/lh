/**
 * @file blur.c
 * @brief Implementation of `lh/ui/blur.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/blur.h>
#include <lh/ui/radius.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_blur_window_take(lh_ui_blur_window_t *self, const lh_ui_color_t *pixel)
{
    const lh_s32_t a = lh_ui_color_get_a(pixel);

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pixel);
    /* Colour times alpha, not the colour alone: a pixel nobody can see still
       holds bytes, and those bytes are not part of the picture. */
    self->r += lh_ui_color_get_r(pixel) * a;
    self->g += lh_ui_color_get_g(pixel) * a;
    self->b += lh_ui_color_get_b(pixel) * a;
    self->a += a;
}

lh_void
lh_ui_blur_window_drop(lh_ui_blur_window_t *self, const lh_ui_color_t *pixel)
{
    const lh_s32_t a = lh_ui_color_get_a(pixel);

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pixel);
    self->r -= lh_ui_color_get_r(pixel) * a;
    self->g -= lh_ui_color_get_g(pixel) * a;
    self->b -= lh_ui_color_get_b(pixel) * a;
    self->a -= a;
}

lh_u8_t
lh_ui_blur_window_average(const lh_ui_blur_window_t *self, lh_s32_t channel, lh_s32_t count)
{
    lh_s32_t sum;
    lh_s32_t straight;

    lh_assert_runtime_ref(self);
    lh_return_if(channel < 0 || channel > 3 || count <= 0, (lh_u8_t)0);
    if (channel == 3)
    {
        return lh_cast_static(lh_u8_t, lh_math_clamp(lh_math_div(self->a + count / 2, count), 0, 255));
    }
    switch (channel)
    {
        case 0:
            sum = self->r;
            break;
        case 1:
            sum = self->g;
            break;
        default:
            sum = self->b;
            break;
    }
    lh_return_if(sum == 0 || self->a <= 0, (lh_u8_t)0);
    /* Every pixel opaque is the picture a frame actually blurs, and that average
       was already (sum of the channel + count / 2) / count. Dividing the
       premultiplied sum by the alpha sum rounds the other way once the window
       holds an odd number of opaque pixels, so that case stays on the old
       divisor. Anything else divides the weights back out by the alpha. */
    if (self->a == 255 * count)
    {
        straight = sum / 255;
        return lh_cast_static(lh_u8_t, lh_math_clamp(lh_math_div(straight + count / 2, count), 0, 255));
    }
    return lh_cast_static(lh_u8_t, lh_math_clamp(lh_math_div(sum + self->a / 2, self->a), 0, 255));
}

lh_usize_t
lh_ui_blur_scratch_size(const lh_ui_rect_t *rect, lh_s32_t radius)
{
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    lh_s32_t rows;

    if (lh_ui_rect_is_empty(rect))
    {
        return 0;
    }
    rows = lh_ui_size_get_height(size);
    if (radius > 0)
    {
        rows += radius * 2;
    }
    return (lh_usize_t)lh_ui_size_get_width(size) * (lh_usize_t)rows * 4U;
}

/* Rows the column pass has to already hold: the rect, and up to @p radius rows
   past it on each side, cut to the pixmap. The scratch is addressed from @p y0. */
static lh_void
blur_band(const lh_ui_pixmap_t *pixmap, lh_s32_t top, lh_s32_t height, lh_s32_t radius, lh_s32_t *y0, lh_s32_t *y1)
{
    const lh_s32_t pix_h = lh_ui_pixmap_get_height(pixmap);
    lh_s32_t last = top + height + (radius > 0 ? radius : 0);

    *y0 = radius > 0 && top > radius ? top - radius : 0;
    if (last > pix_h)
    {
        last = pix_h;
    }
    *y1 = last;
}

/* One row, blurred across. The window reads the pixmap, out to @p radius past
   the rect and no further than the pixmap; @p out receives only the rect. */
static lh_void
blur_row_x(const lh_ui_pixmap_t *pixmap, lh_s32_t y, lh_s32_t left, lh_s32_t width, lh_s32_t radius, lh_u8_t *out)
{
    const lh_s32_t pix_w = lh_ui_pixmap_get_width(pixmap);
    const lh_s32_t read0 = radius > 0 && left > radius ? left - radius : 0;
    lh_s32_t read1 = left + width + (radius > 0 ? radius : 0);
    lh_s32_t win0;
    lh_s32_t win1;
    lh_s32_t x;
    lh_ui_blur_window_t sum = {0, 0, 0, 0};

    if (read1 > pix_w)
    {
        read1 = pix_w;
    }
    win0 = left - radius;
    win1 = left + radius + 1;
    if (win0 < read0)
    {
        win0 = read0;
    }
    if (win1 > read1)
    {
        win1 = read1;
    }
    for (x = win0; x < win1; ++x)
    {
        const lh_ui_color_t pixel = lh_ui_pixmap_get_pixel(pixmap, x, y);

        lh_ui_blur_window_take(&sum, &pixel);
    }
    for (x = left; x < left + width; ++x)
    {
        lh_s32_t next0 = x + 1 - radius;
        lh_s32_t next1 = x + 1 + radius + 1;

        out[0] = lh_ui_blur_window_average(&sum, 0, win1 - win0);
        out[1] = lh_ui_blur_window_average(&sum, 1, win1 - win0);
        out[2] = lh_ui_blur_window_average(&sum, 2, win1 - win0);
        out[3] = lh_ui_blur_window_average(&sum, 3, win1 - win0);
        out += 4;
        if (next0 < read0)
        {
            next0 = read0;
        }
        if (next1 > read1)
        {
            next1 = read1;
        }
        for (; win0 < next0; ++win0)
        {
            const lh_ui_color_t gone = lh_ui_pixmap_get_pixel(pixmap, win0, y);

            lh_ui_blur_window_drop(&sum, &gone);
        }
        for (; win1 < next1; ++win1)
        {
            const lh_ui_color_t in = lh_ui_pixmap_get_pixel(pixmap, win1, y);

            lh_ui_blur_window_take(&sum, &in);
        }
    }
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
    lh_s32_t y0;
    lh_s32_t y1;
    lh_s32_t y;

    lh_return_if(radius < 0 || lh_null_eq(scratch) || width <= 0 || height <= 0);
    blur_band(pixmap, top, height, radius, &y0, &y1);
    for (y = y0; y < y1; ++y)
    {
        blur_row_x(pixmap, y, left, width, radius, scratch + (y - y0) * width * 4);
    }
}

static lh_void
blur_put(lh_ui_pixmap_t *pixmap, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color, const lh_ui_rect_t *shape,
         lh_ui_scalar_t corner)
{
    lh_byte_t cover;

    if (lh_null_eq(shape) || corner <= lh_ui_scalar(0))
    {
        lh_ui_pixmap_set_pixel(pixmap, x, y, color);
        return;
    }
    /* The square corner of a rounded panel is not the panel. Leaving it is what
       keeps the picture there the picture that was there; writing the blur and
       trusting the tint to cover it leaves a square halo, because the tint
       follows the arc and the arc does not reach this pixel. A pixel the arc
       covers in part is the arc's edge, so the blur replaces it in that part. */
    cover = lh_ui_radius_coverage(shape, corner, x, y);
    lh_return_if(cover == 0U);
    if (cover == 255U)
    {
        lh_ui_pixmap_set_pixel(pixmap, x, y, color);
        return;
    }
    lh_ui_pixmap_cover_pixel(pixmap, x, y, color, cover);
}

lh_void
lh_ui_blur_columns(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, const lh_u8_t *scratch,
                   const lh_ui_rect_t *shape, lh_ui_scalar_t corner)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_s32_t left = lh_ui_point_get_x(origin);
    const lh_s32_t top = lh_ui_point_get_y(origin);
    const lh_s32_t width = lh_ui_size_get_width(size);
    const lh_s32_t height = lh_ui_size_get_height(size);
    const lh_ui_scalar_t rounded =
        lh_null_ne(shape) && corner > lh_ui_scalar(0) ? lh_ui_radius_clamp(shape, corner) : lh_ui_scalar(0);
    lh_s32_t y0;
    lh_s32_t y1;
    lh_s32_t x;

    lh_return_if(radius < 0 || lh_null_eq(scratch) || width <= 0 || height <= 0);
    blur_band(pixmap, top, height, radius, &y0, &y1);
    for (x = 0; x < width; ++x)
    {
        lh_s32_t win0 = top - radius;
        lh_s32_t win1 = top + radius + 1;
        lh_s32_t y;
        lh_ui_blur_window_t sum = {0, 0, 0, 0};

        if (win0 < y0)
        {
            win0 = y0;
        }
        if (win1 > y1)
        {
            win1 = y1;
        }
        for (y = win0; y < win1; ++y)
        {
            const lh_u8_t *sample = scratch + ((y - y0) * width + x) * 4;
            lh_ui_color_t pixel;

            lh_ui_color_init(&pixel, sample[0], sample[1], sample[2], sample[3]);
            lh_ui_blur_window_take(&sum, &pixel);
        }
        for (y = top; y < top + height; ++y)
        {
            lh_s32_t next0 = y + 1 - radius;
            lh_s32_t next1 = y + 1 + radius + 1;
            lh_ui_color_t out;

            lh_ui_color_init(&out, lh_ui_blur_window_average(&sum, 0, win1 - win0),
                             lh_ui_blur_window_average(&sum, 1, win1 - win0),
                             lh_ui_blur_window_average(&sum, 2, win1 - win0),
                             lh_ui_blur_window_average(&sum, 3, win1 - win0));
            blur_put(pixmap, left + x, y, &out, rounded > lh_ui_scalar(0) ? shape : lh_null, rounded);
            if (next0 < y0)
            {
                next0 = y0;
            }
            if (next1 > y1)
            {
                next1 = y1;
            }
            for (; win0 < next0; ++win0)
            {
                const lh_u8_t *sample = scratch + ((win0 - y0) * width + x) * 4;
                lh_ui_color_t gone;

                lh_ui_color_init(&gone, sample[0], sample[1], sample[2], sample[3]);
                lh_ui_blur_window_drop(&sum, &gone);
            }
            for (; win1 < next1; ++win1)
            {
                const lh_u8_t *sample = scratch + ((win1 - y0) * width + x) * 4;
                lh_ui_color_t in;

                lh_ui_color_init(&in, sample[0], sample[1], sample[2], sample[3]);
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
    lh_s32_t px;

    lh_return_if(lh_null_eq(pixmap) || lh_null_eq(rect) || radius <= 0 || lh_null_eq(scratch));
    cut = lh_ui_rect_intersection(rect, &bounds);
    px = lh_ui_scalar_floor_s32(radius);
    lh_return_if(lh_ui_blur_scratch_size(lh_addr_of(cut), px) == 0);
    lh_ui_blur_rows(pixmap, lh_addr_of(cut), px, scratch);
    lh_ui_blur_columns(pixmap, lh_addr_of(cut), px, scratch, lh_null, lh_ui_scalar(0));
}
