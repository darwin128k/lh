/**
 * @file container.c
 * @brief Place a layer's children by borrowed scroll axes.
 */
#include <lh/gl/container.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/layer.h>

lh_gl_layer_t *
lh_gl_container_get_layer(const lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return self->layer;
}

lh_void
lh_gl_container_set_layer(lh_gl_container_t *self, lh_gl_layer_t *layer)
{
    lh_assert_runtime_ref(self);
    self->layer = layer;
}

const lh_gl_scroll_axis_t *
lh_gl_container_get_horizontal(const lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return self->horizontal;
}

lh_void
lh_gl_container_set_horizontal(lh_gl_container_t *self, const lh_gl_scroll_axis_t *horizontal)
{
    lh_assert_runtime_ref(self);
    self->horizontal = horizontal;
}

const lh_gl_scroll_axis_t *
lh_gl_container_get_vertical(const lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return self->vertical;
}

lh_void
lh_gl_container_set_vertical(lh_gl_container_t *self, const lh_gl_scroll_axis_t *vertical)
{
    lh_assert_runtime_ref(self);
    self->vertical = vertical;
}

lh_void
lh_gl_container_init(lh_gl_container_t *self, lh_gl_layer_t *layer,
                     const lh_gl_scroll_axis_t *horizontal, const lh_gl_scroll_axis_t *vertical)
{
    lh_gl_container_set_layer(self, layer);
    lh_gl_container_set_horizontal(self, horizontal);
    lh_gl_container_set_vertical(self, vertical);
}

lh_void
lh_gl_container_content_origin(const lh_gl_container_t *self, const lh_gl_point_t *viewport,
                               lh_gl_point_t *out)
{
    const lh_gl_scroll_axis_t *horizontal = lh_gl_container_get_horizontal(self);
    lh_gl_scalar_t dx = 0;

    if (lh_ptr_is_set(horizontal))
    {
        dx = lh_gl_scroll_axis_get_offset(horizontal);
    }
    const lh_gl_scroll_axis_t *vertical = lh_gl_container_get_vertical(self);
    lh_gl_scalar_t dy = 0;

    if (lh_ptr_is_set(vertical))
    {
        dy = lh_gl_scroll_axis_get_offset(vertical);
    }
    lh_gl_point_offset(viewport, -dx, -dy, out);
}

const lh_gl_entity_t *
lh_gl_container_find(const lh_gl_container_t *self, lh_gl_point_t point)
{
    lh_gl_layer_t *layer = lh_gl_container_get_layer(self);

    if (lh_ptr_is_null(layer))
    {
        return lh_null;
    }
    lh_gl_rect_t window;

    lh_gl_entity_get_absolute_rect(lh_gl_layer_get_entity(layer), lh_addr_of(window));
    lh_gl_point_t origin;

    lh_gl_container_content_origin(self, lh_gl_rect_get_origin(lh_addr_of(window)),
                                   lh_addr_of(origin));
    return lh_gl_layer_find_entity_at_origin(layer, point, lh_addr_of(window),
                                            lh_addr_of(origin));
}

lh_void
lh_gl_container_draw(const lh_gl_container_t *self, lh_gl_canvas_t *canvas,
                     const lh_gl_rect_t *clip)
{
    lh_gl_layer_t *layer = lh_gl_container_get_layer(self);

    if (lh_ptr_is_null(layer))
    {
        return;
    }
    lh_gl_rect_t window;

    lh_gl_entity_get_absolute_rect(lh_gl_layer_get_entity(layer), lh_addr_of(window));
    lh_gl_point_t origin;

    lh_gl_container_content_origin(self, lh_gl_rect_get_origin(lh_addr_of(window)),
                                   lh_addr_of(origin));
    lh_gl_layer_draw_at_origin(layer, canvas, lh_addr_of(window), clip, lh_addr_of(origin));
}

lh_void
lh_gl_container_invalidate(const lh_gl_container_t *self)
{
    lh_gl_layer_t *layer = lh_gl_container_get_layer(self);

    if (lh_ptr_is_null(layer))
    {
        return;
    }
    const lh_gl_rect_t *rect = lh_gl_entity_get_rect(lh_gl_layer_get_entity(layer));
    lh_gl_rect_t local;

    lh_gl_rect_init(lh_addr_of(local), 0, 0, lh_gl_size_get_width(lh_gl_rect_get_size(rect)),
                    lh_gl_size_get_height(lh_gl_rect_get_size(rect)));
    lh_gl_layer_invalidate(layer, lh_addr_of(local));
}
