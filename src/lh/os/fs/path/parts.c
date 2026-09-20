#include <lh/os/fs/path/parts.h>
#include <lh/assert.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

static lh_vector_t *
lh_os_fs_path_parts_get_items(lh_os_fs_path_parts_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->items);
}

static const lh_vector_t *
lh_os_fs_path_parts_get_items_as_const(const lh_os_fs_path_parts_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->items);
}

void
lh_os_fs_path_parts_init(lh_os_fs_path_parts_t *self)
{
    lh_vector_init(lh_os_fs_path_parts_get_items(self), sizeof(lh_os_fs_path_span_t));
}

void
lh_os_fs_path_parts_clear(lh_os_fs_path_parts_t *self)
{
    lh_vector_clear(lh_os_fs_path_parts_get_items(self));
}

void
lh_os_fs_path_parts_deinit(lh_os_fs_path_parts_t *self)
{
    lh_vector_deinit(lh_os_fs_path_parts_get_items(self));
}

void
lh_os_fs_path_parts_assign(lh_os_fs_path_parts_t *self, const lh_os_fs_path_parts_t *other)
{
    lh_assert_runtime_ref(other);
    if (self == other)
    {
        return;
    }
    lh_vector_assign(lh_os_fs_path_parts_get_items(self),
                     lh_os_fs_path_parts_get_items_as_const(other));
}

lh_bool_t
lh_os_fs_path_parts_is_empty(const lh_os_fs_path_parts_t *self)
{
    return lh_vector_is_empty(lh_os_fs_path_parts_get_items_as_const(self));
}

lh_usize_t
lh_os_fs_path_parts_get_size(const lh_os_fs_path_parts_t *self)
{
    return lh_vector_get_size(lh_os_fs_path_parts_get_items_as_const(self));
}

lh_os_fs_path_span_t *
lh_os_fs_path_parts_get(lh_os_fs_path_parts_t *self, lh_uindex_t index)
{
    return lh_ptr_cast(lh_os_fs_path_span_t,
                       lh_vector_get_ptr(lh_os_fs_path_parts_get_items(self), index));
}

const lh_os_fs_path_span_t *
lh_os_fs_path_parts_get_as_const(const lh_os_fs_path_parts_t *self, lh_uindex_t index)
{
    return lh_ptr_cast(const lh_os_fs_path_span_t,
                       lh_vector_get_ptr(lh_os_fs_path_parts_get_items_as_const(self), index));
}

void
lh_os_fs_path_parts_push(lh_os_fs_path_parts_t *self, const lh_os_fs_path_span_t *span)
{
    lh_vector_push_back(lh_os_fs_path_parts_get_items(self), span);
}

void
lh_os_fs_path_parts_pop(lh_os_fs_path_parts_t *self, lh_os_fs_path_span_t *dst)
{
    lh_vector_pop_back(lh_os_fs_path_parts_get_items(self), dst);
}
