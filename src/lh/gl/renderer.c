/**
 * @file renderer.c
 * @brief One deferred frame over a surface's dirty rectangles.
 */
#include <lh/gl/renderer.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/layer.h>
#include <lh/gl/surface.h>

lh_gl_surface_t *
lh_gl_renderer_get_surface(const lh_gl_renderer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->surface;
}

lh_void
lh_gl_renderer_set_surface(lh_gl_renderer_t *self, lh_gl_surface_t *surface)
{
    lh_assert_runtime_ref(self);
    self->surface = surface;
}

lh_gl_canvas_t *
lh_gl_renderer_get_canvas(const lh_gl_renderer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas;
}

lh_void
lh_gl_renderer_set_canvas(lh_gl_renderer_t *self, lh_gl_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    self->canvas = canvas;
}

lh_gl_scalar_t
lh_gl_renderer_get_band(const lh_gl_renderer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->band;
}

lh_void
lh_gl_renderer_set_band(lh_gl_renderer_t *self, lh_gl_scalar_t band)
{
    lh_assert_runtime_ref(self);
    self->band = band;
}

lh_void
lh_gl_renderer_init(lh_gl_renderer_t *self, lh_gl_surface_t *surface, lh_gl_canvas_t *canvas,
                    lh_gl_scalar_t band)
{
    lh_gl_renderer_set_surface(self, surface);
    lh_gl_renderer_set_canvas(self, canvas);
    lh_gl_renderer_set_band(self, band);
}

lh_void
lh_gl_renderer_draw(lh_gl_renderer_t *self, const lh_gl_rect_t *area, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    if (count == 0U)
    {
        return;
    }
    lh_assert_runtime_ref(area);

    lh_gl_surface_t *surface = lh_gl_renderer_get_surface(self);

    if (lh_ptr_is_null(surface))
    {
        return;
    }
    lh_gl_layer_t *root = lh_gl_surface_get_root(surface);

    if (lh_ptr_is_null(root))
    {
        return;
    }
    lh_gl_canvas_t *canvas = lh_gl_renderer_get_canvas(self);

    lh_assert_runtime_ref(canvas);

    lh_gl_scalar_t band = lh_gl_renderer_get_band(self);

    for (lh_uindex_t i = 0U; i < count; ++i)
    {
        const lh_gl_rect_t *one =
            lh_ptr_add_by_offset(const lh_gl_rect_t, area, i * sizeof(lh_gl_rect_t));
        const lh_gl_size_t *size = lh_gl_rect_get_size(one);
        lh_gl_scalar_t height = lh_gl_size_get_height(size);

        if (band <= 0 || band >= height)
        {
            lh_gl_layer_draw(root, canvas, one);
            continue;
        }
        lh_gl_scalar_t y = lh_gl_point_get_y(lh_gl_rect_get_origin(one));
        lh_gl_scalar_t y_end = y + height;

        while (y < y_end)
        {
            lh_gl_scalar_t stripe = lh_math_min(band, y_end - y);
            lh_gl_rect_t clip;

            lh_gl_rect_init(lh_addr_of(clip), lh_gl_point_get_x(lh_gl_rect_get_origin(one)), y,
                            lh_gl_size_get_width(size), stripe);
            lh_gl_layer_draw(root, canvas, lh_addr_of(clip));
            y += stripe;
        }
    }
}

lh_void
lh_gl_renderer_render(lh_gl_renderer_t *self)
{
    lh_gl_surface_t *surface = lh_gl_renderer_get_surface(self);

    if (lh_ptr_is_null(surface) || lh_ptr_is_null(lh_gl_surface_get_root(surface)))
    {
        return;
    }
    lh_usize_t count = lh_gl_surface_get_dirty_count(surface);
    lh_gl_rect_t areas[LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX];

    for (lh_uindex_t i = 0U; i < count; ++i)
    {
        areas[i] = lh_ptr_deref(lh_gl_surface_get_dirty(surface, i));
    }
    lh_gl_renderer_draw(self, areas, count);
    lh_gl_surface_clear_dirty(surface);
}
