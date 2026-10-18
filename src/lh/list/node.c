#include <lh/list/node.h>
#include <lh/assert.h>
#include <lh/list/node/link.h>

lh_void
lh_list_node_set_next(lh_list_node_t *self, lh_list_node_t *next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}

lh_void
lh_list_node_set_prev(lh_list_node_t *self, lh_list_node_t *prev)
{
    lh_assert_runtime_ref(self);
    self->prev = prev;
}

lh_list_node_t *
lh_list_node_get_next(const lh_list_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_list_node_t *
lh_list_node_get_prev(const lh_list_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->prev;
}

lh_ptr
lh_list_node_get_entry(const lh_list_node_t *self, lh_usize_t offset)
{
    if (lh_ptr_is_null(self))
    {
        return lh_null;
    }
    return lh_ptr_sub_by_offset_unsafe(lh_void, self, offset);
}

lh_void
lh_list_node_init(lh_list_node_t *self)
{
    lh_list_node_set_next(self, self);
    lh_list_node_set_prev(self, self);
}

lh_bool_t
lh_list_node_is_linked(const lh_list_node_t *self)
{
    return lh_cast_static(lh_bool_t, lh_ptr_ne(lh_list_node_get_next(self), self));
}

lh_void
lh_list_node_link(lh_list_node_t *self, lh_list_node_t *prev, lh_list_node_t *next)
{
    /* Linking a linked node would silently tear its old list apart. */
    lh_assert_runtime_if(lh_list_node_is_linked(self), lh_runtime_error_code_invalid_argument);
    lh_list_node_set_prev(self, prev);
    lh_list_node_set_next(self, next);
    lh_list_node_set_next(prev, self);
    lh_list_node_set_prev(next, self);
}

lh_void
lh_list_node_unlink(lh_list_node_t *self)
{
    lh_list_node_t *const prev = lh_list_node_get_prev(self);
    lh_list_node_t *const next = lh_list_node_get_next(self);

    lh_list_node_set_next(prev, next);
    lh_list_node_set_prev(next, prev);
    lh_list_node_init(self);
}

lh_void
lh_list_node_insert_after(lh_list_node_t *self, lh_list_node_t *pos)
{
    lh_list_node_link(self, pos, lh_list_node_get_next(pos));
}

lh_void
lh_list_node_insert_before(lh_list_node_t *self, lh_list_node_t *pos)
{
    lh_list_node_link(self, lh_list_node_get_prev(pos), pos);
}

lh_void
lh_list_node_move_after(lh_list_node_t *self, lh_list_node_t *pos)
{
    if (lh_ptr_eq(self, pos))
    {
        return;
    }
    lh_list_node_unlink(self);
    lh_list_node_insert_after(self, pos);
}

lh_void
lh_list_node_move_before(lh_list_node_t *self, lh_list_node_t *pos)
{
    if (lh_ptr_eq(self, pos))
    {
        return;
    }
    lh_list_node_unlink(self);
    lh_list_node_insert_before(self, pos);
}
