#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_os_fs_path_t *
lh_os_fs_file_get_path(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

const lh_os_fs_path_t *
lh_os_fs_file_get_path_as_const(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

lh_os_fs_file_handle_t *
lh_os_fs_file_get_handle(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

const lh_os_fs_file_handle_t *
lh_os_fs_file_get_handle_as_const(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

lh_bool_t
lh_os_fs_file_is_valid(const lh_os_fs_file_t *self)
{
    return (lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self)) !=
            LH_OS_FS_FILE_HANDLE_INVALID)
               ? lh_bool_true
               : lh_bool_false;
}

void
lh_os_fs_file_close(lh_os_fs_file_t *self)
{
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = LH_OS_FS_FILE_HANDLE_INVALID;
}

void
lh_os_fs_file_set_path(lh_os_fs_file_t *self, const lh_os_fs_path_t *path)
{
    lh_os_fs_path_assign(lh_os_fs_file_get_path(self), path);
}

void
lh_os_fs_file_init(lh_os_fs_file_t *self)
{
    lh_os_fs_path_init(lh_os_fs_file_get_path(self));
    lh_os_fs_file_close(self);
}

void
lh_os_fs_file_deinit(lh_os_fs_file_t *self)
{
    lh_os_fs_file_close(self);
    lh_os_fs_path_deinit(lh_os_fs_file_get_path(self));
}
