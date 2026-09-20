#include <lh/os/fs/stat.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/char/dot.h>
#include <lh/compiler/os.h>
#include <lh/os.h>
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
    *self = *other;
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

lh_u64_t
lh_os_fs_stat_get_size(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_s64_t
lh_os_fs_stat_get_atime(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->atime;
}

lh_s64_t
lh_os_fs_stat_get_mtime(const lh_os_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mtime;
}

lh_s64_t
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
lh_os_fs_stat_fill(lh_os_fs_stat_t *self, lh_os_fs_kind_t kind, lh_os_fs_perm_t perm, lh_u64_t size,
                   lh_s64_t atime, lh_s64_t mtime, lh_s64_t ctime, lh_os_fs_attr_t attr)
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
    lh_str_view_t last;
    lh_usize_t n;
    lh_usize_t size;

    n = lh_os_fs_path_get_part_count(path);
    if (n == 0U)
    {
        return 0U;
    }
    last = lh_os_fs_path_get_part(path, n - 1U);
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

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static lh_s64_t
lh_os_fs_filetime_to_unix(const FILETIME *ft)
{
    ULARGE_INTEGER ticks;

    ticks.LowPart = ft->dwLowDateTime;
    ticks.HighPart = ft->dwHighDateTime;
    if (ticks.QuadPart < LH_OS_FS_FILETIME_UNIX_EPOCH)
    {
        return 0;
    }
    return lh_cast_static(lh_s64_t,
                          (ticks.QuadPart - LH_OS_FS_FILETIME_UNIX_EPOCH) / LH_OS_FS_FILETIME_HZ);
}

static lh_u64_t
lh_os_fs_size_from_win(DWORD high, DWORD low)
{
    return lh_bit_or(lh_bit_shl(lh_cast_static(lh_u64_t, high), 32), lh_cast_static(lh_u64_t, low));
}

static lh_os_fs_perm_t
lh_os_fs_perm_from_win(DWORD attrs)
{
    lh_os_fs_perm_t perm;

    perm = lh_os_fs_perm_irusr | lh_os_fs_perm_irgrp | lh_os_fs_perm_iroth;
    if ((attrs & FILE_ATTRIBUTE_READONLY) == 0U)
    {
        perm = lh_bit_or(perm, lh_os_fs_perm_iwusr | lh_os_fs_perm_iwgrp | lh_os_fs_perm_iwoth);
    }
    if ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0U)
    {
        perm = lh_bit_or(perm, lh_os_fs_perm_ixusr | lh_os_fs_perm_ixgrp | lh_os_fs_perm_ixoth);
    }
    return perm;
}

static lh_os_fs_attr_t
lh_os_fs_attr_from_win(DWORD attrs)
{
    lh_os_fs_attr_t attr;

    attr = 0U;
    if ((attrs & FILE_ATTRIBUTE_HIDDEN) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_hidden);
    }
    if ((attrs & FILE_ATTRIBUTE_SYSTEM) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_system);
    }
    if ((attrs & FILE_ATTRIBUTE_ARCHIVE) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_archive);
    }
    if ((attrs & FILE_ATTRIBUTE_COMPRESSED) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_compressed);
    }
    if ((attrs & FILE_ATTRIBUTE_ENCRYPTED) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_encrypted);
    }
    if ((attrs & FILE_ATTRIBUTE_TEMPORARY) != 0U)
    {
        attr = lh_bit_or(attr, lh_os_fs_attr_temporary);
    }
    return attr;
}

static lh_os_fs_kind_t
lh_os_fs_kind_from_win(DWORD attrs, lh_bool_t is_symlink)
{
    if (is_symlink)
    {
        return lh_os_fs_kind_symlink;
    }
    if ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0U)
    {
        return lh_os_fs_kind_dir;
    }
    return lh_os_fs_kind_file;
}

static void
lh_os_fs_stat_fill_from_win(lh_os_fs_stat_t *out, DWORD attrs, DWORD size_high, DWORD size_low,
                            const FILETIME *atime, const FILETIME *mtime, const FILETIME *ctime,
                            lh_bool_t is_symlink, lh_os_fs_attr_t extra)
{
    lh_os_fs_stat_fill(out, lh_os_fs_kind_from_win(attrs, is_symlink), lh_os_fs_perm_from_win(attrs),
                       lh_os_fs_size_from_win(size_high, size_low),
                       lh_os_fs_filetime_to_unix(atime), lh_os_fs_filetime_to_unix(mtime),
                       lh_os_fs_filetime_to_unix(ctime),
                       lh_bit_or(lh_os_fs_attr_from_win(attrs), extra));
}
#else
static lh_os_fs_kind_t
lh_os_fs_kind_from_unix(mode_t mode)
{
    if (S_ISLNK(mode))
    {
        return lh_os_fs_kind_symlink;
    }
    if (S_ISDIR(mode))
    {
        return lh_os_fs_kind_dir;
    }
    if (S_ISREG(mode))
    {
        return lh_os_fs_kind_file;
    }
    return lh_os_fs_kind_other;
}

static lh_bool_t
lh_os_fs_stat_fill_from_unix(lh_os_fs_stat_t *out, const struct stat *info, lh_os_fs_attr_t extra)
{
    if (info->st_size < 0)
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_negative_size,
                             lh_os_error_desc_lit("file size is negative")));
        return lh_bool_false;
    }
    lh_os_fs_stat_fill(out, lh_os_fs_kind_from_unix(info->st_mode),
                       lh_cast_static(lh_os_fs_perm_t, lh_bit_and(info->st_mode, LH_OS_FS_PERM_UNIX_MASK)),
                       lh_cast_static(lh_u64_t, info->st_size),
                       lh_cast_static(lh_s64_t, info->st_atime),
                       lh_cast_static(lh_s64_t, info->st_mtime),
                       lh_cast_static(lh_s64_t, info->st_ctime), extra);
    return lh_bool_true;
}
#endif

lh_bool_t
lh_os_fs_stat(const lh_os_fs_path_t *path, lh_os_fs_stat_t *out)
{
    lh_str_cptr cstr;

    lh_assert_runtime_ref(out);
    if (lh_os_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    cstr = lh_str_get_data(lh_os_fs_path_get_text_as_const(path));

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        WIN32_FILE_ATTRIBUTE_DATA info;
        lh_bool_t is_symlink;

        if (!GetFileAttributesExA(cstr, GetFileExInfoStandard, lh_addr_of(info)))
        {
            lh_os_system_error_capture();
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
                return lh_bool_false;
            }
            FindClose(find);
            is_symlink = lh_cast_static(lh_bool_t, data.dwReserved0 == IO_REPARSE_TAG_SYMLINK);
        }
        lh_os_fs_stat_fill_from_win(out, info.dwFileAttributes, info.nFileSizeHigh,
                                    info.nFileSizeLow, lh_addr_of(info.ftLastAccessTime),
                                    lh_addr_of(info.ftLastWriteTime),
                                    lh_addr_of(info.ftCreationTime), is_symlink,
                                    lh_os_fs_attr_hidden_from_path(path));
        return lh_bool_true;
    }
#else
    {
        struct stat info;

        if (lstat(cstr, lh_addr_of(info)) != 0)
        {
            lh_os_system_error_capture();
            return lh_bool_false;
        }
        return lh_os_fs_stat_fill_from_unix(out, lh_addr_of(info),
                                            lh_os_fs_attr_hidden_from_path(path));
    }
#endif
}

lh_bool_t
lh_os_fs_mtime(const lh_os_fs_path_t *path, lh_s64_t *out)
{
    lh_os_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    *out = lh_os_fs_stat_get_mtime(lh_addr_of(st));
    return lh_bool_true;
}
