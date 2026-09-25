#include <lh/os/system/fs/file.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/system/error/capture.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

LH_ATTRIBUTE_STATIC
HANDLE
lh_os_system_fs_file_native(lh_os_system_fs_file_handle_t handle)
{
    return lh_cast_reinterpret(HANDLE, handle);
}

LH_ATTRIBUTE_STATIC
DWORD
lh_os_system_fs_file_size(lh_usize_t size)
{
    return lh_cast_static(DWORD, lh_math_min(size, lh_cast_static(lh_usize_t, MAXDWORD)));
}

LH_ATTRIBUTE_STATIC
DWORD
lh_os_system_fs_file_access(lh_os_system_fs_file_mode_t mode)
{
    if (lh_math_eq(mode, lh_os_system_fs_file_mode_read))
    {
        return GENERIC_READ;
    }
    if (lh_math_eq(mode, lh_os_system_fs_file_mode_write))
    {
        return GENERIC_WRITE;
    }
    return GENERIC_READ | GENERIC_WRITE;
}

LH_ATTRIBUTE_STATIC
DWORD
lh_os_system_fs_file_disposition(lh_os_system_fs_file_mode_t mode)
{
    if (lh_math_eq(mode, lh_os_system_fs_file_mode_read))
    {
        return OPEN_EXISTING;
    }
    if (lh_math_eq(mode, lh_os_system_fs_file_mode_write))
    {
        return CREATE_ALWAYS;
    }
    return OPEN_ALWAYS;
}

lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open(lh_str_cptr path, lh_os_system_fs_file_mode_t mode)
{
    HANDLE native;

    lh_assert_runtime_ref(path);
    lh_assert_runtime_if(!lh_os_system_fs_file_mode_is_readable(mode) &&
                             !lh_os_system_fs_file_mode_is_writable(mode),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    native = CreateFileA(path, lh_os_system_fs_file_access(mode), FILE_SHARE_READ, lh_null,
                         lh_os_system_fs_file_disposition(mode), FILE_ATTRIBUTE_NORMAL, lh_null);
    if (lh_math_eq(native, INVALID_HANDLE_VALUE))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    }
    return lh_cast_reinterpret(lh_os_system_fs_file_handle_t, native);
}

void
lh_os_system_fs_file_close(lh_os_system_fs_file_handle_t handle)
{
    (void)CloseHandle(lh_os_system_fs_file_native(handle));
}

lh_ssize_t
lh_os_system_fs_file_read(lh_os_system_fs_file_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    DWORD result;

    if (!ReadFile(lh_os_system_fs_file_native(handle), buf, lh_os_system_fs_file_size(size),
                  lh_addr_of(result), lh_null))
    {
        lh_os_system_error_capture();
        return lh_cast_static(lh_ssize_t, -1);
    }
    return lh_cast_static(lh_ssize_t, result);
}

lh_ssize_t
lh_os_system_fs_file_write(lh_os_system_fs_file_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    DWORD result;

    if (!WriteFile(lh_os_system_fs_file_native(handle), buf, lh_os_system_fs_file_size(size),
                   lh_addr_of(result), lh_null))
    {
        lh_os_system_error_capture();
        return lh_cast_static(lh_ssize_t, -1);
    }
    return lh_cast_static(lh_ssize_t, result);
}
