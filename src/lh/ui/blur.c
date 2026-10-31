/**
 * @file blur.c
 * @brief Implementation of `lh/ui/blur.h`.
 */

#include <lh/math.h>
#include <lh/ui/blur.h>
#include <lh/ui/radius.h>
#include <lh/util/return.h>

lh_usize_t
lh_ui_blur_scratch_size(const lh_ui_rect_t *rect, lh_s32_t radius)
{
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);
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
    return (lh_usize_t)lh_ui_size_get_width(size) * lh_cast_static(lh_usize_t, rows) * 4U;
}

lh_void
lh_ui_blur_band(const lh_ui_pixmap_t *pixmap, lh_s32_t top, lh_s32_t height, lh_s32_t radius,
                lh_s32_t *y0, lh_s32_t *y1)
{
    const lh_s32_t pix_h = lh_ui_pixmap_get_height(pixmap);
    lh_s32_t last = top + height + (radius > 0 ? radius : 0);

    lh_ptr_deref(y0) = radius > 0 && top > radius ? top - radius : 0;
    if (last > pix_h)
    {
        last = pix_h;
    }
    lh_ptr_deref(y1) = last;
}

lh_void
lh_ui_blur_row_x(const lh_ui_pixmap_t *pixmap, lh_s32_t y, lh_s32_t left, lh_s32_t width,
                 lh_s32_t radius, lh_u8_t *out)
{
    const lh_s32_t pix_w = lh_ui_pixmap_get_width(pixmap);
    const lh_s32_t read0 = radius > 0 && left > radius ? left - radius : 0;
    lh_s32_t read1 = left + width + (radius > 0 ? radius : 0);
    lh_ui_blur_window_t sum = {0, 0, 0, 0};

    if (read1 > pix_w)
    {
        read1 = pix_w;
    }
    lh_s32_t win0 = left - radius;
    lh_s32_t win1 = left + radius + 1;
    if (win0 < read0)
    {
        win0 = read0;
    }
    if (win1 > read1)
    {
        win1 = read1;
    }
    lh_s32_t x;
    for (x = win0; x < win1; ++x)
    {
        lh_ui_color_t pixel;
        lh_ui_pixmap_get_pixel(pixmap, x, y, lh_addr_of(pixel));

        lh_ui_blur_window_take(lh_addr_of(sum), lh_addr_of(pixel));
    }
    for (x = left; x < left + width; ++x)
    {
        lh_s32_t next0 = x + 1 - radius;
        lh_s32_t next1 = x + 1 + radius + 1;

        out[0] = lh_ui_blur_window_average(lh_addr_of(sum), 0, win1 - win0);
        out[1] = lh_ui_blur_window_average(lh_addr_of(sum), 1, win1 - win0);
        out[2] = lh_ui_blur_window_average(lh_addr_of(sum), 2, win1 - win0);
        out[3] = lh_ui_blur_window_average(lh_addr_of(sum), 3, win1 - win0);
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

            lh_ui_blur_window_drop(lh_addr_of(sum), lh_addr_of(gone));
        }
        for (; win1 < next1; ++win1)
        {
            lh_ui_color_t in;
            lh_ui_pixmap_get_pixel(pixmap, win1, y, lh_addr_of(in));

            lh_ui_blur_window_take(lh_addr_of(sum), lh_addr_of(in));
        }
    }
}

lh_void
lh_ui_blur_rows(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, lh_u8_t *scratch)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size(rect);
    const lh_s32_t left = lh_ui_point_get_x(origin);
    const lh_s32_t top = lh_ui_point_get_y(origin);
    const lh_s32_t width = lh_ui_size_get_width(size);
    const lh_s32_t height = lh_ui_size_get_height(size);

    lh_return_if(radius < 0 || lh_null_eq(scratch) || width <= 0 || height <= 0);
    {
        lh_s32_t y0;
        lh_s32_t y1;

        lh_ui_blur_band(pixmap, top, height, radius, lh_addr_of(y0), lh_addr_of(y1));
        for (lh_s32_t y = y0; y < y1; ++y)
        {
            lh_ui_blur_row_x(pixmap, y, left, width, radius, scratch + (y - y0) * width * 4);
        }
    }
}

lh_void
lh_ui_blur_put(lh_ui_pixmap_t *pixmap, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color,
               const lh_ui_rect_t *shape, lh_ui_scalar_t corner)
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
lh_ui_blur_column(lh_ui_pixmap_t *pixmap, const lh_ui_blur_pass_t *pass, lh_s32_t x)
{
    const lh_usize_t stride = lh_cast_static(lh_usize_t, lh_ui_blur_pass_get_width(pass)) * 4U;
    /* Row `y` of column @p x is at `column + (y - y0) * stride`. */
    const lh_u8_t *column = lh_ui_blur_pass_get_scratch(pass) + lh_cast_static(lh_usize_t, x) * 4U;
    lh_ui_blur_window_t sum = {0, 0, 0, 0};

    {
        lh_s32_t win0;
        lh_s32_t win1;

        lh_ui_blur_window_span(lh_ui_blur_pass_get_top(pass), lh_ui_blur_pass_get_radius(pass),
                               lh_ui_blur_pass_get_y0(pass), lh_ui_blur_pass_get_y1(pass),
                               lh_addr_of(win0), lh_addr_of(win1));
        lh_ui_blur_window_take_samples(
            lh_addr_of(sum),
            column + lh_cast_static(lh_usize_t, win0 - lh_ui_blur_pass_get_y0(pass)) * stride,
            stride, win1 - win0);
        for (lh_s32_t y = lh_ui_blur_pass_get_top(pass);
             y < lh_ui_blur_pass_get_top(pass) + lh_ui_blur_pass_get_height(pass); ++y)
        {
            lh_ui_color_t out;
            lh_ui_blur_window_average_color(lh_addr_of(sum), win1 - win0, lh_addr_of(out));
            lh_ui_blur_put(pixmap, lh_ui_blur_pass_get_left(pass) + x, y, lh_addr_of(out),
                           lh_ui_blur_pass_get_shape(pass), lh_ui_blur_pass_get_corner(pass));
            lh_s32_t next0;
            lh_s32_t next1;
            lh_ui_blur_window_span(y + 1, lh_ui_blur_pass_get_radius(pass),
                                   lh_ui_blur_pass_get_y0(pass), lh_ui_blur_pass_get_y1(pass),
                                   lh_addr_of(next0), lh_addr_of(next1));
            lh_ui_blur_window_drop_samples(
                lh_addr_of(sum),
                column + lh_cast_static(lh_usize_t, win0 - lh_ui_blur_pass_get_y0(pass)) * stride,
                stride, next0 - win0);
            lh_ui_blur_window_take_samples(
                lh_addr_of(sum),
                column + lh_cast_static(lh_usize_t, win1 - lh_ui_blur_pass_get_y0(pass)) * stride,
                stride, next1 - win1);
            win0 = lh_math_max(win0, next0);
            win1 = lh_math_max(win1, next1);
        }
    }
}

lh_void
lh_ui_blur_columns(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius,
                   const lh_u8_t *scratch, const lh_ui_rect_t *shape, lh_ui_scalar_t corner)
{
    lh_ui_blur_pass_t pass;

    lh_return_if(
        !lh_ui_blur_pass_init(pixmap, rect, radius, scratch, shape, corner, lh_addr_of(pass)));
    for (lh_s32_t x = 0; x < lh_ui_blur_pass_get_width(lh_addr_of(pass)); ++x)
    {
        lh_ui_blur_column(pixmap, lh_addr_of(pass), x);
    }
}

lh_void
lh_ui_blur_rect(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                lh_u8_t *scratch)
{
    lh_ui_rect_t bounds;
    lh_ui_pixmap_get_bounds(pixmap, lh_addr_of(bounds));

    lh_return_if(lh_null_eq(pixmap) || lh_null_eq(rect) || radius <= 0 || lh_null_eq(scratch));
    lh_ui_rect_t cut;
    lh_ui_rect_intersection(rect, lh_addr_of(bounds), lh_addr_of(cut));
    lh_s32_t px = lh_ui_scalar_floor_s32(radius);
    lh_return_if(lh_ui_blur_scratch_size(lh_addr_of(cut), px) == 0);
    lh_ui_blur_rows(pixmap, lh_addr_of(cut), px, scratch);
    lh_ui_blur_columns(pixmap, lh_addr_of(cut), px, scratch, lh_null, lh_ui_scalar(0));
}
