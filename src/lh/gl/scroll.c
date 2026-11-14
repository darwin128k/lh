/**
 * @file scroll.c
 * @brief Shift the direct children of one borrowed layer.
 */
#include <lh/gl/scroll.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/layer.h>

lh_gl_layer_t *
lh_gl_scroll_get_layer(const lh_gl_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->layer;
}

lh_void
lh_gl_scroll_set_layer(lh_gl_scroll_t *self, lh_gl_layer_t *layer)
{
    lh_assert_runtime_ref(self);
    self->layer = layer;
}

lh_void
lh_gl_scroll_init(lh_gl_scroll_t *self, lh_gl_layer_t *layer)
{
    lh_gl_scroll_set_layer(self, layer);
}

lh_void
lh_gl_scroll_by(lh_gl_scroll_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy)
{
    lh_gl_layer_t *layer = lh_gl_scroll_get_layer(self);

    if (lh_ptr_is_null(layer) || (dx == 0 && dy == 0))
    {
        return;
    }
    lh_list_t *items = lh_gl_layer_get_items_mut(layer);

    for (lh_list_node_t *node = lh_list_get_first(items); lh_ptr_is_set(node);
         node = lh_list_get_next(items, node))
    {
        lh_gl_entity_t *child = lh_gl_entity_from_node(node);
        const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_gl_entity_get_rect(child));
        lh_gl_point_t moved;

        lh_gl_point_offset(origin, dx, dy, lh_addr_of(moved));
        lh_gl_entity_set_origin(child, moved);
    }
}
