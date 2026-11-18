/**
 * @file node.c
 * @brief Implementation of `lh/gx/node.h`.
 */

#include <lh/gx/node.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/util/return.h>

const lh_gx_node_t *
lh_gx_node_get_owner(const lh_gx_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

lh_gx_node_t *
lh_gx_node_get_owner_mut(lh_gx_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

lh_void
lh_gx_node_set_owner(lh_gx_node_t *self, lh_gx_node_t *owner)
{
    lh_assert_runtime_ref(self);
    self->owner = owner;
}

const lh_list_t *
lh_gx_node_get_children(const lh_gx_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

lh_list_t *
lh_gx_node_get_children_mut(lh_gx_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

const lh_list_node_t *
lh_gx_node_get_link(const lh_gx_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_list_node_t *
lh_gx_node_get_link_mut(lh_gx_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_gx_node_t *
lh_gx_node_from_link(const lh_list_node_t *entry)
{
    return lh_list_entry(lh_gx_node_t, link, entry);
}

lh_void
lh_gx_node_init(lh_gx_node_t *self)
{
    lh_gx_node_set_owner(self, lh_null);
    lh_list_init(lh_gx_node_get_children_mut(self));
    lh_list_node_init(lh_gx_node_get_link_mut(self));
}

lh_bool_t
lh_gx_node_is_attached(const lh_gx_node_t *self)
{
    return lh_null_ne(lh_gx_node_get_owner(self));
}

lh_bool_t
lh_gx_node_has_children(const lh_gx_node_t *self)
{
    return lh_cast_static(lh_bool_t, !lh_list_is_empty(lh_gx_node_get_children(self)));
}

lh_gx_node_t *
lh_gx_node_get_first_child(const lh_gx_node_t *self)
{
    return lh_gx_node_from_link(lh_list_get_first(lh_gx_node_get_children(self)));
}

lh_gx_node_t *
lh_gx_node_get_last_child(const lh_gx_node_t *self)
{
    return lh_gx_node_from_link(lh_list_get_last(lh_gx_node_get_children(self)));
}

lh_gx_node_t *
lh_gx_node_get_next_sibling(const lh_gx_node_t *self)
{
    const lh_gx_node_t *owner = lh_gx_node_get_owner(self);

    lh_return_if(lh_null_eq(owner), lh_null);
    return lh_gx_node_from_link(
        lh_list_get_next(lh_gx_node_get_children(owner), lh_gx_node_get_link(self)));
}

lh_gx_node_t *
lh_gx_node_get_prev_sibling(const lh_gx_node_t *self)
{
    const lh_gx_node_t *owner = lh_gx_node_get_owner(self);

    lh_return_if(lh_null_eq(owner), lh_null);
    return lh_gx_node_from_link(
        lh_list_get_prev(lh_gx_node_get_children(owner), lh_gx_node_get_link(self)));
}

lh_bool_t
lh_gx_node_is_ancestor_of(const lh_gx_node_t *self, const lh_gx_node_t *other)
{
    lh_assert_runtime_ref(self);
    for (const lh_gx_node_t *up = lh_gx_node_get_owner(other); lh_null_ne(up);
         up = lh_gx_node_get_owner(up))
    {
        lh_return_if(up == self, lh_bool_true);
    }
    return lh_bool_false;
}

lh_void
lh_gx_node_attach(lh_gx_node_t *owner, lh_gx_node_t *child)
{
    lh_gx_node_set_owner(child, owner);
    lh_list_push_back(lh_gx_node_get_children_mut(owner), lh_gx_node_get_link_mut(child));
}

const lh_gx_node_t *
lh_gx_node_get_root(const lh_gx_node_t *self)
{
    const lh_gx_node_t *root = self;

    while (lh_gx_node_is_attached(root))
    {
        root = lh_gx_node_get_owner(root);
    }
    return root;
}

lh_void
lh_gx_node_attach_v(lh_gx_node_t *owner, lh_gx_node_t *child)
{
    lh_assert_runtime_if(lh_gx_node_is_attached(child), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(owner == child, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_gx_node_is_ancestor_of(child, owner),
                         lh_runtime_error_code_invalid_argument);
    lh_gx_node_attach(owner, child);
}

lh_void
lh_gx_node_detach(lh_gx_node_t *self)
{
    lh_return_if(!lh_gx_node_is_attached(self));
    lh_list_node_unlink(lh_gx_node_get_link_mut(self));
    lh_gx_node_set_owner(self, lh_null);
}

lh_gx_node_t *
lh_gx_node_get_next_skip(const lh_gx_node_t *self, const lh_gx_node_t *top)
{
    for (const lh_gx_node_t *at = self; at != top && lh_null_ne(at); at = lh_gx_node_get_owner(at))
    {
        lh_gx_node_t *next = lh_gx_node_get_next_sibling(at);

        lh_return_if(lh_null_ne(next), next);
    }
    return lh_null;
}

lh_gx_node_t *
lh_gx_node_get_next(const lh_gx_node_t *self, const lh_gx_node_t *top)
{
    lh_gx_node_t *child = lh_gx_node_get_first_child(self);

    lh_return_if(lh_null_ne(child), child);
    return lh_gx_node_get_next_skip(self, top);
}
