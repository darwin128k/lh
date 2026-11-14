/**
 * @file pixmap.c
 * @brief Replace rectangle pixels in a borrowed color grid.
 */
#include <lh/gl/pixmap.h>

#include <lh/core/assert/runtime.h>

static const lh_gl_canvas_backend_t lh_gl_pixmap_canvas_backend =
    lh_gl_canvas_backend_initializer(lh_gl_pixmap_canvas_fill_rect, lh_null, lh_null, lh_null,
                                     lh_null, lh_null);

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

lh_void
lh_gl_pixmap_fill_rect(lh_gl_pixmap_t *self, const lh_gl_rect_t *clip, const lh_gl_rect_t *rect,
                       const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(clip);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(color);
    if (lh_ptr_is_null(lh_gl_pixmap_get_pixels(self)))
    {
        return;
    }
    const lh_gl_size_t *size = lh_gl_pixmap_get_size(self);
    lh_gl_rect_t bounds;

    lh_gl_rect_init(lh_addr_of(bounds), 0, 0, lh_gl_size_get_width(size),
                    lh_gl_size_get_height(size));
    lh_gl_rect_t visible;

    lh_gl_rect_intersection(rect, clip, lh_addr_of(visible));
    lh_gl_rect_t area;

    lh_gl_rect_intersection(lh_addr_of(visible), lh_addr_of(bounds), lh_addr_of(area));
    if (lh_gl_rect_is_empty(lh_addr_of(area)))
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
            lh_ptr_deref(lh_gl_pixmap_get_pixel_mut(self, x, y)) = lh_ptr_deref(color);
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

const lh_gl_canvas_backend_t *
lh_gl_pixmap_get_canvas_backend(void)
{
    return lh_addr_of(lh_gl_pixmap_canvas_backend);
}
