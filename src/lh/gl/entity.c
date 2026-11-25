/**
 * @file entity.c
 * @brief Implementation of `lh/gl/entity.h`: the entity itself: its fields (level 1), init and its
 * class.
 */

#include <lh/gl/entity.h>

#include <lh/core/assert.h>

const lh_gl_place_t *
lh_gl_entity_get_place(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->place);
}

const lh_gl_style_t *
lh_gl_entity_get_style(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_bool_t
lh_gl_entity_is_pressed(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

/** The view the tree of @p self belongs to, read off its root; may be ::lh_null. */
lh_void
lh_gl_entity_set_pressed(lh_gl_entity_t *self, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    self->pressed = pressed;
}

const lh_gl_entity_class_t *
lh_gl_entity_get_class(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->class;
}

lh_bool_t
lh_gl_entity_is_hidden(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hidden;
}

lh_gl_entity_t *
lh_gl_entity_get_parent(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->parent;
}

lh_gl_rect_t *
lh_gl_entity_get_rect_mut(lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_void
lh_gl_entity_set_parent(lh_gl_entity_t *self, struct lh_gl_entity *parent)
{
    lh_assert_runtime_ref(self);
    self->parent = parent;
}

struct lh_gl_view *
lh_gl_entity_get_view(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->view;
}

lh_void
lh_gl_entity_set_view(lh_gl_entity_t *self, struct lh_gl_view *view)
{
    lh_assert_runtime_ref(self);
    self->view = view;
}

lh_gl_place_t *
lh_gl_entity_get_place_mut(lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->place);
}

const lh_list_t *
lh_gl_entity_get_children(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

lh_list_t *
lh_gl_entity_get_children_mut(lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

lh_void
lh_gl_entity_set_children(lh_gl_entity_t *self, lh_list_t children)
{
    lh_assert_runtime_ref(self);
    self->children = children;
}

const lh_list_node_t *
lh_gl_entity_get_link(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_list_node_t *
lh_gl_entity_get_link_mut(lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_void
lh_gl_entity_set_link(lh_gl_entity_t *self, lh_list_node_t link)
{
    lh_assert_runtime_ref(self);
    self->link = link;
}

const lh_gl_rect_t *
lh_gl_entity_get_rect(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->rect);
}

lh_void
lh_gl_entity_set_place(lh_gl_entity_t *self, lh_gl_place_t place)
{
    lh_assert_runtime_ref(self);
    self->place = place;
}

lh_void
lh_gl_entity_set_rect(lh_gl_entity_t *self, lh_gl_rect_t rect)
{
    lh_assert_runtime_ref(self);
    self->rect = rect;
}

lh_void
lh_gl_entity_set_class(lh_gl_entity_t *self, const lh_gl_entity_class_t *entity_class)
{
    lh_assert_runtime_ref(self);
    self->class = entity_class;
}

lh_void
lh_gl_entity_set_hidden(lh_gl_entity_t *self, lh_bool_t hidden)
{
    lh_assert_runtime_ref(self);
    self->hidden = hidden;
}

lh_void
lh_gl_entity_set_style(lh_gl_entity_t *self, const lh_gl_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->style = style;
}

lh_bool_t
lh_gl_entity_is_scroll_chain_enabled(const lh_gl_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scroll_chain;
}

lh_void
lh_gl_entity_set_scroll_chain(lh_gl_entity_t *self, lh_bool_t enabled)
{
    lh_assert_runtime_ref(self);
    self->scroll_chain = enabled;
}

lh_void
lh_gl_entity_init(lh_gl_entity_t *self, lh_gl_rect_t rect)
{
    lh_gl_entity_set_rect(self, rect);
    lh_gl_entity_set_style(self, lh_null);
    lh_gl_entity_set_class(self, lh_addr_of(lh_gl_entity_class));
    lh_gl_entity_set_hidden(self, lh_bool_false);
    lh_gl_entity_set_pressed(self, lh_bool_false);
    lh_gl_entity_set_scroll_chain(self, lh_bool_true);
    lh_gl_entity_set_parent(self, lh_null);
    lh_gl_entity_set_view(self, lh_null);
    lh_gl_place_init(lh_gl_entity_get_place_mut(self), lh_gl_place_size_fixed, lh_gl_scalar(0));
    lh_list_init(lh_gl_entity_get_children_mut(self));
    lh_list_node_init(lh_gl_entity_get_link_mut(self));
}

lh_void
lh_gl_entity_copy_rect(const lh_gl_entity_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_entity_get_rect(self));
}

lh_void
lh_gl_entity_set_class_v(lh_gl_entity_t *self, const lh_gl_entity_class_t *class)
{
    lh_assert_runtime_ref(class);
    lh_assert_runtime_ref(lh_gl_entity_class_get_event_fn(class));
    lh_gl_entity_set_class(self, class);
}

struct lh_gl_view *
lh_gl_entity_own_view(const lh_gl_entity_t *self)
{
    const lh_gl_entity_t *top = self;

    while (lh_null_ne(lh_gl_entity_get_parent(top)))
    {
        top = lh_gl_entity_get_parent(top);
    }
    return lh_gl_entity_get_view(top);
}
