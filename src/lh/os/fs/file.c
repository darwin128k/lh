#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <unistd.h>
#endif

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
    lh_assert_runtime_ref(self);
    if (!lh_os_fs_file_is_valid(self))
    {
        return;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    (void)CloseHandle(lh_cast_reinterpret(
        HANDLE, lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self))));
#else
    (void)close(lh_cast_static(int, lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self))));
#endif
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
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = LH_OS_FS_FILE_HANDLE_INVALID;
}

void
lh_os_fs_file_deinit(lh_os_fs_file_t *self)
{
    lh_os_fs_file_close(self);
    lh_os_fs_path_deinit(lh_os_fs_file_get_path(self));
}
