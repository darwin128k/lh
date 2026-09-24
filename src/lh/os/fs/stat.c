#include <lh/os/fs/stat.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system.h>
#include <lh/os/system/error/capture.h>
#include <lh/str.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    ifndef IO_REPARSE_TAG_SYMLINK
#        define IO_REPARSE_TAG_SYMLINK 0xA000000CUL
#    endif
#else
#    include <sys/stat.h>
#endif

#define LH_OS_FS_FILETIME_UNIX_EPOCH 116444736000000000ULL
#define LH_OS_FS_FILETIME_HZ 10000000ULL
#define LH_OS_FS_PERM_UNIX_MASK 07777U

void
lh_os_fs_stat_init(lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    self->kind = lh_os_fs_kind_other;
    self->perm = 0U;
    self->size = 0U;
    self->atime = 0;
    self->mtime = 0;
    self->ctime = 0;
    self->attr = 0U;
}

void
lh_os_fs_stat_assign(lh_os_fs_stat_t *self, const lh_os_fs_stat_t *other)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);
    lh_ptr_deref(self) = lh_ptr_deref(other);
}

lh_os_fs_kind_t
lh_os_fs_stat_get_kind(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_os_fs_perm_t
lh_os_fs_stat_get_perm(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->perm;
}

lh_os_fs_size_t
lh_os_fs_stat_get_size(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_os_fs_time_t
lh_os_fs_stat_get_atime(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->atime;
}

lh_os_fs_time_t
lh_os_fs_stat_get_mtime(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mtime;
}

lh_os_fs_time_t
lh_os_fs_stat_get_ctime(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ctime;
}

lh_os_fs_attr_t
lh_os_fs_stat_get_attr(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->attr;
}

LH_ATTRIBUTE_STATIC
void
lh_os_fs_stat_fill(lh_os_fs_stat_t *self, lh_os_fs_kind_t kind, lh_os_fs_perm_t perm,
                   lh_os_fs_size_t size, lh_os_fs_time_t atime, lh_os_fs_time_t mtime,
                   lh_os_fs_time_t ctime, lh_os_fs_attr_t attr)
{
    self->kind = kind;
    self->perm = perm;
    self->size = size;
    self->atime = atime;
    self->mtime = mtime;
    self->ctime = ctime;
    self->attr = attr;
}

/*
 * Pure decoders for Windows' on-disk bit formats (FILE_ATTRIBUTE_* flags,
 * FILETIME's 100ns-ticks-since-1601). Deliberately built on portable
 * lh_u32_t/lh_u64_t, not DWORD/FILETIME/ULARGE_INTEGER, and on our own
 * mirrors of the FILE_ATTRIBUTE_* numeric values (stable, documented Win32
 * ABI constants, unchanged for decades) instead of <windows.h>'s. Only
 * called from the Windows branch of lh_os_fs_stat below, so guarded by the
 * same #if as everything else that's Windows-only.
 */
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS

#define LH_OS_FS_WIN_ATTR_READONLY 0x00000001UL
#define LH_OS_FS_WIN_ATTR_HIDDEN 0x00000002UL
#define LH_OS_FS_WIN_ATTR_SYSTEM 0x00000004UL
#define LH_OS_FS_WIN_ATTR_DIRECTORY 0x00000010UL
#define LH_OS_FS_WIN_ATTR_ARCHIVE 0x00000020UL
#define LH_OS_FS_WIN_ATTR_TEMPORARY 0x00000100UL
#define LH_OS_FS_WIN_ATTR_COMPRESSED 0x00000800UL
#define LH_OS_FS_WIN_ATTR_ENCRYPTED 0x00004000UL

/**
 * @brief Raw `FILE_ATTRIBUTE_*` bitmask (`dwFileAttributes`), before it's
 *        decoded into ::lh_os_fs_perm_t/::lh_os_fs_attr_t/::lh_os_fs_kind_t.
 *        Never crosses the public API — internal to this file.
 */
typedef lh_u32_t lh_os_fs_win_attrs_t;

LH_ATTRIBUTE_STATIC
lh_os_fs_time_t
lh_os_fs_time_from_filetime_ticks(lh_u64_t ticks)
{
    if (lh_math_lt(ticks, LH_OS_FS_FILETIME_UNIX_EPOCH))
    {
        return 0;
    }
    return lh_cast_static(lh_os_fs_time_t,
                          lh_math_div(lh_math_sub(ticks, LH_OS_FS_FILETIME_UNIX_EPOCH), LH_OS_FS_FILETIME_HZ));
}

LH_ATTRIBUTE_STATIC
lh_u64_t
lh_os_fs_u64_from_win_parts(lh_u32_t high, lh_u32_t low)
{
    return lh_bit_or(lh_bit_shl(lh_cast_static(lh_u64_t, high), 32), lh_cast_static(lh_u64_t, low));
}

LH_ATTRIBUTE_STATIC
lh_os_fs_perm_t
lh_os_fs_perm_from_win_attrs(lh_os_fs_win_attrs_t attrs)
{
    lh_os_fs_perm_t perm;

    perm = lh_bit_or(lh_bit_or(lh_os_fs_perm_irusr, lh_os_fs_perm_irgrp), lh_os_fs_perm_iroth);
    if (lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_READONLY)))
    {
        perm = lh_bit_or(perm, lh_bit_or(lh_bit_or(lh_os_fs_perm_iwusr, lh_os_fs_perm_iwgrp), lh_os_fs_perm_iwoth));
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_DIRECTORY)))
    {
        perm = lh_bit_or(perm, lh_bit_or(lh_bit_or(lh_os_fs_perm_ixusr, lh_os_fs_perm_ixgrp), lh_os_fs_perm_ixoth));
    }
    return perm;
}

LH_ATTRIBUTE_STATIC
lh_os_fs_attr_t
lh_os_fs_attr_from_win_attrs(lh_os_fs_win_attrs_t attrs)
{
    lh_os_fs_attr_t attr;

    attr = 0U;
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_HIDDEN)))
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_hidden);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_SYSTEM)))
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_system);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_ARCHIVE)))
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_archive);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_COMPRESSED)))
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_compressed);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_ENCRYPTED)))
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_encrypted);
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_TEMPORARY)))
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_temporary);
    }
    return attr;
}

LH_ATTRIBUTE_STATIC
lh_os_fs_kind_t
lh_os_fs_kind_from_win_attrs(lh_os_fs_win_attrs_t attrs, lh_bool_t is_symlink)
{
    if (is_symlink)
    {
        return lh_os_fs_kind_symlink;
    }
    if (!lh_math_is_zero(lh_bit_and(attrs, LH_OS_FS_WIN_ATTR_DIRECTORY)))
    {
        return lh_os_fs_kind_dir;
    }
    return lh_os_fs_kind_file;
}

LH_ATTRIBUTE_STATIC
void
lh_os_fs_stat_fill_from_win_attrs(lh_os_fs_stat_t *out, lh_os_fs_win_attrs_t attrs, lh_u32_t size_high,
                                  lh_u32_t size_low, lh_u64_t atime_ticks, lh_u64_t mtime_ticks,
                                  lh_u64_t ctime_ticks, lh_bool_t is_symlink, lh_os_fs_attr_t extra)
{
    lh_os_fs_stat_fill(out, lh_os_fs_kind_from_win_attrs(attrs, is_symlink),
                       lh_os_fs_perm_from_win_attrs(attrs),
                       lh_cast_static(lh_os_fs_size_t, lh_os_fs_u64_from_win_parts(size_high, size_low)),
                       lh_os_fs_time_from_filetime_ticks(atime_ticks),
                       lh_os_fs_time_from_filetime_ticks(mtime_ticks),
                       lh_os_fs_time_from_filetime_ticks(ctime_ticks),
                       lh_bit_or(lh_os_fs_attr_from_win_attrs(attrs), extra));
}

#else

/*
 * Pure decoder for POSIX st_mode's S_IFMT bit format. Deliberately built on
 * a portable lh_u32_t, not mode_t, and on our own mirrors of the S_IF*
 * numeric values — not <sys/stat.h>'s. These values are not POSIX-mandated
 * numbers, but every mainstream Unix (Linux, BSD, macOS, Solaris) has used
 * them unchanged since traditional Unix, so they're safe to hardcode. Only
 * called from the POSIX branch of lh_os_fs_stat below, hence the #else.
 */
#define LH_OS_FS_UNIX_S_IFMT 0170000U
#define LH_OS_FS_UNIX_S_IFLNK 0120000U
#define LH_OS_FS_UNIX_S_IFDIR 0040000U
#define LH_OS_FS_UNIX_S_IFREG 0100000U

/**
 * @brief Raw POSIX `st_mode` bits (file-type bits + permission bits,
 *        undecoded). Not ::lh_os_fs_file_mode_t (that's our own read/write
 *        open-mode discriminator — unrelated). Never crosses the public
 *        API — internal to this file.
 */
typedef lh_u32_t lh_os_fs_unix_mode_t;

LH_ATTRIBUTE_STATIC
lh_os_fs_kind_t
lh_os_fs_kind_from_unix_mode(lh_os_fs_unix_mode_t mode)
{
    lh_u32_t fmt;

    fmt = lh_bit_and(mode, LH_OS_FS_UNIX_S_IFMT);
    if (lh_math_eq(fmt, LH_OS_FS_UNIX_S_IFLNK))
    {
        return lh_os_fs_kind_symlink;
    }
    if (lh_math_eq(fmt, LH_OS_FS_UNIX_S_IFDIR))
    {
        return lh_os_fs_kind_dir;
    }
    if (lh_math_eq(fmt, LH_OS_FS_UNIX_S_IFREG))
    {
        return lh_os_fs_kind_file;
    }
    return lh_os_fs_kind_other;
}

LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_fs_stat_fill_from_unix_fields(lh_os_fs_stat_t *out, lh_os_fs_unix_mode_t mode_bits, lh_s64_t size,
                                    lh_os_fs_time_t atime, lh_os_fs_time_t mtime,
                                    lh_os_fs_time_t ctime, lh_os_fs_attr_t extra)
{
    if (lh_math_is_negative(size))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_negative_size,
                             lh_os_error_desc_lit("file size is negative")));
        return lh_bool_false;
    }
    lh_os_fs_stat_fill(out, lh_os_fs_kind_from_unix_mode(mode_bits),
                       lh_cast_static(lh_os_fs_perm_t, lh_bit_and(mode_bits, LH_OS_FS_PERM_UNIX_MASK)),
                       lh_cast_static(lh_os_fs_size_t, size), atime, mtime, ctime, extra);
    return lh_bool_true;
}

#endif /* LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS */

lh_bool_t
lh_os_fs_stat(const lh_fs_path_t *path, lh_os_fs_stat_t *out)
{
    lh_str_t buf;
    lh_str_cptr cstr;
    lh_bool_t ok;

    lh_assert_runtime_ref(out);
    if (lh_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }

    cstr = lh_fs_path_to_cstr(path, lh_addr_of(buf));

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        WIN32_FILE_ATTRIBUTE_DATA info;
        lh_bool_t is_symlink;

        if (!GetFileAttributesExA(cstr, GetFileExInfoStandard, lh_addr_of(info)))
        {
            lh_os_system_error_capture();
            lh_str_deinit(lh_addr_of(buf));
            return lh_bool_false;
        }
        is_symlink = lh_bool_false;
        if (!lh_math_is_zero(lh_bit_and(info.dwFileAttributes, FILE_ATTRIBUTE_REPARSE_POINT)))
        {
            WIN32_FIND_DATAA data;
            HANDLE find;

            find = FindFirstFileA(cstr, lh_addr_of(data));
            if (lh_math_eq(find, INVALID_HANDLE_VALUE))
            {
                lh_os_system_error_capture();
                lh_str_deinit(lh_addr_of(buf));
                return lh_bool_false;
            }
            FindClose(find);
            is_symlink = lh_cast_static(lh_bool_t, lh_math_eq(data.dwReserved0, IO_REPARSE_TAG_SYMLINK));
        }
        /* Native FILETIME -> a plain u64 tick count; the actual math lives
           in the portable lh_os_fs_time_from_filetime_ticks above. */
        lh_os_fs_stat_fill_from_win_attrs(
            out, lh_cast_static(lh_os_fs_win_attrs_t, info.dwFileAttributes),
            lh_cast_static(lh_u32_t, info.nFileSizeHigh), lh_cast_static(lh_u32_t, info.nFileSizeLow),
            lh_os_fs_u64_from_win_parts(lh_cast_static(lh_u32_t, info.ftLastAccessTime.dwHighDateTime),
                                        lh_cast_static(lh_u32_t, info.ftLastAccessTime.dwLowDateTime)),
            lh_os_fs_u64_from_win_parts(lh_cast_static(lh_u32_t, info.ftLastWriteTime.dwHighDateTime),
                                        lh_cast_static(lh_u32_t, info.ftLastWriteTime.dwLowDateTime)),
            lh_os_fs_u64_from_win_parts(lh_cast_static(lh_u32_t, info.ftCreationTime.dwHighDateTime),
                                        lh_cast_static(lh_u32_t, info.ftCreationTime.dwLowDateTime)),
            is_symlink, lh_fs_path_is_hidden(path) ? lh_os_fs_attr_hidden : 0U);
        ok = lh_bool_true;
    }
#else
    {
        struct stat info;

        if (lh_math_ne(lstat(cstr, lh_addr_of(info)), 0))
        {
            lh_os_system_error_capture();
            lh_str_deinit(lh_addr_of(buf));
            return lh_bool_false;
        }
        ok = lh_os_fs_stat_fill_from_unix_fields(
            out, lh_cast_static(lh_os_fs_unix_mode_t, info.st_mode),
            lh_cast_static(lh_s64_t, info.st_size), lh_cast_static(lh_os_fs_time_t, info.st_atime),
            lh_cast_static(lh_os_fs_time_t, info.st_mtime),
            lh_cast_static(lh_os_fs_time_t, info.st_ctime), lh_fs_path_is_hidden(path) ? lh_os_fs_attr_hidden : 0U);
    }
#endif

    lh_str_deinit(lh_addr_of(buf));
    return ok;
}

lh_bool_t
lh_os_fs_mtime(const lh_fs_path_t *path, lh_os_fs_time_t *out)
{
    lh_os_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_os_fs_stat_get_mtime(lh_addr_of(st));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_is_readable(const lh_fs_path_t *path, lh_bool_t *out)
{
    lh_os_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_os_fs_perm_is_readable(lh_os_fs_stat_get_perm(lh_addr_of(st)));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_is_writable(const lh_fs_path_t *path, lh_bool_t *out)
{
    lh_os_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_os_fs_perm_is_writable(lh_os_fs_stat_get_perm(lh_addr_of(st)));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_is_executable(const lh_fs_path_t *path, lh_bool_t *out)
{
    lh_os_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_os_fs_perm_is_executable(lh_os_fs_stat_get_perm(lh_addr_of(st)));
    return lh_bool_true;
}
