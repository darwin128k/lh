#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/runtime/error.h>
#include <lh/str.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <fcntl.h>
#    include <unistd.h>
#endif

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static DWORD
lh_os_fs_file_win_access(lh_os_fs_file_mode_t mode)
{
    if (mode == lh_os_fs_file_mode_read)
    {
        return GENERIC_READ;
    }
    if (mode == lh_os_fs_file_mode_write)
    {
        return GENERIC_WRITE;
    }
    return GENERIC_READ | GENERIC_WRITE;
}

static DWORD
lh_os_fs_file_win_disposition(lh_os_fs_file_mode_t mode)
{
    if (mode == lh_os_fs_file_mode_read)
    {
        return OPEN_EXISTING;
    }
    if (mode == lh_os_fs_file_mode_write)
    {
        return CREATE_ALWAYS;
    }
    return OPEN_ALWAYS;
}

static lh_bool_t
lh_os_fs_file_open_native(lh_os_fs_file_t *self, lh_str_cptr cstr, lh_os_fs_file_mode_t mode)
{
    HANDLE native;

    native = CreateFileA(cstr, lh_os_fs_file_win_access(mode), FILE_SHARE_READ, lh_null,
                         lh_os_fs_file_win_disposition(mode), FILE_ATTRIBUTE_NORMAL, lh_null);
    if (native == INVALID_HANDLE_VALUE)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) =
        lh_cast_reinterpret(lh_os_fs_file_handle_t, native);
    return lh_bool_true;
}
#else
static int
lh_os_fs_file_posix_flags(lh_os_fs_file_mode_t mode)
{
    if (mode == lh_os_fs_file_mode_read)
    {
        return O_RDONLY;
    }
    if (mode == lh_os_fs_file_mode_write)
    {
        return O_WRONLY | O_CREAT | O_TRUNC;
    }
    return O_RDWR | O_CREAT;
}

static lh_bool_t
lh_os_fs_file_open_native(lh_os_fs_file_t *self, lh_str_cptr cstr, lh_os_fs_file_mode_t mode)
{
    int native;

    native = mode == lh_os_fs_file_mode_read
                 ? open(cstr, lh_os_fs_file_posix_flags(mode))
                 : open(cstr, lh_os_fs_file_posix_flags(mode), 0644);
    if (native < 0)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) =
        lh_cast_static(lh_os_fs_file_handle_t, native);
    return lh_bool_true;
}
#endif

lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, const lh_os_fs_path_t *path, lh_os_fs_file_mode_t mode)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(path);
    lh_assert_runtime_if(mode != lh_os_fs_file_mode_read && mode != lh_os_fs_file_mode_write &&
                             mode != lh_os_fs_file_mode_readwrite,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (lh_os_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    lh_os_fs_file_close(self);
    if (!lh_os_fs_file_open_native(self, lh_str_get_data(lh_os_fs_path_get_text_as_const(path)),
                                   mode))
    {
        return lh_bool_false;
    }
    lh_os_fs_path_assign(lh_os_fs_file_get_path(self), path);
    return lh_bool_true;
}
