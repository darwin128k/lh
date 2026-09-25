#include <lh/os/system/fs/stat.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/system/error/capture.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef IO_REPARSE_TAG_SYMLINK
#    define IO_REPARSE_TAG_SYMLINK 0xA000000CUL
#endif

#define LH_OS_SYSTEM_FS_FILETIME_UNIX_EPOCH 116444736000000000ULL
#define LH_OS_SYSTEM_FS_FILETIME_HZ 10000000ULL

#define LH_OS_SYSTEM_FS_WIN_ATTR_READONLY 0x00000001UL
#define LH_OS_SYSTEM_FS_WIN_ATTR_HIDDEN 0x00000002UL
#define LH_OS_SYSTEM_FS_WIN_ATTR_SYSTEM 0x00000004UL
#define LH_OS_SYSTEM_FS_WIN_ATTR_DIRECTORY 0x00000010UL
#define LH_OS_SYSTEM_FS_WIN_ATTR_ARCHIVE 0x00000020UL
#define LH_OS_SYSTEM_FS_WIN_ATTR_TEMPORARY 0x00000100UL
#define LH_OS_SYSTEM_FS_WIN_ATTR_COMPRESSED 0x00000800UL
#define LH_OS_SYSTEM_FS_WIN_ATTR_ENCRYPTED 0x00004000UL

/*
 * Pure decoders for Windows' on-disk bit formats (FILE_ATTRIBUTE_* flags,
 * FILETIME's 100ns-ticks-since-1601). Built on portable lh_u32_t/lh_u64_t,
 * not DWORD/FILETIME/ULARGE_INTEGER, and on our own mirrors of the
 * FILE_ATTRIBUTE_* numeric values (stable, documented Win32 ABI constants,
 * unchanged for decades) instead of <windows.h>'s. The native structs are
 * unpacked once, in lh_os_system_fs_stat below.
 */

/**
 * @brief Raw `FILE_ATTRIBUTE_*` bitmask (`dwFileAttributes`), before it's
 *        decoded into ::lh_fs_perm_t/::lh_fs_attr_t/::lh_fs_kind_t.
 */
typedef lh_u32_t lh_os_system_fs_win_attrs_t;

LH_ATTRIBUTE_STATIC
lh_fs_time_t
lh_os_system_fs_time_from_filetime_ticks(lh_u64_t ticks)
{
    if (lh_math_lt(ticks, LH_OS_SYSTEM_FS_FILETIME_UNIX_EPOCH))
    {
        return 0;
    }
    return lh_cast_static(
        lh_fs_time_t,
        lh_math_div(lh_math_sub(ticks, LH_OS_SYSTEM_FS_FILETIME_UNIX_EPOCH), LH_OS_SYSTEM_FS_FILETIME_HZ));
}

LH_ATTRIBUTE_STATIC
lh_u64_t
lh_os_system_fs_u64_from_win_parts(lh_u32_t high, lh_u32_t low)
{
    return lh_bit_or(lh_bit_shl(lh_cast_static(lh_u64_t, high), 32), lh_cast_static(lh_u64_t, low));
}

LH_ATTRIBUTE_STATIC
lh_fs_time_t
lh_os_system_fs_time_from_filetime(const FILETIME *time)
{
    return lh_os_system_fs_time_from_filetime_ticks(lh_os_system_fs_u64_from_win_parts(
        lh_cast_static(lh_u32_t, time->dwHighDateTime), lh_cast_static(lh_u32_t, time->dwLowDateTime)));
}

LH_ATTRIBUTE_STATIC
lh_fs_perm_t
lh_os_system_fs_perm_from_win_attrs(lh_os_system_fs_win_attrs_t attrs)
{
    lh_fs_perm_t perm;

    perm = lh_bit_or(lh_bit_or(lh_fs_perm_irusr, lh_fs_perm_irgrp), lh_fs_perm_iroth);
    if (lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_READONLY)))
    {
        perm = lh_bit_or(perm, lh_bit_or(lh_bit_or(lh_fs_perm_iwusr, lh_fs_perm_iwgrp), lh_fs_perm_iwoth));
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_DIRECTORY)))
    {
        perm = lh_bit_or(perm, lh_bit_or(lh_bit_or(lh_fs_perm_ixusr, lh_fs_perm_ixgrp), lh_fs_perm_ixoth));
    }
    return perm;
}

LH_ATTRIBUTE_STATIC
lh_fs_attr_t
lh_os_system_fs_attr_from_win_attrs(lh_os_system_fs_win_attrs_t attrs)
{
    lh_fs_attr_t attr;

    attr = 0U;
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_HIDDEN)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_hidden);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_SYSTEM)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_system);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_ARCHIVE)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_archive);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_COMPRESSED)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_compressed);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_ENCRYPTED)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_encrypted);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_TEMPORARY)))
    {
        attr = lh_bit_or(attr, lh_fs_attr_temporary);
    }
    return attr;
}

LH_ATTRIBUTE_STATIC
lh_fs_kind_t
lh_os_system_fs_kind_from_win_attrs(lh_os_system_fs_win_attrs_t attrs, lh_bool_t is_symlink)
{
    if (is_symlink)
    {
        return lh_fs_kind_symlink;
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_SYSTEM_FS_WIN_ATTR_DIRECTORY)))
    {
        return lh_fs_kind_dir;
    }
    return lh_fs_kind_file;
}

/*
 * A reparse point is not necessarily a symlink (junctions, dedup, cloud
 * placeholders are too); only the find data carries the reparse tag.
 */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_fs_is_symlink(lh_str_cptr path, lh_os_system_fs_win_attrs_t attrs, lh_bool_t *out)
{
    WIN32_FIND_DATAA data;
    HANDLE find;

    if (lh_math_is_zero(lh_bit_and(attrs, FILE_ATTRIBUTE_REPARSE_POINT)))
    {
        *out = lh_bool_false;
        return lh_bool_true;
    }
    find = FindFirstFileA(path, lh_addr_of(data));
    if (lh_math_eq(find, INVALID_HANDLE_VALUE))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    (void)FindClose(find);
    *out = lh_cast_static(lh_bool_t, lh_math_eq(data.dwReserved0, IO_REPARSE_TAG_SYMLINK));
    return lh_bool_true;
}

lh_bool_t
lh_os_system_fs_stat(lh_str_cptr path, lh_fs_stat_t *out)
{
    WIN32_FILE_ATTRIBUTE_DATA info;
    lh_os_system_fs_win_attrs_t attrs;
    lh_bool_t is_symlink;

    lh_assert_runtime_ref(path);
    lh_assert_runtime_ref(out);

    if (!GetFileAttributesExA(path, GetFileExInfoStandard, lh_addr_of(info)))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    attrs = lh_cast_static(lh_os_system_fs_win_attrs_t, info.dwFileAttributes);
    if (!lh_os_system_fs_is_symlink(path, attrs, lh_addr_of(is_symlink)))
    {
        return lh_bool_false;
    }
    lh_fs_stat_set(out, lh_os_system_fs_kind_from_win_attrs(attrs, is_symlink),
                   lh_os_system_fs_perm_from_win_attrs(attrs),
                   lh_cast_static(lh_fs_size_t,
                                  lh_os_system_fs_u64_from_win_parts(lh_cast_static(lh_u32_t, info.nFileSizeHigh),
                                                                     lh_cast_static(lh_u32_t, info.nFileSizeLow))),
                   lh_os_system_fs_time_from_filetime(lh_addr_of(info.ftLastAccessTime)),
                   lh_os_system_fs_time_from_filetime(lh_addr_of(info.ftLastWriteTime)),
                   lh_os_system_fs_time_from_filetime(lh_addr_of(info.ftCreationTime)),
                   lh_os_system_fs_attr_from_win_attrs(attrs));
    return lh_bool_true;
}
