#include <lh/os/system/fs/path.h>
#include <lh/attribute/unused.h>
#include <lh/char/map.h>
#include <lh/math/ops.h>
#include <lh/util/ptr.h>
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
    if (!lh_ptr_deref(path))
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
        converted ? CreateFileW(lh_os_str_get_data(lh_addr_of(native)), 0UL,
                                lh_math_bit_or(lh_math_bit_or(LH_OS_SYSTEM_WIN_FILE_SHARE_READ,
                                                              LH_OS_SYSTEM_WIN_FILE_SHARE_WRITE),
                                               LH_OS_SYSTEM_WIN_FILE_SHARE_DELETE),
                                lh_null, LH_OS_SYSTEM_WIN_OPEN_EXISTING,
                                LH_OS_SYSTEM_WIN_FILE_FLAG_BACKUP_SEMANTICS, lh_null)
                  : LH_OS_SYSTEM_WIN_INVALID_HANDLE;
    if (converted && lh_math_eq(file, LH_OS_SYSTEM_WIN_INVALID_HANDLE))
    {
        lh_os_system_error_capture();
    }
    lh_os_str_deinit(lh_addr_of(native));
    if (lh_math_eq(file, LH_OS_SYSTEM_WIN_INVALID_HANDLE))
    {
        return lh_bool_false;
    }
    /* NT's maximum extended path length, in UTF-16 code units. */
    lh_wchar_t buffer[32768];
    const lh_os_system_win_dword_t capacity =
        lh_cast_static(lh_os_system_win_dword_t, lh_math_div(sizeof buffer, sizeof buffer[0]));
    const lh_os_system_win_dword_t count = final_path(file, buffer, capacity, 0UL);
    if (!count)
    {
        lh_os_system_error_capture();
    }
    LH_ATTRIBUTE_UNUSED(CloseHandle(file));
    if (!count)
    {
        return lh_bool_false;
    }
    if (lh_math_ge(count, capacity))
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
        if (lh_math_ge(size, 8U) && lh_math_eq(text[0], lh_char_map_backslash) &&
            lh_math_eq(text[1], lh_char_map_backslash) &&
            lh_math_eq(text[2], lh_char_map_question) &&
            lh_math_eq(text[3], lh_char_map_backslash) &&
            lh_math_eq(text[4], lh_char_map_capital_u) &&
            lh_math_eq(text[5], lh_char_map_capital_n) &&
            lh_math_eq(text[6], lh_char_map_capital_c) &&
            lh_math_eq(text[7], lh_char_map_backslash))
        {
            lh_str_push_back(out, lh_char_map_backslash);
            lh_str_push_back(out, lh_char_map_backslash);
            text = lh_math_add(text, 8U);
            size = lh_math_sub(size, 8U);
        }
        else if (lh_math_ge(size, 4U) && lh_math_eq(text[0], lh_char_map_backslash) &&
                 lh_math_eq(text[1], lh_char_map_backslash) &&
                 lh_math_eq(text[2], lh_char_map_question) &&
                 lh_math_eq(text[3], lh_char_map_backslash))
        {
            text = lh_math_add(text, 4U);
            size = lh_math_sub(size, 4U);
        }
        lh_str_append(out, text, size);
    }
    lh_str_deinit(lh_addr_of(resolved));
    return ok;
}
