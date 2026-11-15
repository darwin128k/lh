/**
 * @file pixmap.c
 * @brief Blend rectangle pixels in a borrowed color grid.
 */
#include <lh/gl/pixmap.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/memory.h>
#include <lh/gl/cover.h>

static const lh_gl_canvas_backend_t lh_gl_pixmap_canvas_backend =
    lh_gl_canvas_backend_initializer(lh_gl_pixmap_canvas_fill_rect, lh_gl_pixmap_canvas_stroke_rect,
                                     lh_gl_pixmap_canvas_fill_circle,
                                     lh_gl_pixmap_canvas_stroke_circle,
                                     lh_gl_pixmap_canvas_fill_round_rect,
                                     lh_gl_pixmap_canvas_stroke_round_rect,
                                     lh_gl_pixmap_canvas_fill_triangle,
                                     lh_gl_pixmap_canvas_fill_colors);

lh_gl_color_t *
lh_gl_pixmap_get_pixels_mut(lh_gl_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pixels;
}

const lh_gl_color_t *
lh_gl_pixmap_get_pixels(const lh_gl_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pixels;
}

lh_void
lh_gl_pixmap_set_pixels(lh_gl_pixmap_t *self, lh_gl_color_t *pixels)
{
    lh_assert_runtime_ref(self);
    self->pixels = pixels;
}

const lh_gl_size_t *
lh_gl_pixmap_get_size(const lh_gl_pixmap_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

lh_void
lh_gl_pixmap_set_size(lh_gl_pixmap_t *self, lh_gl_size_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

lh_void
lh_gl_pixmap_init(lh_gl_pixmap_t *self, lh_gl_color_t *pixels, lh_gl_size_t size)
{
    lh_gl_pixmap_set_pixels(self, pixels);
    lh_gl_pixmap_set_size(self, size);
}

lh_void
lh_gl_pixmap_store_words(const lh_gl_pixmap_t *self, lh_u32_t *words, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(words))
    {
        return;
    }
    const lh_gl_size_t *size = lh_gl_pixmap_get_size(self);
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);

    if (width <= 0 || height <= 0)
    {
        return;
    }
    lh_usize_t need = lh_type_cast(lh_usize_t, width) * lh_type_cast(lh_usize_t, height);

    if (count < need)
    {
        return;
    }
    for (lh_gl_scalar_t y = 0; y < height; ++y)
    {
        for (lh_gl_scalar_t x = 0; x < width; ++x)
        {
            const lh_gl_color_t *pixel = lh_gl_pixmap_get_pixel(self, x, y);
            lh_usize_t index =
                lh_type_cast(lh_usize_t, y) * lh_type_cast(lh_usize_t, width) +
                lh_type_cast(lh_usize_t, x);

            words[index] = lh_gl_color_get_word(pixel);
        }
    }
}

const lh_gl_color_t *
lh_gl_pixmap_get_pixel(const lh_gl_pixmap_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y)
{
    const lh_gl_size_t *size = lh_gl_pixmap_get_size(self);

    lh_assert_runtime(x >= 0 && y >= 0 && x < lh_gl_size_get_width(size) &&
                          y < lh_gl_size_get_height(size),
                      lh_runtime_error_code_out_of_range);
    const lh_gl_color_t *pixels = lh_gl_pixmap_get_pixels(self);

    if (lh_ptr_is_null(pixels))
    {
        return lh_null;
    }
    return pixels + (y * lh_gl_size_get_width(size) + x);
}

lh_gl_color_t *
lh_gl_pixmap_get_pixel_mut(lh_gl_pixmap_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y)
{
    return lh_ptr_cast(lh_gl_color_t, lh_gl_pixmap_get_pixel(self, x, y));
}

lh_bool_t
lh_gl_pixmap_limit(const lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                   lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(clip);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(out);
    if (lh_ptr_is_null(lh_gl_pixmap_get_pixels(self)))
    {
        return lh_bool_false;
    }
    const lh_gl_size_t *size = lh_gl_pixmap_get_size(self);
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), 0, 0, lh_gl_size_get_width(size),
                    lh_gl_size_get_height(size));
    lh_gl_rect_t visible;

    lh_gl_rect_intersection(rect, clip, lh_addr_of(visible));
    lh_gl_rect_intersection(lh_addr_of(visible), lh_addr_of(bounds), out);
    if (lh_gl_rect_is_empty(out))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_void
lh_gl_pixmap_fill_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                       const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(color);
    lh_gl_rect_t area;

    if (lh_gl_pixmap_limit(self, clip, rect, lh_addr_of(area)) == lh_bool_false)
    {
        return;
    }
    lh_gl_color_channel_t alpha = lh_gl_color_get_a(color);

    if (alpha == 0)
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(area));
    const lh_gl_size_t *area_size = lh_gl_rect_get_size(lh_addr_of(area));
    lh_gl_scalar_t x0 = lh_gl_point_get_x(origin);
    lh_gl_scalar_t y0 = lh_gl_point_get_y(origin);
    lh_gl_scalar_t row_width = lh_gl_size_get_width(area_size);
    lh_gl_scalar_t row_height = lh_gl_size_get_height(area_size);

    if (alpha == 255)
    {
        lh_u32_t word;

        lh_memory_std_copy(lh_addr_of(word), color, sizeof word);
        for (lh_gl_scalar_t row = 0; row < row_height; ++row)
        {
            lh_memory_set_u32(
                lh_ptr_rcast(lh_u32_t, lh_gl_pixmap_get_pixel_mut(self, x0, y0 + row)),
                lh_type_cast(lh_usize_t, row_width), word);
        }
        return;
    }
    for (lh_gl_scalar_t row = 0; row < row_height; ++row)
    {
        lh_gl_color_t *pixel = lh_gl_pixmap_get_pixel_mut(self, x0, y0 + row);

        for (lh_gl_scalar_t col = 0; col < row_width; ++col)
        {
            lh_gl_color_t mixed;

            lh_gl_color_over(pixel, color, lh_addr_of(mixed));
            lh_ptr_deref(pixel) = mixed;
            pixel = lh_ptr_add_by_offset(lh_gl_color_t, pixel, sizeof(lh_gl_color_t));
        }
    }
}

lh_void
lh_gl_pixmap_canvas_fill_rect(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                              const lh_gl_color_t *color)
{
    if (lh_ptr_is_null(target))
    {
        return;
    }
    lh_gl_pixmap_fill_rect(lh_ptr_cast(lh_gl_pixmap_t, target), clip, rect, color);
}

lh_void
lh_gl_pixmap_fill_colors(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, lh_gl_scalar_t x,
                         lh_gl_scalar_t y, const lh_gl_color_t *colors, lh_gl_scalar_t count)
{
    lh_assert_runtime_ref(clip);
    lh_assert_runtime_ref(colors);
    if (count <= 0 || lh_ptr_is_null(lh_gl_pixmap_get_pixels(self)))
    {
        return;
    }
    const lh_gl_point_t *clip_origin = lh_gl_rect_get_origin(clip);
    lh_gl_scalar_t clip_x = lh_gl_point_get_x(clip_origin);
    lh_gl_scalar_t clip_y = lh_gl_point_get_y(clip_origin);
    lh_gl_scalar_t clip_right = clip_x + lh_gl_size_get_width(lh_gl_rect_get_size(clip));
    lh_gl_scalar_t clip_bottom = clip_y + lh_gl_size_get_height(lh_gl_rect_get_size(clip));
    const lh_gl_size_t *size = lh_gl_pixmap_get_size(self);
    lh_gl_scalar_t pixmap_width = lh_gl_size_get_width(size);
    lh_gl_scalar_t pixmap_height = lh_gl_size_get_height(size);

    if (y < 0 || y < clip_y || y >= clip_bottom || y >= pixmap_height)
    {
        return;
    }
    lh_gl_scalar_t from = x;
    lh_gl_scalar_t to = x + count;

    if (from < 0)
    {
        from = 0;
    }
    if (from < clip_x)
    {
        from = clip_x;
    }
    if (to > clip_right)
    {
        to = clip_right;
    }
    if (to > pixmap_width)
    {
        to = pixmap_width;
    }
    if (from >= to)
    {
        return;
    }
    lh_gl_color_t *pixel = lh_gl_pixmap_get_pixel_mut(self, from, y);
    const lh_gl_color_t *sample =
        lh_ptr_add_by_offset(const lh_gl_color_t, colors,
                             (lh_uaddr_t)(from - x) * sizeof(lh_gl_color_t));

    for (lh_gl_scalar_t at = from; at < to;)
    {
        lh_gl_color_channel_t alpha = lh_gl_color_get_a(sample);

        if (alpha == 255)
        {
            const lh_gl_color_t *start = sample;
            lh_gl_scalar_t run = at;

            do
            {
                ++run;
                sample = lh_ptr_add_by_offset(const lh_gl_color_t, sample, sizeof(lh_gl_color_t));
            } while (run < to && lh_gl_color_get_a(sample) == 255);
            lh_memory_std_copy(pixel, start,
                               lh_type_cast(lh_usize_t, run - at) * sizeof(lh_gl_color_t));
            pixel = lh_ptr_add_by_offset(lh_gl_color_t, pixel,
                                         lh_type_cast(lh_uaddr_t, run - at) * sizeof(lh_gl_color_t));
            at = run;
            continue;
        }
        if (alpha != 0)
        {
            lh_gl_color_t mixed;

            lh_gl_color_over(pixel, sample, lh_addr_of(mixed));
            lh_ptr_deref(pixel) = mixed;
        }
        pixel = lh_ptr_add_by_offset(lh_gl_color_t, pixel, sizeof(lh_gl_color_t));
        sample = lh_ptr_add_by_offset(const lh_gl_color_t, sample, sizeof(lh_gl_color_t));
        ++at;
    }
}

lh_void
lh_gl_pixmap_canvas_fill_colors(lh_ptr target, const lh_gl_rect_t *clip, lh_gl_scalar_t x,
                                lh_gl_scalar_t y, const lh_gl_color_t *colors, lh_gl_scalar_t count)
{
    if (lh_ptr_is_null(target))
    {
        return;
    }
    lh_gl_pixmap_fill_colors(lh_ptr_cast(lh_gl_pixmap_t, target), clip, x, y, colors, count);
}

lh_void
lh_gl_pixmap_blend_pixel(lh_gl_pixmap_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y,
                         const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(color);
    if (lh_ptr_is_null(lh_gl_pixmap_get_pixels(self)))
    {
        return;
    }
    lh_gl_color_t mixed;

    lh_gl_color_over(lh_gl_pixmap_get_pixel(self, x, y), color, lh_addr_of(mixed));
    lh_ptr_deref(lh_gl_pixmap_get_pixel_mut(self, x, y)) = mixed;
}

lh_void
lh_gl_pixmap_fill_circle(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, lh_gl_point_t center,
                         lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(color);
    if (radius < 1)
    {
        return;
    }
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), lh_gl_point_get_x(lh_addr_of(center)) - radius,
                    lh_gl_point_get_y(lh_addr_of(center)) - radius, radius + radius,
                    radius + radius);
    lh_gl_rect_t area;

    if (lh_gl_pixmap_limit(self, clip, lh_addr_of(bounds), lh_addr_of(area)) == lh_bool_false)
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(area));
    const lh_gl_size_t *area_size = lh_gl_rect_get_size(lh_addr_of(area));

    for (lh_gl_scalar_t y = lh_gl_point_get_y(origin);
         y < lh_gl_point_get_y(origin) + lh_gl_size_get_height(area_size); ++y)
    {
        for (lh_gl_scalar_t x = lh_gl_point_get_x(origin);
             x < lh_gl_point_get_x(origin) + lh_gl_size_get_width(area_size); ++x)
        {
            lh_gl_color_channel_t cover = lh_gl_cover_circle(lh_addr_of(center), radius, x, y);

            if (cover == 0)
            {
                continue;
            }
            lh_gl_color_t ink;

            lh_gl_color_scale_alpha(color, cover, lh_addr_of(ink));
            lh_gl_pixmap_blend_pixel(self, x, y, lh_addr_of(ink));
        }
    }
}

lh_void
lh_gl_pixmap_canvas_fill_circle(lh_ptr target, const lh_gl_rect_t *clip, lh_gl_point_t center,
                                lh_gl_scalar_t radius, const lh_gl_color_t *color)
{
    if (lh_ptr_is_null(target))
    {
        return;
    }
    lh_gl_pixmap_fill_circle(lh_ptr_cast(lh_gl_pixmap_t, target), clip, center, radius, color);
}

lh_void
lh_gl_pixmap_fill_round_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip,
                             const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                             const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(color);
    lh_gl_rect_t area;

    if (lh_gl_pixmap_limit(self, clip, rect, lh_addr_of(area)) == lh_bool_false)
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(area));
    const lh_gl_size_t *area_size = lh_gl_rect_get_size(lh_addr_of(area));

    for (lh_gl_scalar_t y = lh_gl_point_get_y(origin);
         y < lh_gl_point_get_y(origin) + lh_gl_size_get_height(area_size); ++y)
    {
        for (lh_gl_scalar_t x = lh_gl_point_get_x(origin);
             x < lh_gl_point_get_x(origin) + lh_gl_size_get_width(area_size); ++x)
        {
            lh_gl_color_channel_t cover = lh_gl_cover_round_rect(rect, radius, x, y);

            if (cover == 0)
            {
                continue;
            }
            lh_gl_color_t ink;

            lh_gl_color_scale_alpha(color, cover, lh_addr_of(ink));
            lh_gl_pixmap_blend_pixel(self, x, y, lh_addr_of(ink));
        }
    }
}

lh_void
lh_gl_pixmap_canvas_fill_round_rect(lh_ptr target, const lh_gl_rect_t *clip,
                                    const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                                    const lh_gl_color_t *color)
{
    if (lh_ptr_is_null(target))
    {
        return;
    }
    lh_gl_pixmap_fill_round_rect(lh_ptr_cast(lh_gl_pixmap_t, target), clip, rect, radius, color);
}

lh_void
lh_gl_pixmap_fill_triangle(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_point_t *a,
                           const lh_gl_point_t *b, const lh_gl_point_t *c,
                           const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    lh_assert_runtime_ref(c);
    lh_assert_runtime_ref(color);
    lh_gl_rect_t bounds;

    lh_gl_rect_init(
        lh_addr_of(bounds),
        lh_math_min(lh_gl_point_get_x(a), lh_math_min(lh_gl_point_get_x(b), lh_gl_point_get_x(c))),
        lh_math_min(lh_gl_point_get_y(a), lh_math_min(lh_gl_point_get_y(b), lh_gl_point_get_y(c))),
        lh_math_max(lh_gl_point_get_x(a), lh_math_max(lh_gl_point_get_x(b), lh_gl_point_get_x(c))) -
            lh_math_min(lh_gl_point_get_x(a),
                        lh_math_min(lh_gl_point_get_x(b), lh_gl_point_get_x(c))),
        lh_math_max(lh_gl_point_get_y(a), lh_math_max(lh_gl_point_get_y(b), lh_gl_point_get_y(c))) -
            lh_math_min(lh_gl_point_get_y(a),
                        lh_math_min(lh_gl_point_get_y(b), lh_gl_point_get_y(c))));
    lh_gl_rect_t area;

    if (lh_gl_pixmap_limit(self, clip, lh_addr_of(bounds), lh_addr_of(area)) == lh_bool_false)
    {
        return;
    }
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(area));
    const lh_gl_size_t *area_size = lh_gl_rect_get_size(lh_addr_of(area));

    for (lh_gl_scalar_t y = lh_gl_point_get_y(origin);
         y < lh_gl_point_get_y(origin) + lh_gl_size_get_height(area_size); ++y)
    {
        for (lh_gl_scalar_t x = lh_gl_point_get_x(origin);
             x < lh_gl_point_get_x(origin) + lh_gl_size_get_width(area_size); ++x)
        {
            lh_gl_color_channel_t cover = lh_gl_cover_triangle(a, b, c, x, y);

            if (cover == 0)
            {
                continue;
            }
            lh_gl_color_t ink;

            lh_gl_color_scale_alpha(color, cover, lh_addr_of(ink));
            lh_gl_pixmap_blend_pixel(self, x, y, lh_addr_of(ink));
        }
    }
}

lh_void
lh_gl_pixmap_canvas_fill_triangle(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_point_t *a,
                                  const lh_gl_point_t *b, const lh_gl_point_t *c,
                                  const lh_gl_color_t *color)
{
    if (lh_ptr_is_null(target))
    {
        return;
    }
    lh_gl_pixmap_fill_triangle(lh_ptr_cast(lh_gl_pixmap_t, target), clip, a, b, c, color);
}

lh_void
lh_gl_pixmap_blend_outside(lh_gl_pixmap_t *self, lh_gl_scalar_t x, lh_gl_scalar_t y,
                           lh_gl_color_channel_t outer, lh_gl_color_channel_t inner,
                           const lh_gl_color_t *color)
{
    if (outer == 0 || inner != 0)
    {
        return;
    }
    lh_gl_color_t ink;

    lh_gl_color_scale_alpha(color, outer, lh_addr_of(ink));
    lh_gl_pixmap_blend_pixel(self, x, y, lh_addr_of(ink));
}

lh_void
lh_gl_pixmap_stroke_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                         const lh_gl_pen_t *pen)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(pen);
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (width < 1)
    {
        return;
    }
    lh_gl_rect_t outer;

    lh_gl_rect_inset(rect, -width, -width, lh_addr_of(outer));
    lh_gl_rect_t area;

    if (lh_gl_pixmap_limit(self, clip, lh_addr_of(outer), lh_addr_of(area)) == lh_bool_false)
    {
        return;
    }
    const lh_gl_color_t *color = lh_gl_pen_get_color(pen);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(area));
    const lh_gl_size_t *area_size = lh_gl_rect_get_size(lh_addr_of(area));

    for (lh_gl_scalar_t y = lh_gl_point_get_y(origin);
         y < lh_gl_point_get_y(origin) + lh_gl_size_get_height(area_size); ++y)
    {
        for (lh_gl_scalar_t x = lh_gl_point_get_x(origin);
             x < lh_gl_point_get_x(origin) + lh_gl_size_get_width(area_size); ++x)
        {
            lh_gl_pixmap_blend_outside(self, x, y,
                                       lh_gl_cover_round_rect(lh_addr_of(outer), 0, x, y),
                                       lh_gl_cover_round_rect(rect, 0, x, y), color);
        }
    }
}

lh_void
lh_gl_pixmap_canvas_stroke_rect(lh_ptr target, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                                const lh_gl_pen_t *pen)
{
    if (lh_ptr_is_null(target))
    {
        return;
    }
    lh_gl_pixmap_stroke_rect(lh_ptr_cast(lh_gl_pixmap_t, target), clip, rect, pen);
}

lh_void
lh_gl_pixmap_stroke_circle(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, lh_gl_point_t center,
                           lh_gl_scalar_t radius, const lh_gl_pen_t *pen)
{
    lh_assert_runtime_ref(pen);
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (radius < 1 || width < 1)
    {
        return;
    }
    lh_gl_scalar_t outer_radius = radius + width;
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), lh_gl_point_get_x(lh_addr_of(center)) - outer_radius,
                    lh_gl_point_get_y(lh_addr_of(center)) - outer_radius,
                    outer_radius + outer_radius, outer_radius + outer_radius);
    lh_gl_rect_t area;

    if (lh_gl_pixmap_limit(self, clip, lh_addr_of(bounds), lh_addr_of(area)) == lh_bool_false)
    {
        return;
    }
    const lh_gl_color_t *color = lh_gl_pen_get_color(pen);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(area));
    const lh_gl_size_t *area_size = lh_gl_rect_get_size(lh_addr_of(area));

    for (lh_gl_scalar_t y = lh_gl_point_get_y(origin);
         y < lh_gl_point_get_y(origin) + lh_gl_size_get_height(area_size); ++y)
    {
        for (lh_gl_scalar_t x = lh_gl_point_get_x(origin);
             x < lh_gl_point_get_x(origin) + lh_gl_size_get_width(area_size); ++x)
        {
            lh_gl_pixmap_blend_outside(self, x, y,
                                       lh_gl_cover_circle(lh_addr_of(center), outer_radius, x, y),
                                       lh_gl_cover_circle(lh_addr_of(center), radius, x, y), color);
        }
    }
}

lh_void
lh_gl_pixmap_canvas_stroke_circle(lh_ptr target, const lh_gl_rect_t *clip, lh_gl_point_t center,
                                  lh_gl_scalar_t radius, const lh_gl_pen_t *pen)
{
    if (lh_ptr_is_null(target))
    {
        return;
    }
    lh_gl_pixmap_stroke_circle(lh_ptr_cast(lh_gl_pixmap_t, target), clip, center, radius, pen);
}

lh_void
lh_gl_pixmap_stroke_round_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip,
                               const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                               const lh_gl_pen_t *pen)
{
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(pen);
    lh_gl_scalar_t width = lh_gl_pen_get_width(pen);

    if (width < 1)
    {
        return;
    }
    lh_gl_rect_t outer;

    lh_gl_rect_inset(rect, -width, -width, lh_addr_of(outer));
    lh_gl_rect_t area;

    if (lh_gl_pixmap_limit(self, clip, lh_addr_of(outer), lh_addr_of(area)) == lh_bool_false)
    {
        return;
    }
    const lh_gl_color_t *color = lh_gl_pen_get_color(pen);
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(area));
    const lh_gl_size_t *area_size = lh_gl_rect_get_size(lh_addr_of(area));

    for (lh_gl_scalar_t y = lh_gl_point_get_y(origin);
         y < lh_gl_point_get_y(origin) + lh_gl_size_get_height(area_size); ++y)
    {
        for (lh_gl_scalar_t x = lh_gl_point_get_x(origin);
             x < lh_gl_point_get_x(origin) + lh_gl_size_get_width(area_size); ++x)
        {
            lh_gl_pixmap_blend_outside(self, x, y,
                                       lh_gl_cover_round_rect(lh_addr_of(outer), radius + width, x,
                                                              y),
                                       lh_gl_cover_round_rect(rect, radius, x, y), color);
        }
    }
}

lh_void
lh_gl_pixmap_canvas_stroke_round_rect(lh_ptr target, const lh_gl_rect_t *clip,
                                      const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                                      const lh_gl_pen_t *pen)
{
    if (lh_ptr_is_null(target))
    {
        return;
    }
    lh_gl_pixmap_stroke_round_rect(lh_ptr_cast(lh_gl_pixmap_t, target), clip, rect, radius, pen);
}

const lh_gl_canvas_backend_t *
lh_gl_pixmap_get_canvas_backend(void)
{
    return lh_addr_of(lh_gl_pixmap_canvas_backend);
}
