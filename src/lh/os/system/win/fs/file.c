#include <lh/os/system/fs/file.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/os/system/win/ntdll.h>
#include <lh/cast/const.h>
#include <lh/util/return.h>
#include <lh/char/map.h>
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

/* The longest name one path component may have. */
#define LH_OS_SYSTEM_WIN_FS_NAME_MAX 255U

/* Put an NT status where the Win32 error of the calling thread is, and capture it. */
LH_ATTRIBUTE_STATIC
lh_void
lh_os_system_fs_file_capture_status(lh_os_system_win_handle_t ntdll, lh_long_t status)
{
    union
    {
        lh_ptr raw;
        lh_os_system_win_rtl_nt_status_to_dos_error_fn convert;
    } procedure;

    procedure.raw = GetProcAddress(ntdll, "RtlNtStatusToDosError");
    if (lh_null_ne(procedure.raw))
    {
        SetLastError(procedure.convert(status));
    }
    lh_os_system_error_capture();
}

/* Open one name in @p directory: a directory to go on with, or the file at the end. */
LH_ATTRIBUTE_STATIC
lh_os_system_win_handle_t
lh_os_system_fs_file_open_step(lh_os_system_win_handle_t ntdll,
                               lh_os_system_win_nt_create_file_fn create,
                               lh_os_system_win_handle_t directory, lh_str_cptr name,
                               lh_bool_t more)
{
    lh_os_str_t wide;

    lh_return_ifn(lh_os_system_str_init_by_utf8(lh_addr_of(wide), name),
                  LH_OS_SYSTEM_WIN_INVALID_HANDLE);
    lh_os_system_win_unicode_string_t text;

    text.Buffer = lh_cast_const(lh_wstr_ptr, lh_os_str_get_data(lh_addr_of(wide)));
    text.Length = lh_cast_static(lh_os_system_win_word_t,
                                 lh_os_str_get_size(lh_addr_of(wide)) * sizeof(lh_wchar_t));
    text.MaximumLength = text.Length;
    lh_os_system_win_object_attributes_t attributes;

    attributes.Length = lh_cast_static(lh_os_system_win_dword_t, sizeof(attributes));
    attributes.RootDirectory = directory;
    attributes.ObjectName = lh_addr_of(text);
    attributes.Attributes = LH_OS_SYSTEM_WIN_OBJ_CASE_INSENSITIVE;
    attributes.SecurityDescriptor = lh_null;
    attributes.SecurityQualityOfService = lh_null;
    lh_os_system_win_io_status_block_t status_block;

    lh_os_system_win_handle_t handle = LH_OS_SYSTEM_WIN_INVALID_HANDLE;
    lh_long_t status;

    status = create(lh_addr_of(handle),
                    more ? (LH_OS_SYSTEM_WIN_FILE_LIST_DIRECTORY | LH_OS_SYSTEM_WIN_FILE_TRAVERSE |
                            LH_OS_SYSTEM_WIN_FILE_READ_ATTRIBUTES | LH_OS_SYSTEM_WIN_SYNCHRONIZE)
                         : (LH_OS_SYSTEM_WIN_GENERIC_READ | LH_OS_SYSTEM_WIN_SYNCHRONIZE),
                    lh_addr_of(attributes), lh_addr_of(status_block), lh_null, 0UL,
                    LH_OS_SYSTEM_WIN_FILE_SHARE_READ | LH_OS_SYSTEM_WIN_FILE_SHARE_WRITE |
                        LH_OS_SYSTEM_WIN_FILE_SHARE_DELETE,
                    LH_OS_SYSTEM_WIN_NT_FILE_OPEN,
                    LH_OS_SYSTEM_WIN_NT_FILE_SYNCHRONOUS_IO_NONALERT |
                        LH_OS_SYSTEM_WIN_NT_FILE_OPEN_REPARSE_POINT |
                        (more ? LH_OS_SYSTEM_WIN_NT_FILE_DIRECTORY_FILE
                              : LH_OS_SYSTEM_WIN_NT_FILE_NON_DIRECTORY_FILE),
                    lh_null, 0UL);
    lh_os_str_deinit(lh_addr_of(wide));
    if (status < 0)
    {
        lh_os_system_fs_file_capture_status(ntdll, status);
        return LH_OS_SYSTEM_WIN_INVALID_HANDLE;
    }
    return handle;
}

lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open_read_at(lh_os_system_fs_file_handle_t root, lh_str_cptr path,
                                  lh_fs_size_t *size)
{
    lh_os_system_win_handle_t ntdll = lh_null;
    union
    {
        lh_ptr raw;
        lh_os_system_win_nt_create_file_fn create;
    } procedure;
    lh_os_system_win_handle_t directory;
    lh_bool_t owned = lh_bool_false;
    lh_str_cptr part = path;

    lh_assert_runtime_ref(path);
    lh_assert_runtime_ref(size);
    if (lh_math_eq(root, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID))
    {
        lh_os_set_last_error_lit(lh_os_error_code_invalid_argument, "expected an open directory");
        return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    }
    if (!GetModuleHandleExW(0UL, L"ntdll.dll", lh_addr_of(ntdll)) ||
        lh_null_eq(procedure.raw = GetProcAddress(ntdll, "NtCreateFile")))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    }
    /* Borrow root until the first component is open; every later directory is ours. */
    directory = lh_os_system_fs_file_native(root);
    for (;;)
    {
        lh_char_t name[LH_OS_SYSTEM_WIN_FS_NAME_MAX + 1U];
        lh_os_system_win_by_handle_file_information_t info;
        lh_os_system_win_handle_t next;
        lh_usize_t length = 0U;
        lh_bool_t more;

        while (part[length] != lh_char_map_nul && part[length] != lh_char_map_slash &&
               part[length] != lh_char_map_backslash && part[length] != lh_char_map_colon &&
               length < LH_OS_SYSTEM_WIN_FS_NAME_MAX)
        {
            ++length;
        }
        if (length == 0U || (part[length] != lh_char_map_nul && part[length] != lh_char_map_slash) ||
            (length == 1U && part[0] == lh_char_map_period) ||
            (length == 2U && part[0] == lh_char_map_period && part[1] == lh_char_map_period))
        {
            lh_os_set_last_error_lit(lh_os_error_code_invalid_argument,
                                     "expected a relative file path without dot components");
            if (owned)
            {
                (void)CloseHandle(directory);
            }
            return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
        }
        lh_ptr_deref(lh_str_ptr_copy(name, sizeof(name) - 1U, part, length)) = lh_char_map_nul;
        more = part[length] == lh_char_map_slash ? lh_bool_true : lh_bool_false;
        next = lh_os_system_fs_file_open_step(ntdll, procedure.create, directory, name, more);
        if (owned)
        {
            (void)CloseHandle(directory);
        }
        lh_return_if(lh_math_eq(next, LH_OS_SYSTEM_WIN_INVALID_HANDLE),
                     LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
        /* A reparse point is opened as itself: a link or a junction is refused, not followed. */
        if (!GetFileInformationByHandle(next, lh_addr_of(info)) ||
            (info.dwFileAttributes & LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_REPARSE_POINT) != 0UL)
        {
            lh_os_set_last_error_lit(lh_os_error_code_invalid_argument,
                                     "a link or a reparse point is in the path");
            (void)CloseHandle(next);
            return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
        }
        if (!more)
        {
            lh_ptr_deref(size) = lh_cast_static(
                lh_fs_size_t, (lh_cast_static(lh_u64_t, info.nFileSizeHigh) << 32) |
                                  lh_cast_static(lh_u64_t, info.nFileSizeLow));
            return lh_cast_reinterpret(lh_os_system_fs_file_handle_t, next);
        }
        directory = next;
        owned = lh_bool_true;
        part += length + 1U;
    }
}

lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open_root(lh_str_cptr path)
{
    lh_os_str_t os_path;

    lh_assert_runtime_ref(path);
    const lh_bool_t ok = lh_os_system_str_init_by_utf8(lh_addr_of(os_path), path);
    const lh_os_system_win_handle_t native =
        ok ? CreateFileW(lh_os_str_get_data(lh_addr_of(os_path)),
                         LH_OS_SYSTEM_WIN_FILE_LIST_DIRECTORY | LH_OS_SYSTEM_WIN_FILE_TRAVERSE |
                             LH_OS_SYSTEM_WIN_FILE_READ_ATTRIBUTES | LH_OS_SYSTEM_WIN_SYNCHRONIZE,
                         LH_OS_SYSTEM_WIN_FILE_SHARE_READ | LH_OS_SYSTEM_WIN_FILE_SHARE_WRITE |
                             LH_OS_SYSTEM_WIN_FILE_SHARE_DELETE,
                         lh_null, LH_OS_SYSTEM_WIN_OPEN_EXISTING,
                         LH_OS_SYSTEM_WIN_FILE_FLAG_BACKUP_SEMANTICS, lh_null)
           : LH_OS_SYSTEM_WIN_INVALID_HANDLE;

    lh_os_str_deinit(lh_addr_of(os_path));
    if (lh_math_eq(native, LH_OS_SYSTEM_WIN_INVALID_HANDLE))
    {
        if (ok)
        {
            lh_os_system_error_capture();
        }
        return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    }
    return lh_cast_reinterpret(lh_os_system_fs_file_handle_t, native);
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
