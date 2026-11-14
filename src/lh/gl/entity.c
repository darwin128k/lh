/**
 * @file entity.c
 * @brief Implementation of lh/gl/entity.h.
 *
 * Only the level-1 accessors name a field; everything else goes through them
 * and through the methods of the class and the rectangle.
 */

#include <lh/gl/entity.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/layer.h>

/* -- Accessors ------------------------------------------------------------- */

lh_gl_rect_t *
lh_gl_entity_get_rect_mut(lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

const lh_gl_rect_t *
lh_gl_entity_get_rect(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_void
lh_gl_entity_set_rect(lh_gl_entity_t *self, lh_gl_rect_t rect)
{
    lh_gl_entity_invalidate(self);
    lh_ptr_deref(lh_gl_entity_get_rect_mut(self)) = rect;
    lh_gl_entity_invalidate(self);
}

lh_gl_layer_t *
lh_gl_entity_get_parent(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->parent;
}

lh_void
lh_gl_entity_set_parent(lh_gl_entity_t *self, lh_gl_layer_t *parent)
{
    lh_assert_runtime_ref(self);
    self->parent = parent;
}

const lh_gl_entity_class_t *
lh_gl_entity_get_class(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cls;
}

lh_void
lh_gl_entity_set_class(lh_gl_entity_t *self, const lh_gl_entity_class_t *cls)
{
    lh_assert_runtime_ref(self);
    self->cls = cls;
}

lh_list_node_t *
lh_gl_entity_get_node_mut(lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

const lh_list_node_t *
lh_gl_entity_get_node(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_gl_entity_t *
lh_gl_entity_from_node(const lh_list_node_t *node)
{
    return lh_list_entry(lh_gl_entity_t, node, node);
}

/* -- Init and queries, by level -------------------------------------------- */

lh_void
lh_gl_entity_init(lh_gl_entity_t *self, lh_gl_rect_t rect, const lh_gl_entity_class_t *cls)
{
    lh_gl_entity_set_parent(self, lh_null);
    lh_gl_entity_set_rect(self, rect);
    lh_gl_entity_set_class(self, cls);
    lh_list_node_init(lh_gl_entity_get_node_mut(self));
}

lh_void
lh_gl_entity_invalidate(lh_gl_entity_t *self)
{
    lh_gl_layer_t *parent = lh_gl_entity_get_parent(self);

    if (lh_ptr_is_set(parent))
    {
        lh_gl_layer_invalidate(parent, lh_gl_entity_get_rect(self));
    }
}

lh_void
lh_gl_entity_set_origin(lh_gl_entity_t *self, lh_gl_point_t origin)
{
    lh_gl_rect_t rect = lh_ptr_deref(lh_gl_entity_get_rect(self));

    lh_gl_rect_set_origin(lh_addr_of(rect), origin);
    lh_gl_entity_set_rect(self, rect);
}

lh_void
lh_gl_entity_set_size(lh_gl_entity_t *self, lh_gl_size_t size)
{
    lh_gl_rect_t rect = lh_ptr_deref(lh_gl_entity_get_rect(self));

    lh_gl_rect_set_size(lh_addr_of(rect), size);
    lh_gl_entity_set_rect(self, rect);
}

lh_gl_entity_class_kind_t
lh_gl_entity_get_kind(const lh_gl_entity_t *self)
{
    return lh_gl_entity_class_get_kind(lh_gl_entity_get_class(self));
}

lh_bool_t
lh_gl_entity_is_layer(const lh_gl_entity_t *self)
{
    return lh_gl_entity_class_is_layer(lh_gl_entity_get_class(self));
}

lh_bool_t
lh_gl_entity_is_root(const lh_gl_entity_t *self)
{
    return lh_ptr_is_null(lh_gl_entity_get_parent(self));
}

lh_void
lh_gl_entity_draw(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas)
{
    lh_gl_entity_class_draw_cb draw_cb =
        lh_gl_entity_class_get_draw_cb(lh_gl_entity_get_class(self));

    if (lh_ptr_is_null(draw_cb))
    {
        return;
    }
    draw_cb(self, canvas);
}
