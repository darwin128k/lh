/**
 * @file blur.c
 * @brief Implementation of `lh/ui/blur.h`.
 */

#include <lh/math.h>
#include <lh/ui/blur.h>
#include <lh/ui/radius.h>
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

    lh_assert_runtime_ref(self);
    lh_return_if(channel < 0 || channel > 3 || count <= 0, lh_cast_static(lh_u8_t, 0));
    if (channel == 3)
    {
        return lh_cast_static(lh_u8_t, lh_math_clamp(lh_math_div_round(self->a, count), 0, 255));
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
    lh_return_if(sum == 0 || self->a <= 0, lh_cast_static(lh_u8_t, 0));
    /* Every pixel opaque is the picture a frame actually blurs, and that average
       was already (sum of the channel + count / 2) / count. Dividing the
       premultiplied sum by the alpha sum rounds the other way once the window
       holds an odd number of opaque pixels, so that case stays on the old
       divisor. Anything else divides the weights back out by the alpha. */
    if (self->a == 255 * count)
    {
        return lh_cast_static(lh_u8_t, lh_math_clamp(lh_math_div_round(sum / 255, count), 0, 255));
    }
    return lh_cast_static(lh_u8_t, lh_math_clamp(lh_math_div_round(sum, self->a), 0, 255));
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
    return (lh_usize_t)lh_ui_size_get_width(size) * lh_cast_static(lh_usize_t, rows)* 4U;
}

/** Rows the column pass has to already hold: the rect, and up to @p radius rows
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

/** One row, blurred across. The window reads the pixmap, out to @p radius past
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
        lh_ui_color_t pixel;
        lh_ui_pixmap_get_pixel(pixmap, x, y, lh_addr_of(pixel));

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
            lh_ui_color_t gone;
            lh_ui_pixmap_get_pixel(pixmap, win0, y, lh_addr_of(gone));

            lh_ui_blur_window_drop(&sum, &gone);
        }
        for (; win1 < next1; ++win1)
        {
            lh_ui_color_t in;
            lh_ui_pixmap_get_pixel(pixmap, win1, y, lh_addr_of(in));

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

/** Write a blurred pixel, cut to the rounded @p shape when there is one: kept out of its square
 * corners, blended on its edge. */
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
lh_ui_blur_sample_color(const lh_u8_t *sample, lh_ui_color_t *out)
{
    lh_assert_runtime_ref(sample);
    lh_ui_color_init(out, sample[0], sample[1], sample[2], sample[3]);
}

lh_void
lh_ui_blur_window_take_samples(lh_ui_blur_window_t *self, const lh_u8_t *sample, lh_usize_t stride, lh_s32_t count)
{
    lh_ui_color_t pixel;

    for (; count > 0; --count, sample += stride)
    {
        lh_ui_blur_sample_color(sample, lh_addr_of(pixel));
        lh_ui_blur_window_take(self, lh_addr_of(pixel));
    }
}

lh_void
lh_ui_blur_window_drop_samples(lh_ui_blur_window_t *self, const lh_u8_t *sample, lh_usize_t stride, lh_s32_t count)
{
    lh_ui_color_t pixel;

    for (; count > 0; --count, sample += stride)
    {
        lh_ui_blur_sample_color(sample, lh_addr_of(pixel));
        lh_ui_blur_window_drop(self, lh_addr_of(pixel));
    }
}

lh_void
lh_ui_blur_window_average_color(const lh_ui_blur_window_t *self, lh_s32_t count, lh_ui_color_t *out)
{
    lh_ui_color_init(out, lh_ui_blur_window_average(self, 0, count), lh_ui_blur_window_average(self, 1, count),
                     lh_ui_blur_window_average(self, 2, count), lh_ui_blur_window_average(self, 3, count));
}

lh_void
lh_ui_blur_window_span(lh_s32_t at, lh_s32_t radius, lh_s32_t lo, lh_s32_t hi, lh_s32_t *win0, lh_s32_t *win1)
{
    lh_assert_runtime_ref(win0);
    lh_assert_runtime_ref(win1);
    *win0 = lh_math_max(at - radius, lo);
    *win1 = lh_math_min(at + radius + 1, hi);
}

lh_bool_t
lh_ui_blur_pass_init(const lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, const lh_u8_t *scratch,
                     const lh_ui_rect_t *shape, lh_ui_scalar_t corner, lh_ui_blur_pass_t *out)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);

    lh_assert_runtime_ref(out);
    out->scratch = scratch;
    out->left = lh_ui_point_get_x(origin);
    out->top = lh_ui_point_get_y(origin);
    out->width = lh_ui_size_get_width(size);
    out->height = lh_ui_size_get_height(size);
    out->radius = radius;
    out->corner =
        lh_null_ne(shape) && corner > lh_ui_scalar(0) ? lh_ui_radius_clamp(shape, corner) : lh_ui_scalar(0);
    out->shape = out->corner > lh_ui_scalar(0) ? shape : lh_null;
    lh_return_if(radius < 0 || lh_null_eq(scratch) || out->width <= 0 || out->height <= 0, lh_bool_false);
    blur_band(pixmap, out->top, out->height, radius, lh_addr_of(out->y0), lh_addr_of(out->y1));
    return lh_bool_true;
}

lh_void
lh_ui_blur_column(lh_ui_pixmap_t *pixmap, const lh_ui_blur_pass_t *pass, lh_s32_t x)
{
    const lh_usize_t stride = lh_cast_static(lh_usize_t, pass->width) * 4U;
    /* Row `y` of column @p x is at `column + (y - y0) * stride`. */
    const lh_u8_t *column = pass->scratch + lh_cast_static(lh_usize_t, x) * 4U;
    lh_ui_blur_window_t sum = {0, 0, 0, 0};
    lh_ui_color_t out;
    lh_s32_t win0;
    lh_s32_t win1;
    lh_s32_t next0;
    lh_s32_t next1;
    lh_s32_t y;

    lh_ui_blur_window_span(pass->top, pass->radius, pass->y0, pass->y1, lh_addr_of(win0), lh_addr_of(win1));
    lh_ui_blur_window_take_samples(lh_addr_of(sum), column + lh_cast_static(lh_usize_t, win0 - pass->y0) * stride,
                                   stride, win1 - win0);
    for (y = pass->top; y < pass->top + pass->height; ++y)
    {
        lh_ui_blur_window_average_color(lh_addr_of(sum), win1 - win0, lh_addr_of(out));
        blur_put(pixmap, pass->left + x, y, lh_addr_of(out), pass->shape, pass->corner);
        lh_ui_blur_window_span(y + 1, pass->radius, pass->y0, pass->y1, lh_addr_of(next0), lh_addr_of(next1));
        lh_ui_blur_window_drop_samples(lh_addr_of(sum),
                                       column + lh_cast_static(lh_usize_t, win0 - pass->y0) * stride, stride,
                                       next0 - win0);
        lh_ui_blur_window_take_samples(lh_addr_of(sum),
                                       column + lh_cast_static(lh_usize_t, win1 - pass->y0) * stride, stride,
                                       next1 - win1);
        win0 = lh_math_max(win0, next0);
        win1 = lh_math_max(win1, next1);
    }
}

lh_void
lh_ui_blur_columns(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, const lh_u8_t *scratch,
                   const lh_ui_rect_t *shape, lh_ui_scalar_t corner)
{
    lh_ui_blur_pass_t pass;
    lh_s32_t x;

    lh_return_if(!lh_ui_blur_pass_init(pixmap, rect, radius, scratch, shape, corner, lh_addr_of(pass)));
    for (x = 0; x < pass.width; ++x)
    {
        lh_ui_blur_column(pixmap, lh_addr_of(pass), x);
    }
}

lh_void
lh_ui_blur_rect(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_u8_t *scratch)
{
    lh_ui_rect_t bounds;
    lh_ui_pixmap_get_bounds(pixmap, lh_addr_of(bounds));
    lh_ui_rect_t cut;
    lh_s32_t px;

    lh_return_if(lh_null_eq(pixmap) || lh_null_eq(rect) || radius <= 0 || lh_null_eq(scratch));
    lh_ui_rect_intersection(rect, &bounds, lh_addr_of(cut));
    px = lh_ui_scalar_floor_s32(radius);
    lh_return_if(lh_ui_blur_scratch_size(lh_addr_of(cut), px) == 0);
    lh_ui_blur_rows(pixmap, lh_addr_of(cut), px, scratch);
    lh_ui_blur_columns(pixmap, lh_addr_of(cut), px, scratch, lh_null, lh_ui_scalar(0));
}
