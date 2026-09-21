#include <lh/os/fs/stat.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/char/dot.h>
#include <lh/compiler/os.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/error.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
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

static void
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

static lh_os_fs_attr_t
lh_os_fs_attr_hidden_from_path(const lh_os_fs_path_t *path)
{
    const lh_str_list_t *segments;
    lh_str_view_t last;
    lh_usize_t n;
    lh_usize_t size;

    segments = lh_os_fs_path_get_segments_as_const(path);
    n = lh_str_list_get_size(segments);
    if (n == 0U)
    {
        return 0U;
    }
    last = lh_str_as_view(lh_str_list_get_as_const(segments, n - 1U));
    if (lh_str_view_is_empty(lh_addr_of(last)))
    {
        return 0U;
    }
    size = lh_str_view_get_size(lh_addr_of(last));
    if (!lh_char_is_dot(lh_str_view_get_char_from_begin(lh_addr_of(last), 0U)))
    {
        return 0U;
    }
    if (size == 1U)
    {
        return 0U;
    }
    if (size == 2U && lh_char_is_dot(lh_str_view_get_char_from_begin(lh_addr_of(last), 1U)))
    {
        return 0U;
    }
    return lh_os_fs_attr_hidden;
}

/*
 * Pure decoders for Windows' on-disk bit formats (FILE_ATTRIBUTE_* flags,
 * FILETIME's 100ns-ticks-since-1601). Deliberately built on portable
 * lh_u32_t/lh_u64_t, not DWORD/FILETIME/ULARGE_INTEGER, and on our own
 * mirrors of the FILE_ATTRIBUTE_* numeric values (stable, documented Win32
 * ABI constants, unchanged for decades) instead of <windows.h>'s. None of
 * this is a native API call — it's arithmetic — so it needs no #if and
 * compiles/is callable on every OS lh targets, Windows included.
 */
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

static lh_os_fs_time_t
lh_os_fs_time_from_filetime_ticks(lh_u64_t ticks)
{
    if (ticks < LH_OS_FS_FILETIME_UNIX_EPOCH)
    {
        return 0;
    }
    return lh_cast_static(lh_os_fs_time_t, (ticks - LH_OS_FS_FILETIME_UNIX_EPOCH) / LH_OS_FS_FILETIME_HZ);
}

static lh_os_fs_size_t
lh_os_fs_size_from_win_parts(lh_u32_t high, lh_u32_t low)
{
    return lh_bit_or(lh_bit_shl(lh_cast_static(lh_u64_t, high), 32), lh_cast_static(lh_u64_t, low));
}

static lh_os_fs_perm_t
lh_os_fs_perm_from_win_attrs(lh_os_fs_win_attrs_t attrs)
{
    lh_os_fs_perm_t perm;

    perm = lh_os_fs_perm_irusr | lh_os_fs_perm_irgrp | lh_os_fs_perm_iroth;
    if ((attrs & LH_OS_FS_WIN_ATTR_READONLY) == 0U)
    {
        perm = lh_bit_or(perm, lh_os_fs_perm_iwusr | lh_os_fs_perm_iwgrp | lh_os_fs_perm_iwoth);
    }
    if ((attrs & LH_OS_FS_WIN_ATTR_DIRECTORY) != 0U)
    {
        perm = lh_bit_or(perm, lh_os_fs_perm_ixusr | lh_os_fs_perm_ixgrp | lh_os_fs_perm_ixoth);
    }
    return perm;
}

static lh_os_fs_attr_t
lh_os_fs_attr_from_win_attrs(lh_os_fs_win_attrs_t attrs)
{
    lh_os_fs_attr_t attr;

    attr = 0U;
    if ((attrs & LH_OS_FS_WIN_ATTR_HIDDEN) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_hidden);
    }
    if ((attrs & LH_OS_FS_WIN_ATTR_SYSTEM) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_system);
    }
    if ((attrs & LH_OS_FS_WIN_ATTR_ARCHIVE) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_archive);
    }
    if ((attrs & LH_OS_FS_WIN_ATTR_COMPRESSED) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_compressed);
    }
    if ((attrs & LH_OS_FS_WIN_ATTR_ENCRYPTED) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_encrypted);
    }
    if ((attrs & LH_OS_FS_WIN_ATTR_TEMPORARY) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_temporary);
    }
    return attr;
}

static lh_os_fs_kind_t
lh_os_fs_kind_from_win_attrs(lh_os_fs_win_attrs_t attrs, lh_bool_t is_symlink)
{
    if (is_symlink)
    {
        return lh_os_fs_kind_symlink;
    }
    if ((attrs & LH_OS_FS_WIN_ATTR_DIRECTORY) != 0U)
    {
        return lh_os_fs_kind_dir;
    }
    return lh_os_fs_kind_file;
}

static void
lh_os_fs_stat_fill_from_win_attrs(lh_os_fs_stat_t *out, lh_os_fs_win_attrs_t attrs, lh_u32_t size_high,
                                  lh_u32_t size_low, lh_u64_t atime_ticks, lh_u64_t mtime_ticks,
                                  lh_u64_t ctime_ticks, lh_bool_t is_symlink, lh_os_fs_attr_t extra)
{
    lh_os_fs_stat_fill(out, lh_os_fs_kind_from_win_attrs(attrs, is_symlink),
                       lh_os_fs_perm_from_win_attrs(attrs),
                       lh_os_fs_size_from_win_parts(size_high, size_low),
                       lh_os_fs_time_from_filetime_ticks(atime_ticks),
                       lh_os_fs_time_from_filetime_ticks(mtime_ticks),
                       lh_os_fs_time_from_filetime_ticks(ctime_ticks),
                       lh_bit_or(lh_os_fs_attr_from_win_attrs(attrs), extra));
}

/*
 * Pure decoder for POSIX st_mode's S_IFMT bit format. Deliberately built on
 * a portable lh_u32_t, not mode_t, and on our own mirrors of the S_IF*
 * numeric values — not <sys/stat.h>'s. These values are not POSIX-mandated
 * numbers, but every mainstream Unix (Linux, BSD, macOS, Solaris) has used
 * them unchanged since traditional Unix, so they're safe to hardcode; no
 * <sys/stat.h>, no #if, compiles/is callable on every OS lh targets.
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

static lh_os_fs_kind_t
lh_os_fs_kind_from_unix_mode(lh_os_fs_unix_mode_t mode)
{
    lh_u32_t fmt;

    fmt = lh_bit_and(mode, LH_OS_FS_UNIX_S_IFMT);
    if (fmt == LH_OS_FS_UNIX_S_IFLNK)
    {
        return lh_os_fs_kind_symlink;
    }
    if (fmt == LH_OS_FS_UNIX_S_IFDIR)
    {
        return lh_os_fs_kind_dir;
    }
    if (fmt == LH_OS_FS_UNIX_S_IFREG)
    {
        return lh_os_fs_kind_file;
    }
    return lh_os_fs_kind_other;
}

static lh_bool_t
lh_os_fs_stat_fill_from_unix_fields(lh_os_fs_stat_t *out, lh_os_fs_unix_mode_t mode_bits, lh_s64_t size,
                                    lh_os_fs_time_t atime, lh_os_fs_time_t mtime,
                                    lh_os_fs_time_t ctime, lh_os_fs_attr_t extra)
{
    if (size < 0)
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

lh_bool_t
lh_os_fs_stat(const lh_os_fs_path_t *path, lh_os_fs_stat_t *out)
{
    lh_str_t buf;
    lh_str_cptr cstr;
    lh_bool_t ok;

    lh_assert_runtime_ref(out);
    if (lh_os_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }

    cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));

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
        if ((info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U)
        {
            WIN32_FIND_DATAA data;
            HANDLE find;

            find = FindFirstFileA(cstr, lh_addr_of(data));
            if (find == INVALID_HANDLE_VALUE)
            {
                lh_os_system_error_capture();
                lh_str_deinit(lh_addr_of(buf));
                return lh_bool_false;
            }
            FindClose(find);
            is_symlink = (data.dwReserved0 == IO_REPARSE_TAG_SYMLINK) ? lh_bool_true : lh_bool_false;
        }
        /* Native FILETIME -> a plain u64 tick count; the actual math lives
           in the portable lh_os_fs_time_from_filetime_ticks above. */
        lh_os_fs_stat_fill_from_win_attrs(
            out, lh_cast_static(lh_os_fs_win_attrs_t, info.dwFileAttributes),
            lh_cast_static(lh_u32_t, info.nFileSizeHigh), lh_cast_static(lh_u32_t, info.nFileSizeLow),
            lh_bit_or(lh_bit_shl(lh_cast_static(lh_u64_t, info.ftLastAccessTime.dwHighDateTime), 32),
                     lh_cast_static(lh_u64_t, info.ftLastAccessTime.dwLowDateTime)),
            lh_bit_or(lh_bit_shl(lh_cast_static(lh_u64_t, info.ftLastWriteTime.dwHighDateTime), 32),
                     lh_cast_static(lh_u64_t, info.ftLastWriteTime.dwLowDateTime)),
            lh_bit_or(lh_bit_shl(lh_cast_static(lh_u64_t, info.ftCreationTime.dwHighDateTime), 32),
                     lh_cast_static(lh_u64_t, info.ftCreationTime.dwLowDateTime)),
            is_symlink, lh_os_fs_attr_hidden_from_path(path));
        ok = lh_bool_true;
    }
#else
    {
        struct stat info;

        if (lstat(cstr, lh_addr_of(info)) != 0)
        {
            lh_os_system_error_capture();
            lh_str_deinit(lh_addr_of(buf));
            return lh_bool_false;
        }
        ok = lh_os_fs_stat_fill_from_unix_fields(
            out, lh_cast_static(lh_os_fs_unix_mode_t, info.st_mode),
            lh_cast_static(lh_s64_t, info.st_size), lh_cast_static(lh_os_fs_time_t, info.st_atime),
            lh_cast_static(lh_os_fs_time_t, info.st_mtime),
            lh_cast_static(lh_os_fs_time_t, info.st_ctime), lh_os_fs_attr_hidden_from_path(path));
    }
#endif

    lh_str_deinit(lh_addr_of(buf));
    return ok;
}

lh_bool_t
lh_os_fs_mtime(const lh_os_fs_path_t *path, lh_os_fs_time_t *out)
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
