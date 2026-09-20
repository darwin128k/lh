#include <lh/os/fs/access.h>
#include <lh/assert.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/runtime/error.h>
#include <lh/str.h>
#include <lh/util/addr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <unistd.h>
#endif

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static DWORD
lh_os_fs_can_win_desired(lh_os_fs_access_t which)
{
    if (which == lh_os_fs_access_writable)
    {
        return GENERIC_WRITE;
    }
    if (which == lh_os_fs_access_executable)
    {
        return GENERIC_EXECUTE;
    }
    return GENERIC_READ;
}

static lh_bool_t
lh_os_fs_can_native(const lh_os_fs_path_t *path, lh_os_fs_access_t which)
{
    lh_str_cptr cstr;
    DWORD attrs;
    DWORD flags;
    HANDLE native;

    cstr = lh_str_get_data(lh_os_fs_path_get_text_as_const(path));
    attrs = GetFileAttributesA(cstr);
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    flags = ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0U) ? FILE_FLAG_BACKUP_SEMANTICS
                                                       : FILE_ATTRIBUTE_NORMAL;
    native = CreateFileA(cstr, lh_os_fs_can_win_desired(which),
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, lh_null,
                         OPEN_EXISTING, flags, lh_null);
    if (native == INVALID_HANDLE_VALUE)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    (void)CloseHandle(native);
    return lh_bool_true;
}
#else
static int
lh_os_fs_can_posix_mode(lh_os_fs_access_t which)
{
    if (which == lh_os_fs_access_writable)
    {
        return W_OK;
    }
    if (which == lh_os_fs_access_executable)
    {
        return X_OK;
    }
    return R_OK;
}

static lh_bool_t
lh_os_fs_can_native(const lh_os_fs_path_t *path, lh_os_fs_access_t which)
{
    if (access(lh_str_get_data(lh_os_fs_path_get_text_as_const(path)),
               lh_os_fs_can_posix_mode(which)) != 0)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_bool_true;
}
#endif

lh_bool_t
lh_os_fs_can(const lh_os_fs_path_t *path, lh_os_fs_access_t which)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_if(which != lh_os_fs_access_readable && which != lh_os_fs_access_writable &&
                             which != lh_os_fs_access_executable,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (lh_os_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    return lh_os_fs_can_native(path, which);
}
