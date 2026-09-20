#include <lh/os/fs/path/span.h>
#include <lh/assert.h>
#include <lh/memory/view.h>
#include <lh/util/addr.h>

void
lh_os_fs_path_span_init(lh_os_fs_path_span_t *self, lh_usize_t offset, lh_usize_t size)
{
    lh_assert_runtime_ref(self);
    self->offset = offset;
    self->size = size;
}

lh_os_fs_path_span_t
lh_os_fs_path_span_make(lh_usize_t offset, lh_usize_t size)
{
    lh_os_fs_path_span_t self;

    lh_os_fs_path_span_init(lh_addr_of(self), offset, size);
    return self;
}

lh_usize_t
lh_os_fs_path_span_get_offset(const lh_os_fs_path_span_t *self)
{
    lh_assert_runtime_ref(self);
    return self->offset;
}

lh_usize_t
lh_os_fs_path_span_get_size(const lh_os_fs_path_span_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_bool_t
lh_os_fs_path_span_is_empty(const lh_os_fs_path_span_t *self)
{
    return lh_os_fs_path_span_get_size(self) == 0U;
}

lh_str_view_t
lh_os_fs_path_span_as_view(const lh_os_fs_path_span_t *self, lh_str_view_t text)
{
    return lh_memory_view_make_from_offset(lh_addr_of(text), lh_os_fs_path_span_get_offset(self),
                                           lh_os_fs_path_span_get_size(self));
}
