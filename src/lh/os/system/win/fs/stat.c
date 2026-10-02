#include <lh/os/system/fs/stat.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/str.h>
#include <lh/os/system/win/filetime.h>
#include <lh/os/system/win/fs/kind.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/bit/half.h>
#include <lh/util/math.h>

LH_ATTRIBUTE_STATIC
lh_fs_perm_t
lh_os_system_fs_perm_from_win_attrs(lh_os_system_win_dword_t attrs)
{
    lh_fs_perm_t perm = lh_bit_or(lh_bit_or(lh_fs_perm_irusr, lh_fs_perm_irgrp), lh_fs_perm_iroth);

    if (lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_READONLY)))
    {
        perm = lh_bit_or(perm, lh_bit_or(lh_bit_or(lh_fs_perm_iwusr, lh_fs_perm_iwgrp), lh_fs_perm_iwoth));
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_DIRECTORY)))
    {
        perm = lh_bit_or(perm, lh_bit_or(lh_bit_or(lh_fs_perm_ixusr, lh_fs_perm_ixgrp), lh_fs_perm_ixoth));
    }
    return perm;
}

LH_ATTRIBUTE_STATIC
lh_fs_attr_t
lh_os_system_fs_attr_from_win_attrs(lh_os_system_win_dword_t attrs)
{
    lh_fs_attr_t attr = 0U;

    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_HIDDEN)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_hidden);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_SYSTEM)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_system);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_ARCHIVE)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_archive);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_COMPRESSED)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_compressed);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_ENCRYPTED)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_encrypted);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_TEMPORARY)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_temporary);
    }
    return attr;
}

/* Only the find data carries the reparse tag (see kind.h), so a reparse
   point costs one FindFirstFile; anything else is decided from attrs. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_fs_is_symlink(lh_wstr_cptr path, lh_os_system_win_dword_t attrs, lh_bool_t *out)
{
    lh_os_system_win_find_data_t data;

    if (lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_WIN_FILE_ATTRIBUTE_REPARSE_POINT)))
    {
        *out = lh_bool_false;
        return lh_bool_true;
    }
    const lh_os_system_win_handle_t find = FindFirstFileW(path, lh_addr_of(data));
    if (lh_math_eq(find, LH_OS_SYSTEM_WIN_INVALID_HANDLE))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    (void)FindClose(find);
    *out = lh_os_system_fs_is_symlink_tag(attrs, data.dwReserved0);
    return lh_bool_true;
}

/* lh_os_system_fs_stat with the path already in UTF-16. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_fs_stat_wide(lh_wstr_cptr path, lh_fs_stat_t *out)
{
    lh_os_system_win_file_attribute_data_t info;
    lh_bool_t is_symlink;

    if (!GetFileAttributesExW(path, lh_os_system_win_get_file_ex_info_standard, lh_addr_of(info)))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    const lh_os_system_win_dword_t attrs = info.dwFileAttributes;
    if (!lh_os_system_fs_is_symlink(path, attrs, lh_addr_of(is_symlink)))
    {
        return lh_bool_false;
    }
    lh_fs_stat_set(
        out, lh_os_system_fs_kind_from_attrs(attrs, is_symlink),
        lh_os_system_fs_perm_from_win_attrs(attrs),
        lh_cast_static(lh_fs_size_t, lh_bit_make_u64(lh_cast_static(lh_u32_t, info.nFileSizeHigh),
                                                     lh_cast_static(lh_u32_t, info.nFileSizeLow))),
        lh_os_system_timestamp_from_filetime(lh_addr_of(info.ftLastAccessTime)),
        lh_os_system_timestamp_from_filetime(lh_addr_of(info.ftLastWriteTime)),
        lh_os_system_timestamp_from_filetime(lh_addr_of(info.ftCreationTime)),
        lh_os_system_fs_attr_from_win_attrs(attrs));
    return lh_bool_true;
}

lh_bool_t
lh_os_system_fs_stat(lh_str_cptr path, lh_fs_stat_t *out)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_ref(out);
    lh_os_str_t os_path;
    const lh_bool_t ok = lh_os_system_str_init_by_utf8(lh_addr_of(os_path), path) &&
                         lh_os_system_fs_stat_wide(lh_os_str_get_data(lh_addr_of(os_path)), out);

    lh_os_str_deinit(lh_addr_of(os_path));
    return ok;
}
