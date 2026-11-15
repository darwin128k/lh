/**
 * @file blur.c
 * @brief Two box passes over a ::lh_gl_pixmap_t, and the glass panel on top.
 */

#include <lh/gl/blur.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>
#include <lh/gl/blur/window.h>
#include <lh/gl/cover.h>

lh_usize_t
lh_gl_blur_scratch_count(const lh_gl_rect_t *rect, lh_gl_scalar_t radius)
{
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    lh_gl_scalar_t rows = lh_gl_size_get_height(size);

    if (lh_gl_rect_is_empty(rect))
    {
        return 0U;
    }
    if (radius > 0)
    {
        rows += radius * 2;
    }
    return lh_type_cast(lh_usize_t, lh_gl_size_get_width(size)) * lh_type_cast(lh_usize_t, rows);
}

lh_void
lh_gl_blur_band(const lh_gl_pixmap_t *pixmap, lh_gl_scalar_t top, lh_gl_scalar_t height,
                lh_gl_scalar_t radius, lh_gl_scalar_t *y0, lh_gl_scalar_t *y1)
{
    lh_gl_scalar_t pix_h = lh_gl_size_get_height(lh_gl_pixmap_get_size(pixmap));
    lh_gl_scalar_t last = top + height + lh_math_max(radius, 0);

    lh_assert_runtime_ref(y0);
    lh_assert_runtime_ref(y1);
    lh_ptr_deref(y0) = radius > 0 && top > radius ? top - radius : 0;
    if (last > pix_h)
    {
        last = pix_h;
    }
    lh_ptr_deref(y1) = last;
}

lh_void
lh_gl_blur_row(const lh_gl_pixmap_t *pixmap, lh_gl_scalar_t y, lh_gl_scalar_t left,
               lh_gl_scalar_t width, lh_gl_scalar_t radius, lh_gl_color_t *out)
{
    lh_gl_scalar_t pix_w = lh_gl_size_get_width(lh_gl_pixmap_get_size(pixmap));
    lh_gl_scalar_t read0 = radius > 0 && left > radius ? left - radius : 0;
    lh_gl_scalar_t read1 = left + width + lh_math_max(radius, 0);

    if (read1 > pix_w)
    {
        read1 = pix_w;
    }
    lh_gl_blur_window_t sum;

    lh_gl_blur_window_init(lh_addr_of(sum));
    lh_gl_scalar_t win0;
    lh_gl_scalar_t win1;

    lh_gl_blur_window_span(left, radius, read0, read1, lh_addr_of(win0), lh_addr_of(win1));
    for (lh_gl_scalar_t x = win0; x < win1; ++x)
    {
        lh_gl_blur_window_take(lh_addr_of(sum), lh_gl_pixmap_get_pixel(pixmap, x, y));
    }
    for (lh_gl_scalar_t x = left; x < left + width; ++x)
    {
        lh_gl_blur_window_average_color(lh_addr_of(sum), win1 - win0, out);
        ++out;
        lh_gl_scalar_t next0;
        lh_gl_scalar_t next1;

        lh_gl_blur_window_span(x + 1, radius, read0, read1, lh_addr_of(next0), lh_addr_of(next1));
        for (; win0 < next0; ++win0)
        {
            lh_gl_blur_window_drop(lh_addr_of(sum), lh_gl_pixmap_get_pixel(pixmap, win0, y));
        }
        for (; win1 < next1; ++win1)
        {
            lh_gl_blur_window_take(lh_addr_of(sum), lh_gl_pixmap_get_pixel(pixmap, win1, y));
        }
    }
}

lh_void
lh_gl_blur_rows(const lh_gl_pixmap_t *pixmap, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                lh_gl_color_t *scratch)
{
    const lh_gl_size_t *size = lh_gl_rect_get_size(rect);
    lh_gl_scalar_t left = lh_gl_point_get_x(lh_gl_rect_get_origin(rect));
    lh_gl_scalar_t top = lh_gl_point_get_y(lh_gl_rect_get_origin(rect));
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);

    lh_return_if(radius < 0 || lh_ptr_is_null(scratch) || width <= 0 || height <= 0);
    lh_gl_scalar_t y0;
    lh_gl_scalar_t y1;

    lh_gl_blur_band(pixmap, top, height, radius, lh_addr_of(y0), lh_addr_of(y1));
    for (lh_gl_scalar_t y = y0; y < y1; ++y)
    {
        lh_gl_blur_row(pixmap, y, left, width, radius, scratch + (y - y0) * width);
    }
}

lh_void
lh_gl_blur_put(lh_gl_pixmap_t *pixmap, lh_gl_scalar_t x, lh_gl_scalar_t y,
               const lh_gl_color_t *color, const lh_gl_rect_t *shape, lh_gl_scalar_t corner,
               const lh_gl_color_t *tint)
{
    lh_gl_color_t *pixel = lh_gl_pixmap_get_pixel_mut(pixmap, x, y);

    if (lh_ptr_is_set(shape) && corner > 0 && lh_gl_cover_round_rect(shape, corner, x, y) == 0)
    {
        return;
    }
    lh_gl_color_set_r(pixel, lh_gl_color_get_r(color));
    lh_gl_color_set_g(pixel, lh_gl_color_get_g(color));
    lh_gl_color_set_b(pixel, lh_gl_color_get_b(color));
    lh_gl_color_set_a(pixel, lh_gl_color_get_a(color));
    if (lh_ptr_is_set(tint))
    {
        lh_gl_color_over(pixel, tint, pixel);
    }
}

lh_void
lh_gl_blur_column(lh_gl_pixmap_t *pixmap, const lh_gl_color_t *scratch, lh_gl_scalar_t left,
                  lh_gl_scalar_t top, lh_gl_scalar_t width, lh_gl_scalar_t height,
                  lh_gl_scalar_t radius, lh_gl_scalar_t y0, lh_gl_scalar_t y1, lh_gl_scalar_t x,
                  const lh_gl_rect_t *shape, lh_gl_scalar_t corner, const lh_gl_color_t *tint)
{
    const lh_gl_color_t *column = scratch + x;
    lh_usize_t stride = lh_type_cast(lh_usize_t, width);
    lh_gl_blur_window_t sum;

    lh_gl_blur_window_init(lh_addr_of(sum));
    lh_gl_scalar_t win0;
    lh_gl_scalar_t win1;

    lh_gl_blur_window_span(top, radius, y0, y1, lh_addr_of(win0), lh_addr_of(win1));
    lh_gl_blur_window_take_colors(lh_addr_of(sum), column + (win0 - y0) * width, stride,
                                  win1 - win0);
    for (lh_gl_scalar_t y = top; y < top + height; ++y)
    {
        lh_gl_color_t out;

        lh_gl_blur_window_average_color(lh_addr_of(sum), win1 - win0, lh_addr_of(out));
        lh_gl_blur_put(pixmap, left + x, y, lh_addr_of(out), shape, corner, tint);
        lh_gl_scalar_t next0;
        lh_gl_scalar_t next1;

        lh_gl_blur_window_span(y + 1, radius, y0, y1, lh_addr_of(next0), lh_addr_of(next1));
        lh_gl_blur_window_drop_colors(lh_addr_of(sum), column + (win0 - y0) * width, stride,
                                      next0 - win0);
        lh_gl_blur_window_take_colors(lh_addr_of(sum), column + (win1 - y0) * width, stride,
                                      next1 - win1);
        win0 = lh_math_max(win0, next0);
        win1 = lh_math_max(win1, next1);
    }
}

lh_void
lh_gl_blur_rect(lh_gl_pixmap_t *pixmap, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                lh_gl_color_t *scratch)
{
    lh_return_if(lh_ptr_is_null(pixmap) || lh_ptr_is_null(rect) || radius <= 0 ||
                 lh_ptr_is_null(scratch));
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), 0, 0,
                    lh_gl_size_get_width(lh_gl_pixmap_get_size(pixmap)),
                    lh_gl_size_get_height(lh_gl_pixmap_get_size(pixmap)));
    lh_gl_rect_t cut;

    lh_gl_rect_intersection(rect, lh_addr_of(bounds), lh_addr_of(cut));
    lh_return_if(lh_gl_blur_scratch_count(lh_addr_of(cut), radius) == 0U);
    lh_gl_blur_rows(pixmap, lh_addr_of(cut), radius, scratch);
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(cut)));
    lh_gl_scalar_t y0;
    lh_gl_scalar_t y1;

    lh_gl_blur_band(pixmap, lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(cut))),
                    lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(cut))), radius,
                    lh_addr_of(y0), lh_addr_of(y1));
    for (lh_gl_scalar_t x = 0; x < width; ++x)
    {
        lh_gl_blur_column(pixmap, scratch,
                          lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(cut))),
                          lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(cut))), width,
                          lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(cut))), radius, y0,
                          y1, x, lh_null, 0, lh_null);
    }
}

lh_void
lh_gl_glass(lh_gl_pixmap_t *pixmap, const lh_gl_rect_t *panel, lh_gl_scalar_t corner,
            lh_gl_scalar_t blur, const lh_gl_color_t *tint, lh_gl_color_t *scratch,
            const lh_gl_rect_t *clip)
{
    lh_return_if(lh_ptr_is_null(pixmap) || lh_ptr_is_null(panel));
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), 0, 0,
                    lh_gl_size_get_width(lh_gl_pixmap_get_size(pixmap)),
                    lh_gl_size_get_height(lh_gl_pixmap_get_size(pixmap)));
    lh_gl_rect_t write;

    lh_gl_rect_intersection(panel, lh_addr_of(bounds), lh_addr_of(write));
    if (lh_ptr_is_set(clip))
    {
        lh_gl_rect_t limited;

        lh_gl_rect_intersection(lh_addr_of(write), clip, lh_addr_of(limited));
        write = limited;
    }
    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(write)));
    lh_return_if(blur > 0 && lh_ptr_is_null(scratch));
    if (blur <= 0)
    {
        lh_gl_scalar_t left = lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(write)));
        lh_gl_scalar_t top = lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(write)));
        lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(write)));
        lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(write)));

        lh_return_if(lh_ptr_is_null(tint));
        for (lh_gl_scalar_t y = top; y < top + height; ++y)
        {
            for (lh_gl_scalar_t x = left; x < left + width; ++x)
            {
                lh_gl_color_t kept;

                if (corner > 0 && lh_gl_cover_round_rect(panel, corner, x, y) == 0)
                {
                    continue;
                }
                kept = lh_ptr_deref(lh_gl_pixmap_get_pixel(pixmap, x, y));
                lh_gl_blur_put(pixmap, x, y, lh_addr_of(kept), lh_null, 0, tint);
            }
        }
        return;
    }
    lh_gl_blur_rows(pixmap, lh_addr_of(write), blur, scratch);
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(write)));
    const lh_gl_rect_t *shape = corner > 0 ? panel : lh_null;
    lh_gl_scalar_t y0;
    lh_gl_scalar_t y1;

    lh_gl_blur_band(pixmap, lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(write))),
                    lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(write))), blur,
                    lh_addr_of(y0), lh_addr_of(y1));
    for (lh_gl_scalar_t x = 0; x < width; ++x)
    {
        lh_gl_blur_column(pixmap, scratch,
                          lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(write))),
                          lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(write))), width,
                          lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(write))), blur, y0,
                          y1, x, shape, corner, tint);
    }
}
