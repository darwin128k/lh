/**
 * @file entity.c
 * @brief Implementation of `lh/gx/entity.h`.
 */

#include <lh/gx/entity.h>

#include <lh/core/util/return.h>

const lh_gx_node_t *
lh_gx_entity_get_node(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_gx_node_t *
lh_gx_entity_get_node_mut(lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

const lh_gx_rect_t *
lh_gx_entity_get_place(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->place);
}

lh_void
lh_gx_entity_set_place(lh_gx_entity_t *self, lh_gx_rect_t place)
{
    lh_assert_runtime_ref(self);
    self->place = place;
}

lh_bool_t
lh_gx_entity_is_visible(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->visible;
}

lh_void
lh_gx_entity_set_visible(lh_gx_entity_t *self, lh_bool_t visible)
{
    lh_assert_runtime_ref(self);
    self->visible = visible;
}

const struct lh_gx_class *
lh_gx_entity_get_class(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->klass;
}

lh_void
lh_gx_entity_set_class(lh_gx_entity_t *self, const struct lh_gx_class *klass)
{
    lh_assert_runtime_ref(self);
    self->klass = klass;
}

const struct lh_gx_style *
lh_gx_entity_get_style(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_gx_entity_set_style(lh_gx_entity_t *self, const struct lh_gx_style *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
}

lh_ptr
lh_gx_entity_get_content(const lh_gx_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->content;
}

lh_void
lh_gx_entity_set_content(lh_gx_entity_t *self, lh_ptr content)
{
    lh_assert_runtime_ref(self);
    self->content = content;
}

lh_gx_entity_t *
lh_gx_entity_from_node(const lh_gx_node_t *node)
{
    lh_return_if(lh_null_eq(node), lh_null);
    return lh_list_entry(lh_gx_entity_t, node.link, lh_gx_node_get_link(node));
}

lh_void
lh_gx_entity_init(lh_gx_entity_t *self, const struct lh_gx_class *klass,
                  const struct lh_gx_style *style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                  lh_gx_scalar_t width, lh_gx_scalar_t height)
{
    lh_gx_node_init(lh_gx_entity_get_node_mut(self));
    lh_gx_rect_t place;

    lh_gx_rect_init(lh_addr_of(place), x, y, width, height);
    lh_gx_entity_set_place(self, place);
    lh_gx_entity_set_visible(self, lh_bool_true);
    lh_gx_entity_set_class(self, klass);
    lh_gx_entity_set_style(self, style);
    lh_gx_entity_set_content(self, lh_null);
}

lh_gx_entity_t *
lh_gx_entity_get_owner(const lh_gx_entity_t *self)
{
    return lh_gx_entity_from_node(lh_gx_node_get_owner(lh_gx_entity_get_node(self)));
}

lh_gx_entity_t *
lh_gx_entity_get_first_child(const lh_gx_entity_t *self)
{
    return lh_gx_entity_from_node(lh_gx_node_get_first_child(lh_gx_entity_get_node(self)));
}

lh_gx_entity_t *
lh_gx_entity_get_next_sibling(const lh_gx_entity_t *self)
{
    return lh_gx_entity_from_node(lh_gx_node_get_next_sibling(lh_gx_entity_get_node(self)));
}

lh_void
lh_gx_entity_attach(lh_gx_entity_t *owner, lh_gx_entity_t *child)
{
    lh_gx_node_attach(lh_gx_entity_get_node_mut(owner), lh_gx_entity_get_node_mut(child));
}

lh_void
lh_gx_entity_attach_v(lh_gx_entity_t *owner, lh_gx_entity_t *child)
{
    lh_gx_node_attach_v(lh_gx_entity_get_node_mut(owner), lh_gx_entity_get_node_mut(child));
}

lh_void
lh_gx_entity_detach(lh_gx_entity_t *self)
{
    lh_gx_node_detach(lh_gx_entity_get_node_mut(self));
}
