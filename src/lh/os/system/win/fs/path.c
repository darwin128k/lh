#include <lh/os/system/fs/path.h>
#include <lh/assert.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/util/str/ptr.h>
#include <lh/math.h>

lh_fs_path_style_t
lh_os_system_fs_path_style_native(void)
{
    return lh_fs_path_style_windows;
}

/** Vista+ function; loaded dynamically so older systems can fail without a new import. */
typedef lh_os_system_win_dword_t(LH_OS_SYSTEM_WIN_CALL *lh_os_system_fs_path_final_fn)(
    lh_os_system_win_handle_t file, lh_wstr_ptr path, lh_os_system_win_dword_t capacity,
    lh_os_system_win_dword_t flags);

lh_bool_t
lh_os_system_fs_path_canonical(lh_str_cptr path, lh_str_t *out)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_ref(out);
    if (!*path)
    {
        lh_os_set_last_error_lit(lh_os_error_code_path_empty, "path is empty");
        return lh_bool_false;
    }
    const lh_os_system_fs_path_final_fn final_path = lh_cast_reinterpret(
        lh_os_system_fs_path_final_fn,
        GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetFinalPathNameByHandleW"));
    if (lh_null_eq(final_path))
    {
        lh_os_set_last_error_lit(lh_os_error_code_not_supported,
                                 "final path resolution is not supported by this system");
        return lh_bool_false;
    }
    lh_os_str_t native;
    const lh_bool_t converted = lh_os_system_str_init_by_utf8(lh_addr_of(native), path);
    const lh_os_system_win_handle_t file =
        converted
            ? CreateFileW(lh_os_str_get_data(lh_addr_of(native)), 0UL,
                          LH_OS_SYSTEM_WIN_FILE_SHARE_READ | LH_OS_SYSTEM_WIN_FILE_SHARE_WRITE |
                              LH_OS_SYSTEM_WIN_FILE_SHARE_DELETE,
                          lh_null, LH_OS_SYSTEM_WIN_OPEN_EXISTING,
                          LH_OS_SYSTEM_WIN_FILE_FLAG_BACKUP_SEMANTICS, lh_null)
            : LH_OS_SYSTEM_WIN_INVALID_HANDLE;
    if (converted && file == LH_OS_SYSTEM_WIN_INVALID_HANDLE)
    {
        lh_os_system_error_capture();
    }
    lh_os_str_deinit(lh_addr_of(native));
    if (file == LH_OS_SYSTEM_WIN_INVALID_HANDLE)
    {
        return lh_bool_false;
    }
    /* NT's maximum extended path length, in UTF-16 code units. */
    lh_wchar_t buffer[32768];
    const lh_os_system_win_dword_t capacity =
        lh_cast_static(lh_os_system_win_dword_t, sizeof buffer / sizeof buffer[0]);
    const lh_os_system_win_dword_t count = final_path(file, buffer, capacity, 0UL);
    if (!count)
    {
        lh_os_system_error_capture();
    }
    (void)CloseHandle(file);
    if (!count)
    {
        return lh_bool_false;
    }
    if (count >= capacity)
    {
        lh_os_set_last_error_lit(lh_os_error_code_name_too_long, "resolved path is too long");
        return lh_bool_false;
    }
    lh_str_t resolved;
    lh_str_init(lh_addr_of(resolved));
    const lh_bool_t ok = lh_os_system_str_to_utf8(buffer, count, lh_addr_of(resolved));
    if (ok)
    {
        lh_str_cptr text = lh_str_get_data(lh_addr_of(resolved));
        lh_usize_t size = lh_str_get_size(lh_addr_of(resolved));
        lh_str_clear(out);
        if (size >= 8U && text[0] == '\\' && text[1] == '\\' && text[2] == '?' && text[3] == '\\' &&
            text[4] == 'U' && text[5] == 'N' && text[6] == 'C' && text[7] == '\\')
        {
            lh_str_push_back(out, '\\');
            lh_str_push_back(out, '\\');
            text += 8U;
            size -= 8U;
        }
        else if (size >= 4U && text[0] == '\\' && text[1] == '\\' && text[2] == '?' &&
                 text[3] == '\\')
        {
            text += 4U;
            size -= 4U;
        }
        lh_str_append(out, text, size);
    }
    lh_str_deinit(lh_addr_of(resolved));
    return ok;
}
