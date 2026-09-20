#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/runtime/error.h>
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

lh_os_fs_file_mode_t
lh_os_fs_file_get_mode(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

lh_bool_t
lh_os_fs_file_is(const lh_os_fs_file_t *self, lh_os_fs_access_t access)
{
    lh_os_fs_file_mode_t mode;

    lh_assert_runtime_if(access != lh_os_fs_access_readable &&
                             access != lh_os_fs_access_writable &&
                             access != lh_os_fs_access_executable,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (!lh_os_fs_file_is_valid(self))
    {
        return lh_bool_false;
    }
    mode = lh_os_fs_file_get_mode(self);
    if (access == lh_os_fs_access_readable)
    {
        return (mode == lh_os_fs_file_mode_read || mode == lh_os_fs_file_mode_readwrite)
                   ? lh_bool_true
                   : lh_bool_false;
    }
    if (access == lh_os_fs_access_writable)
    {
        return (mode == lh_os_fs_file_mode_write || mode == lh_os_fs_file_mode_readwrite)
                   ? lh_bool_true
                   : lh_bool_false;
    }
    return lh_bool_false;
}

void
lh_os_fs_file_close(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    if (!lh_os_fs_file_is_valid(self))
    {
        self->mode = lh_os_fs_file_mode_none;
        return;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    (void)CloseHandle(lh_cast_reinterpret(
        HANDLE, lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self))));
#else
    (void)close(lh_cast_static(int, lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self))));
#endif
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = LH_OS_FS_FILE_HANDLE_INVALID;
    self->mode = lh_os_fs_file_mode_none;
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
    self->mode = lh_os_fs_file_mode_none;
}

void
lh_os_fs_file_deinit(lh_os_fs_file_t *self)
{
    lh_os_fs_file_close(self);
    lh_os_fs_path_deinit(lh_os_fs_file_get_path(self));
}
