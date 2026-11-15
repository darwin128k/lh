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
#include <lh/gl/mask.h>

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
    const lh_gl_color_t *row = lh_gl_pixmap_get_pixel(pixmap, read0, y);

    lh_gl_blur_window_slide(row, 1U, read0, read1, radius, left, width, out, 1U);
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
lh_gl_blur_blend_covered(lh_gl_color_t *pixel, const lh_gl_color_t *frost,
                         lh_gl_color_channel_t cover, lh_bool_t replace)
{
    lh_assert_runtime_ref(pixel);
    lh_assert_runtime_ref(frost);
    if (cover == 0)
    {
        return;
    }
    if (replace == lh_bool_true && cover == 255)
    {
        lh_ptr_deref(pixel) = lh_ptr_deref(frost);
        return;
    }
    if (cover == 255)
    {
        lh_gl_color_over(pixel, frost, pixel);
        return;
    }
    lh_gl_color_t ink;

    lh_gl_color_scale_alpha(frost, cover, lh_addr_of(ink));
    lh_gl_color_over(pixel, lh_addr_of(ink), pixel);
}

lh_void
lh_gl_blur_column(lh_gl_pixmap_t *pixmap, const lh_gl_color_t *scratch, lh_gl_scalar_t left,
                  lh_gl_scalar_t top, lh_gl_scalar_t width, lh_gl_scalar_t height,
                  lh_gl_scalar_t radius, lh_gl_scalar_t y0, lh_gl_scalar_t y1, lh_gl_scalar_t x,
                  const lh_gl_rect_t *shape, lh_gl_scalar_t corner, const lh_gl_color_t *tint,
                  const lh_gl_mask_t *mask)
{
    const lh_gl_color_t *column = scratch + x;
    lh_usize_t stride = lh_type_cast(lh_usize_t, width);
    lh_gl_scalar_t pix_w = lh_gl_size_get_width(lh_gl_pixmap_get_size(pixmap));
    lh_gl_scalar_t y_from = top;
    lh_gl_scalar_t y_to = top + height;

    if (lh_ptr_is_set(shape) && corner > 0)
    {
        const lh_gl_point_t *origin = lh_gl_rect_get_origin(shape);
        const lh_gl_size_t *box = lh_gl_rect_get_size(shape);
        lh_gl_rect_t turned;

        lh_gl_rect_init(lh_addr_of(turned), lh_gl_point_get_y(origin), lh_gl_point_get_x(origin),
                        lh_gl_size_get_height(box), lh_gl_size_get_width(box));
        if (lh_gl_cover_round_rect_span(lh_addr_of(turned), corner, left + x, lh_addr_of(y_from),
                                        lh_addr_of(y_to)) == lh_bool_false)
        {
            return;
        }
        if (y_from < top)
        {
            y_from = top;
        }
        if (y_to > top + height)
        {
            y_to = top + height;
        }
        if (y_from >= y_to)
        {
            return;
        }
    }
    if (lh_ptr_is_set(mask))
    {
        for (lh_gl_scalar_t y = y_from; y < y_to;)
        {
            lh_gl_scalar_t count = y_to - y;

            if (count > 64)
            {
                count = 64;
            }
            lh_gl_color_t tmp[64];

            lh_gl_blur_window_slide(column, stride, y0, y1, radius, y, count, tmp, 1U);
            for (lh_gl_scalar_t i = 0; i < count; ++i)
            {
                lh_gl_color_t frost;
                lh_gl_color_channel_t cover = lh_gl_mask_coverage(mask, left + x, y + i);

                if (lh_ptr_is_set(tint))
                {
                    lh_gl_color_over(lh_addr_of(tmp[i]), tint, lh_addr_of(frost));
                }
                else
                {
                    frost = tmp[i];
                }
                lh_gl_blur_blend_covered(
                    lh_gl_pixmap_get_pixel_mut(pixmap, left + x, y + i), lh_addr_of(frost), cover,
                    lh_bool_true);
            }
            y = y + count;
        }
        return;
    }
    lh_gl_blur_window_slide(column, stride, y0, y1, radius, y_from, y_to - y_from,
                            lh_gl_pixmap_get_pixel_mut(pixmap, left + x, y_from),
                            lh_type_cast(lh_usize_t, pix_w));
    if (lh_ptr_is_null(tint))
    {
        return;
    }
    for (lh_gl_scalar_t y = y_from; y < y_to; ++y)
    {
        lh_gl_color_t *pixel = lh_gl_pixmap_get_pixel_mut(pixmap, left + x, y);
        lh_gl_color_t mixed;

        lh_gl_color_over(pixel, tint, lh_addr_of(mixed));
        lh_ptr_deref(pixel) = mixed;
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
                          y1, x, lh_null, 0, lh_null, lh_null);
    }
}

lh_void
lh_gl_glass(lh_gl_pixmap_t *pixmap, const lh_gl_rect_t *panel, lh_gl_scalar_t corner,
            lh_gl_scalar_t blur, const lh_gl_color_t *tint, lh_gl_color_t *scratch,
            const lh_gl_rect_t *clip, const lh_gl_mask_t *mask)
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
        if (corner <= 0)
        {
            if (lh_ptr_is_null(mask))
            {
                lh_gl_pixmap_fill_rect(pixmap, lh_addr_of(bounds), lh_addr_of(write), tint);
                return;
            }
            for (lh_gl_scalar_t y = top; y < top + height; ++y)
            {
                for (lh_gl_scalar_t x = left; x < left + width; ++x)
                {
                    lh_gl_blur_blend_covered(lh_gl_pixmap_get_pixel_mut(pixmap, x, y), tint,
                                             lh_gl_mask_coverage(mask, x, y), lh_bool_false);
                }
            }
            return;
        }
        for (lh_gl_scalar_t y = top; y < top + height; ++y)
        {
            lh_gl_scalar_t x0;
            lh_gl_scalar_t x1;

            if (lh_gl_cover_round_rect_span(panel, corner, y, lh_addr_of(x0), lh_addr_of(x1)) ==
                lh_bool_false)
            {
                continue;
            }
            if (x0 < left)
            {
                x0 = left;
            }
            if (x1 > left + width)
            {
                x1 = left + width;
            }
            if (x0 >= x1)
            {
                continue;
            }
            if (lh_ptr_is_set(mask))
            {
                for (lh_gl_scalar_t x = x0; x < x1; ++x)
                {
                    lh_gl_blur_blend_covered(lh_gl_pixmap_get_pixel_mut(pixmap, x, y), tint,
                                             lh_gl_mask_coverage(mask, x, y), lh_bool_false);
                }
                continue;
            }
            lh_gl_rect_t row;

            lh_gl_rect_init(lh_addr_of(row), x0, y, x1 - x0, 1);
            lh_gl_pixmap_fill_rect(pixmap, lh_addr_of(bounds), lh_addr_of(row), tint);
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
                          y1, x, shape, corner, tint, mask);
    }
}
