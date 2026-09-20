#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/util/addr.h>

void
lh_os_fs_file_init(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_fs_path_init(lh_addr_of(self->path));
    self->handle = LH_OS_FS_FILE_HANDLE_INVALID;
    self->mode = lh_os_fs_file_mode_none;
}

void
lh_os_fs_file_close(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_FS_FILE_HANDLE_INVALID;
    self->mode = lh_os_fs_file_mode_none;
}

void
lh_os_fs_file_deinit(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_fs_file_close(self);
    lh_os_fs_path_deinit(lh_addr_of(self->path));
}
