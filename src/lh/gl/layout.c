/**
 * @file layout.c
 * @brief Place a layer's direct children with one gap.
 */
#include <lh/gl/layout.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/layer.h>

lh_gl_scalar_t
lh_gl_layout_get_gap(const lh_gl_layout_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gap;
}

lh_void
lh_gl_layout_set_gap(lh_gl_layout_t *self, lh_gl_scalar_t gap)
{
    lh_assert_runtime_ref(self);
    self->gap = gap;
}

lh_void
lh_gl_layout_init(lh_gl_layout_t *self, lh_gl_scalar_t gap)
{
    lh_gl_layout_set_gap(self, gap);
}

lh_void
lh_gl_layout_pack(const lh_gl_layout_t *self, lh_gl_layer_t *layer, lh_bool_t horizontal)
{
    lh_gl_scalar_t gap = lh_gl_layout_get_gap(self);

    if (lh_ptr_is_null(layer))
    {
        return;
    }
    lh_gl_scalar_t cursor = 0;

    for (lh_list_node_t *node = lh_list_get_first(lh_gl_layer_get_items_mut(layer));
         lh_ptr_is_set(node); node = lh_list_get_next(lh_gl_layer_get_items_mut(layer), node))
    {
        lh_gl_entity_t *child = lh_gl_entity_from_node(node);
        const lh_gl_rect_t *rect = lh_gl_entity_get_rect(child);
        lh_gl_point_t origin = lh_ptr_deref(lh_gl_rect_get_origin(rect));

        if (horizontal == lh_bool_true)
        {
            lh_gl_point_set_x(lh_addr_of(origin), cursor);
            cursor += lh_gl_size_get_width(lh_gl_rect_get_size(rect)) + gap;
        }
        else
        {
            lh_gl_point_set_y(lh_addr_of(origin), cursor);
            cursor += lh_gl_size_get_height(lh_gl_rect_get_size(rect)) + gap;
        }
        lh_gl_entity_set_origin(child, origin);
    }
}
