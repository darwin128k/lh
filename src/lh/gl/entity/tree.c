/**
 * @file tree.c
 * @brief Implementation of `lh/gl/entity.h`: the tree: parent and children, and the walks over
 * them.
 */

#include <lh/gl/entity.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>
#include <lh/gl/view.h>

lh_bool_t
lh_gl_entity_is_ancestor(const lh_gl_entity_t *ancestor, const lh_gl_entity_t *self)
{
    const lh_gl_entity_t *walk = self;

    while (lh_null_ne(walk) && walk != ancestor)
    {
        walk = lh_gl_entity_get_parent(walk);
    }
    return lh_null_ne(walk) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_gl_entity_add_child(lh_gl_entity_t *self, lh_gl_entity_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    lh_assert_runtime_if(self == child, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_list_node_is_linked(lh_gl_entity_get_link_mut(child)),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_null_ne(lh_gl_entity_get_parent(child)),
                         lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_gl_entity_is_ancestor(child, self),
                         lh_runtime_error_code_invalid_argument);
    lh_gl_entity_set_parent(child, self);
    lh_list_push_back(lh_gl_entity_get_children_mut(self), lh_gl_entity_get_link_mut(child));
}

lh_void
lh_gl_entity_remove_child(lh_gl_entity_t *self, lh_gl_entity_t *child)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(child);
    lh_assert_runtime_ifn(lh_gl_entity_get_parent(child) == self,
                          lh_runtime_error_code_invalid_argument);
    /* Out of the tree, out of the view's slots: what leaves lands on its styles, so
       an entity the application frees after taking it out is not left in flight. */
    if (lh_null_ne(lh_gl_entity_own_view(self)))
    {
        lh_gl_view_finish_transitions(lh_gl_entity_own_view(self), child);
    }
    lh_list_node_unlink(lh_gl_entity_get_link_mut(child));
    lh_gl_entity_set_parent(child, lh_null);
}

lh_gl_entity_t *
lh_gl_entity_get_first_child(const lh_gl_entity_t *self)
{
    return lh_list_entry(lh_gl_entity_t, link, lh_list_get_first(lh_gl_entity_get_children(self)));
}

lh_gl_entity_t *
lh_gl_entity_get_next_child(const lh_gl_entity_t *self, const lh_gl_entity_t *child)
{
    lh_assert_runtime_ref(child);
    return lh_list_entry(
        lh_gl_entity_t, link,
        lh_list_get_next(lh_gl_entity_get_children(self), lh_gl_entity_get_link(child)));
}

lh_gl_entity_t *
lh_gl_entity_get_last_child(const lh_gl_entity_t *self)
{
    return lh_list_entry(lh_gl_entity_t, link, lh_list_get_last(lh_gl_entity_get_children(self)));
}

lh_gl_entity_t *
lh_gl_entity_get_prev_child(const lh_gl_entity_t *self, const lh_gl_entity_t *child)
{
    lh_assert_runtime_ref(child);
    return lh_list_entry(
        lh_gl_entity_t, link,
        lh_list_get_prev(lh_gl_entity_get_children(self), lh_gl_entity_get_link(child)));
}

lh_bool_t
lh_gl_entity_walk(const lh_gl_entity_t *self, lh_gl_entity_visit_cb visit, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(visit);
    lh_return_if(!visit(self, context), lh_bool_false);
    return lh_gl_entity_walk_children(self, visit, context);
}

lh_bool_t
lh_gl_entity_walk_children(const lh_gl_entity_t *self, lh_gl_entity_visit_cb visit, lh_ptr context)
{
    lh_gl_entity_t *child;
    lh_bool_t all = lh_bool_true;

    for (child = lh_gl_entity_get_first_child(self); all && lh_null_ne(child);
         child = lh_gl_entity_get_next_child(self, child))
    {
        all = lh_gl_entity_walk(child, visit, context);
    }
    return all;
}

lh_void
lh_gl_entity_ask_children(const lh_gl_entity_t *self, lh_gl_entity_transform_t *transform)
{
    lh_gl_entity_transform_init(transform);
    lh_gl_entity_send(self, lh_gl_entity_event_children, transform);
}
