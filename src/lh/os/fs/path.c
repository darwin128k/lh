#include <lh/os/fs/path.h>
#include <lh/assert.h>
#include <lh/util/addr.h>

void
lh_os_fs_path_init(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    self->root_kind = lh_os_fs_path_root_kind_none;
    self->root_drive = 0;
    lh_str_list_init(lh_addr_of(self->segments));
}

void
lh_os_fs_path_deinit(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    lh_str_list_deinit(lh_addr_of(self->segments));
}

lh_os_fs_path_root_kind_t
lh_os_fs_path_get_root_kind(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root_kind;
}

lh_char_t
lh_os_fs_path_get_root_drive(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root_drive;
}

lh_str_list_t *
lh_os_fs_path_get_segments(lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->segments);
}

const lh_str_list_t *
lh_os_fs_path_get_segments_as_const(const lh_os_fs_path_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->segments);
}

lh_usize_t
lh_os_fs_path_get_segment_count(const lh_os_fs_path_t *self)
{
    return lh_str_list_get_size(lh_os_fs_path_get_segments_as_const(self));
}

lh_bool_t
lh_os_fs_path_is_empty(const lh_os_fs_path_t *self)
{
    return (lh_os_fs_path_get_root_kind(self) == lh_os_fs_path_root_kind_none &&
            lh_str_list_is_empty(lh_os_fs_path_get_segments_as_const(self)))
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_os_fs_path_is_absolute(const lh_os_fs_path_t *self)
{
    return (lh_os_fs_path_get_root_kind(self) != lh_os_fs_path_root_kind_none) ? lh_bool_true
                                                                                : lh_bool_false;
}
