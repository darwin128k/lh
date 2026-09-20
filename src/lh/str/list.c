#include <lh/str/list.h>
#include <lh/assert.h>
#include <lh/index.h>
#include <lh/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_vector_t *
lh_str_list_get_items(lh_str_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->items);
}

const lh_vector_t *
lh_str_list_get_items_as_const(const lh_str_list_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->items);
}

void
lh_str_list_init(lh_str_list_t *self)
{
    lh_vector_init(lh_str_list_get_items(self), sizeof(lh_str_t));
}

void
lh_str_list_clear(lh_str_list_t *self)
{
    lh_vector_t *items;
    lh_usize_t n;

    items = lh_str_list_get_items(self);
    n = lh_vector_get_size(items);
    while (n > 0U)
    {
        --n;
        lh_str_deinit(lh_ptr_cast(lh_str_t, lh_vector_get_ptr(items, n)));
    }
    lh_vector_clear(items);
}

void
lh_str_list_deinit(lh_str_list_t *self)
{
    lh_str_list_clear(self);
    lh_vector_deinit(lh_str_list_get_items(self));
}

lh_uindex_t
lh_str_list_push_back(lh_str_list_t *self, lh_str_view_t text)
{
    lh_str_t value;

    lh_str_init(lh_addr_of(value));
    lh_str_assign_view(lh_addr_of(value), text);
    /* Bytes of value (including its heap pointer) are copied into the new
       slot; ownership moves there, so value itself is not deinited. */
    return lh_vector_push_back(lh_str_list_get_items(self), lh_addr_of(value));
}

lh_uindex_t
lh_str_list_push_back_str(lh_str_list_t *self, const lh_str_t *value)
{
    return lh_str_list_push_back(self, lh_str_as_view(value));
}

lh_bool_t
lh_str_list_is_empty(const lh_str_list_t *self)
{
    return lh_vector_is_empty(lh_str_list_get_items_as_const(self));
}

lh_usize_t
lh_str_list_get_size(const lh_str_list_t *self)
{
    return lh_vector_get_size(lh_str_list_get_items_as_const(self));
}

lh_str_t *
lh_str_list_get(lh_str_list_t *self, lh_uindex_t index)
{
    return lh_ptr_cast(lh_str_t, lh_vector_get_ptr(lh_str_list_get_items(self), index));
}

const lh_str_t *
lh_str_list_get_as_const(const lh_str_list_t *self, lh_uindex_t index)
{
    return lh_ptr_cast(const lh_str_t,
                       lh_vector_get_ptr(lh_str_list_get_items_as_const(self), index));
}
