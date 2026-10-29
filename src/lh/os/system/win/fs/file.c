#include <lh/os/system/fs/file.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/math.h>
#include <lh/os.h>
#include <lh/os/error/code.h>

LH_ATTRIBUTE_STATIC
lh_os_system_win_handle_t
lh_os_system_fs_file_native(lh_os_system_fs_file_handle_t handle)
{
    return lh_cast_reinterpret(lh_os_system_win_handle_t, handle);
}

LH_ATTRIBUTE_STATIC
lh_os_system_win_dword_t
lh_os_system_fs_file_size(lh_usize_t size)
{
    return lh_cast_static(lh_os_system_win_dword_t,
                          lh_math_min(size, lh_cast_static(lh_usize_t, LH_OS_SYSTEM_WIN_MAXDWORD)));
}

LH_ATTRIBUTE_STATIC
lh_os_system_win_dword_t
lh_os_system_fs_file_access(lh_fs_file_mode_t mode)
{
    if (lh_math_eq(mode, lh_fs_file_mode_read))
    {
        return LH_OS_SYSTEM_WIN_GENERIC_READ;
    }
    if (lh_math_eq(mode, lh_fs_file_mode_write))
    {
        return LH_OS_SYSTEM_WIN_GENERIC_WRITE;
    }
    if (lh_math_eq(mode, lh_fs_file_mode_append))
    {
        return LH_OS_SYSTEM_WIN_FILE_APPEND_DATA;
    }
    return LH_OS_SYSTEM_WIN_GENERIC_READ | LH_OS_SYSTEM_WIN_GENERIC_WRITE;
}

LH_ATTRIBUTE_STATIC
lh_os_system_win_dword_t
lh_os_system_fs_file_disposition(lh_fs_file_mode_t mode)
{
    if (lh_math_eq(mode, lh_fs_file_mode_read))
    {
        return LH_OS_SYSTEM_WIN_OPEN_EXISTING;
    }
    if (lh_math_eq(mode, lh_fs_file_mode_write))
    {
        return LH_OS_SYSTEM_WIN_CREATE_ALWAYS;
    }
    return LH_OS_SYSTEM_WIN_OPEN_ALWAYS;
}

lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open(lh_str_cptr path, lh_fs_file_mode_t mode)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_if(!lh_fs_file_mode_is_readable(mode) && !lh_fs_file_mode_is_writable(mode),
                         lh_runtime_error_code_invalid_argument);

    lh_os_str_t os_path;
    const lh_bool_t ok = lh_os_system_str_init_by_utf8(lh_addr_of(os_path), path);
    const lh_os_system_win_handle_t native =
        ok ? CreateFileW(lh_os_str_get_data(lh_addr_of(os_path)), lh_os_system_fs_file_access(mode),
                         LH_OS_SYSTEM_WIN_FILE_SHARE_READ, lh_null,
                         lh_os_system_fs_file_disposition(mode),
                         LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_NORMAL, lh_null)
           : LH_OS_SYSTEM_WIN_INVALID_HANDLE;

    lh_os_str_deinit(lh_addr_of(os_path));
    if (lh_math_eq(native, LH_OS_SYSTEM_WIN_INVALID_HANDLE))
    {
        /* A failed conversion already set its own error. */
        if (ok)
        {
            lh_os_system_error_capture();
        }
        return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    }
    return lh_cast_reinterpret(lh_os_system_fs_file_handle_t, native);
}

lh_void
lh_os_system_fs_file_close(lh_os_system_fs_file_handle_t handle)
{
    (void)CloseHandle(lh_os_system_fs_file_native(handle));
}

lh_ssize_t
lh_os_system_fs_file_read(lh_os_system_fs_file_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    lh_os_system_win_dword_t result;

    if (!ReadFile(lh_os_system_fs_file_native(handle), buf, lh_os_system_fs_file_size(size),
                  lh_addr_of(result), lh_null))
    {
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return lh_cast_static(lh_ssize_t, result);
}

lh_ssize_t
lh_os_system_fs_file_write(lh_os_system_fs_file_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    lh_os_system_win_dword_t result;

    if (!WriteFile(lh_os_system_fs_file_native(handle), buf, lh_os_system_fs_file_size(size),
                   lh_addr_of(result), lh_null))
    {
        lh_os_system_error_capture();
        return LH_OS_RESULT_INVALID;
    }
    return lh_cast_static(lh_ssize_t, result);
}

lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open_read_at(lh_os_system_fs_file_handle_t root, lh_str_cptr path,
                                  lh_fs_size_t *size)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_ref(size);
    (void)root;
    lh_os_set_last_error_lit(lh_os_error_code_not_supported,
                             "handle-relative file lookup is not supported by this backend");
    return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
}

lh_bool_t
lh_os_system_fs_file_remove(lh_str_cptr path)
{
    lh_os_str_t os_path;

    lh_assert_runtime_ref(path);
    const lh_bool_t ok = lh_os_system_str_init_by_utf8(lh_addr_of(os_path), path);
    const lh_bool_t removed = lh_cast_static(
        lh_bool_t, ok && lh_math_ne(DeleteFileW(lh_os_str_get_data(lh_addr_of(os_path))), 0));
    lh_os_str_deinit(lh_addr_of(os_path));
    /* A failed conversion already set its own error. */
    if (ok && !removed)
    {
        lh_os_system_error_capture();
    }
    return removed;
}
