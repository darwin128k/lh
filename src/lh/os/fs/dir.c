#include <lh/os/fs/dir.h>
#include "dir/state.h"
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/runtime/allocator.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#else
#    include <dirent.h>
#endif

lh_os_fs_path_t *
lh_os_fs_dir_get_path(lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

const lh_os_fs_path_t *
lh_os_fs_dir_get_path_as_const(const lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

lh_os_fs_file_handle_t *
lh_os_fs_dir_get_handle(lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

const lh_os_fs_file_handle_t *
lh_os_fs_dir_get_handle_as_const(const lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

lh_bool_t
lh_os_fs_dir_is_valid(const lh_os_fs_dir_t *self)
{
    return (lh_ptr_deref(lh_os_fs_dir_get_handle_as_const(self)) !=
            LH_OS_FS_FILE_HANDLE_INVALID)
               ? lh_bool_true
               : lh_bool_false;
}

void
lh_os_fs_dir_close(lh_os_fs_dir_t *self)
{
    struct lh_os_fs_dir_state *state;

    lh_assert_runtime_ref(self);
    if (!lh_os_fs_dir_is_valid(self))
    {
        self->mode = lh_os_fs_file_mode_none;
        return;
    }
    state = lh_os_fs_dir_get_state(self);
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    FindClose(state->find);
#else
    (void)closedir(state->dir);
#endif
    lh_runtime_allocator_free(state);
    lh_ptr_deref(lh_os_fs_dir_get_handle(self)) = LH_OS_FS_FILE_HANDLE_INVALID;
    self->mode = lh_os_fs_file_mode_none;
}

void
lh_os_fs_dir_set_path(lh_os_fs_dir_t *self, const lh_os_fs_path_t *path)
{
    lh_os_fs_path_assign(lh_os_fs_dir_get_path(self), path);
}

void
lh_os_fs_dir_init(lh_os_fs_dir_t *self)
{
    lh_os_fs_path_init(lh_os_fs_dir_get_path(self));
    lh_ptr_deref(lh_os_fs_dir_get_handle(self)) = LH_OS_FS_FILE_HANDLE_INVALID;
    self->mode = lh_os_fs_file_mode_none;
}

void
lh_os_fs_dir_deinit(lh_os_fs_dir_t *self)
{
    lh_os_fs_dir_close(self);
    lh_os_fs_path_deinit(lh_os_fs_dir_get_path(self));
}
