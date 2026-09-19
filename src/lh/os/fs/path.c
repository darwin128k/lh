#include <lh/os/fs/path.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/fs/file.h>
#include <lh/memory.h>
#include <lh/util/addr.h>
#include <lh/util/str/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#elif LH_COMPILER_OS == LH_COMPILER_OS_MAC
#    include <mach-o/dyld.h>
#    include <sys/stat.h>
#    include <unistd.h>
#else
#    include <sys/stat.h>
#    include <unistd.h>
#endif

#define LH_OS_FS_FILETIME_UNIX_EPOCH 116444736000000000ULL
#define LH_OS_FS_FILETIME_HZ 10000000ULL

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    ifndef IO_REPARSE_TAG_SYMLINK
#        define IO_REPARSE_TAG_SYMLINK 0xA000000CUL
#    endif
#endif

static const lh_u8_t lh_os_fs_path_shortcut_magic[20] = {
    0x4CU, 0x00U, 0x00U, 0x00U, 0x01U, 0x14U, 0x02U, 0x00U, 0x00U, 0x00U,
    0x00U, 0x00U, 0xC0U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x46U};

static void
lh_os_fs_path_fail_null(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("path is null"));
}

static void
lh_os_fs_path_fail_empty(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("path is empty"));
}

static void
lh_os_fs_path_fail_too_small(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("path buffer is too small"));
}

static void
lh_os_fs_path_fail_kind(void)
{
    lh_os_set_last_error(1, lh_os_error_desc_lit("kind is invalid"));
}

static lh_bool_t
lh_os_fs_path_is_sep(lh_char_t ch)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return lh_cast_static(lh_bool_t, ch == '\\' || ch == '/');
#else
    return lh_cast_static(lh_bool_t, ch == '/');
#endif
}

static const lh_char_t *
lh_os_fs_path_last_sep(lh_str_cptr path, lh_usize_t len)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    static const lh_char_t seps[] = "\\/";

    return lh_str_ptr_rfind_of_chars(lh_cast_const(lh_str_ptr, path), len,
                                     lh_cast_const(lh_str_ptr, seps), 2U);
#else
    return lh_str_ptr_rfind_of_char(lh_cast_const(lh_str_ptr, path), len, '/');
#endif
}

lh_char_t
lh_os_fs_path_sep(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return '\\';
#else
    return '/';
#endif
}

lh_bool_t
lh_os_fs_path_exe(lh_str_ptr out, lh_usize_t out_size)
{
    lh_assert_runtime_ref(out);

    if (out_size < 2U)
    {
        if (out_size > 0U)
        {
            out[0] = '\0';
        }
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD cap;
        DWORD n;

        cap = out_size > 0xFFFFFFFFU ? 0xFFFFFFFFU : lh_cast_static(DWORD, out_size);
        n = GetModuleFileNameA(lh_null, out, cap);
        if (n == 0)
        {
            out[0] = '\0';
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (n >= cap)
        {
            out[0] = '\0';
            lh_os_fs_path_fail_too_small();
            return lh_bool_false;
        }
        return lh_bool_true;
    }
#elif LH_COMPILER_OS == LH_COMPILER_OS_MAC
    {
        uint32_t cap = lh_cast_static(uint32_t, out_size);

        if (_NSGetExecutablePath(out, lh_addr_of(cap)) != 0)
        {
            out[0] = '\0';
            lh_os_fs_path_fail_too_small();
            return lh_bool_false;
        }
        return lh_bool_true;
    }
#else
    {
        lh_ssize_t n;

        n = lh_cast_static(lh_ssize_t, readlink("/proc/self/exe", out, out_size - 1U));
        if (n <= 0)
        {
            out[0] = '\0';
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (lh_cast_static(lh_usize_t, n) >= out_size - 1U)
        {
            out[0] = '\0';
            lh_os_fs_path_fail_too_small();
            return lh_bool_false;
        }
        out[n] = '\0';
        return lh_bool_true;
    }
#endif
}

static lh_bool_t
lh_os_fs_path_drop_last(lh_str_ptr out, lh_usize_t out_size)
{
    const lh_char_t *slash;
    lh_usize_t len;

    len = lh_str_ptr_len(out);
    slash = lh_os_fs_path_last_sep(out, len);
    if (lh_null_eq(slash))
    {
        if (out_size < 2U)
        {
            out[0] = '\0';
            lh_os_fs_path_fail_too_small();
            return lh_bool_false;
        }
        out[0] = '.';
        out[1] = '\0';
        return lh_bool_true;
    }

    if (slash == out)
    {
        out[1] = '\0';
        return lh_bool_true;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if ((slash == out + 2) && (out[1] == ':'))
    {
        out[3] = '\0';
        return lh_bool_true;
    }
#endif

    {
        lh_str_ptr cut = lh_cast_const(lh_str_ptr, slash);
        cut[0] = '\0';
    }
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_path_exe_dir(lh_str_ptr out, lh_usize_t out_size)
{
    if (!lh_os_fs_path_exe(out, out_size))
    {
        return lh_bool_false;
    }
    return lh_os_fs_path_drop_last(out, out_size);
}

lh_bool_t
lh_os_fs_path_dir(lh_str_cptr path, lh_str_ptr out, lh_usize_t out_size)
{
    lh_usize_t len;

    lh_assert_runtime_ref(out);

    if (lh_null_eq(path))
    {
        if (out_size > 0U)
        {
            out[0] = '\0';
        }
        lh_os_fs_path_fail_null();
        return lh_bool_false;
    }
    if (path[0] == '\0')
    {
        if (out_size > 0U)
        {
            out[0] = '\0';
        }
        lh_os_fs_path_fail_empty();
        return lh_bool_false;
    }
    if (out_size < 2U)
    {
        if (out_size > 0U)
        {
            out[0] = '\0';
        }
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }

    len = lh_str_ptr_len(path);
    if (len + 1U > out_size)
    {
        out[0] = '\0';
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }
    lh_str_ptr_copy(out, len, path, len);
    out[len] = '\0';
    return lh_os_fs_path_drop_last(out, out_size);
}

static lh_bool_t
lh_os_fs_path_require(lh_str_cptr path)
{
    if (lh_null_eq(path))
    {
        lh_os_fs_path_fail_null();
        return lh_bool_false;
    }
    if (path[0] == '\0')
    {
        lh_os_fs_path_fail_empty();
        return lh_bool_false;
    }
    return lh_bool_true;
}

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static lh_bool_t
lh_os_fs_path_attrs(lh_str_cptr path, DWORD *attrs)
{
    WIN32_FILE_ATTRIBUTE_DATA info;

    if (!GetFileAttributesExA(path, GetFileExInfoStandard, lh_addr_of(info)))
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
    *attrs = info.dwFileAttributes;
    return lh_bool_true;
}
#endif

lh_bool_t
lh_os_fs_path_is(lh_str_cptr path, lh_os_fs_kind_t kind)
{
    if (kind == lh_os_fs_kind_other)
    {
        return lh_bool_false;
    }
    if (kind != lh_os_fs_kind_file && kind != lh_os_fs_kind_dir &&
        kind != lh_os_fs_kind_symlink && kind != lh_os_fs_kind_shortcut)
    {
        lh_os_fs_path_fail_kind();
        return lh_bool_false;
    }
    if (!lh_os_fs_path_require(path))
    {
        return lh_bool_false;
    }

    if (kind == lh_os_fs_kind_shortcut)
    {
        lh_os_fs_file_t file;
        lh_u8_t buf[20];
        lh_ssize_t n;

        if (!lh_os_fs_path_is(path, lh_os_fs_kind_file))
        {
            return lh_bool_false;
        }
        lh_os_fs_file_init(lh_addr_of(file));
        if (!lh_os_fs_file_open(lh_addr_of(file), path, lh_os_fs_file_mode_read))
        {
            return lh_bool_false;
        }
        n = lh_os_fs_file_read(lh_addr_of(file), buf, sizeof(buf));
        lh_os_fs_file_close(lh_addr_of(file));
        if (n != lh_cast_static(lh_ssize_t, sizeof(buf)))
        {
            return lh_bool_false;
        }
        return lh_memory_compare(buf, sizeof(buf),
                                 lh_cast_const(lh_ptr, lh_os_fs_path_shortcut_magic),
                                 sizeof(lh_os_fs_path_shortcut_magic)) == lh_null
                   ? lh_bool_true
                   : lh_bool_false;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD attrs;

        if (!lh_os_fs_path_attrs(path, lh_addr_of(attrs)))
        {
            return lh_bool_false;
        }
        if (kind == lh_os_fs_kind_dir)
        {
            return lh_cast_static(lh_bool_t, (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0U);
        }
        if (kind == lh_os_fs_kind_file)
        {
            return lh_cast_static(lh_bool_t, (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0U);
        }
        if ((attrs & FILE_ATTRIBUTE_REPARSE_POINT) == 0U)
        {
            return lh_bool_false;
        }
        {
            WIN32_FIND_DATAA data;
            HANDLE find;

            find = FindFirstFileA(path, lh_addr_of(data));
            if (find == INVALID_HANDLE_VALUE)
            {
                lh_os_capture_last_error();
                return lh_bool_false;
            }
            FindClose(find);
            return lh_cast_static(lh_bool_t, data.dwReserved0 == IO_REPARSE_TAG_SYMLINK);
        }
    }
#else
    {
        struct stat info;

        if (kind == lh_os_fs_kind_symlink)
        {
            if (lstat(path, lh_addr_of(info)) != 0)
            {
                lh_os_capture_last_error();
                return lh_bool_false;
            }
            return lh_cast_static(lh_bool_t, S_ISLNK(info.st_mode));
        }
        if (stat(path, lh_addr_of(info)) != 0)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (kind == lh_os_fs_kind_dir)
        {
            return lh_cast_static(lh_bool_t, S_ISDIR(info.st_mode));
        }
        return lh_cast_static(lh_bool_t, S_ISREG(info.st_mode));
    }
#endif
}

lh_bool_t
lh_os_fs_path_is_dir(lh_str_cptr path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_dir);
}

lh_bool_t
lh_os_fs_path_is_file(lh_str_cptr path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_file);
}

lh_bool_t
lh_os_fs_path_is_symlink(lh_str_cptr path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_symlink);
}

lh_bool_t
lh_os_fs_path_is_shortcut(lh_str_cptr path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_shortcut);
}

lh_bool_t
lh_os_fs_path_join(lh_str_ptr out, lh_usize_t out_size, lh_str_cptr dir, lh_str_cptr name)
{
    lh_usize_t dir_len;
    lh_usize_t name_len;
    lh_bool_t need_sep;
    lh_usize_t need;
    lh_str_ptr p;

    lh_assert_runtime_ref(out);

    if (lh_null_eq(dir) || lh_null_eq(name))
    {
        if (out_size > 0U)
        {
            out[0] = '\0';
        }
        lh_os_fs_path_fail_null();
        return lh_bool_false;
    }
    if (name[0] == '\0')
    {
        if (out_size > 0U)
        {
            out[0] = '\0';
        }
        lh_os_fs_path_fail_empty();
        return lh_bool_false;
    }
    if (out_size < 2U)
    {
        if (out_size > 0U)
        {
            out[0] = '\0';
        }
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }

    dir_len = lh_str_ptr_len(dir);
    name_len = lh_str_ptr_len(name);
    need_sep = lh_cast_static(
        lh_bool_t, dir_len > 0U && !lh_os_fs_path_is_sep(dir[dir_len - 1U]) &&
                       !lh_os_fs_path_is_sep(name[0]));

    if (dir_len > out_size || name_len > out_size)
    {
        out[0] = '\0';
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }

    need = dir_len + (need_sep ? 1U : 0U) + name_len + 1U;
    if (need < dir_len || need > out_size)
    {
        out[0] = '\0';
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }

    p = lh_str_ptr_copy(out, dir_len, dir, dir_len);
    if (need_sep)
    {
        p[0] = lh_os_fs_path_sep();
        p = p + 1;
    }
    p = lh_str_ptr_copy(p, name_len, name, name_len);
    p[0] = '\0';
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_path_mtime(lh_str_cptr path, lh_s64_t *out)
{
    lh_assert_runtime_ref(out);

    if (lh_null_eq(path))
    {
        lh_os_fs_path_fail_null();
        return lh_bool_false;
    }
    if (path[0] == '\0')
    {
        lh_os_fs_path_fail_empty();
        return lh_bool_false;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        WIN32_FILE_ATTRIBUTE_DATA info;
        ULARGE_INTEGER ticks;

        if (!GetFileAttributesExA(path, GetFileExInfoStandard, lh_addr_of(info)))
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        ticks.LowPart = info.ftLastWriteTime.dwLowDateTime;
        ticks.HighPart = info.ftLastWriteTime.dwHighDateTime;
        if (ticks.QuadPart < LH_OS_FS_FILETIME_UNIX_EPOCH)
        {
            *out = 0;
            return lh_bool_true;
        }
        *out = lh_cast_static(lh_s64_t,
                              (ticks.QuadPart - LH_OS_FS_FILETIME_UNIX_EPOCH) / LH_OS_FS_FILETIME_HZ);
        return lh_bool_true;
    }
#else
    {
        struct stat info;

        if (stat(path, lh_addr_of(info)) != 0)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        *out = lh_cast_static(lh_s64_t, info.st_mtime);
        return lh_bool_true;
    }
#endif
}

lh_bool_t
lh_os_fs_path_read(lh_str_cptr path, lh_ptr buf, lh_usize_t buf_size, lh_usize_t *out_size)
{
    lh_os_fs_file_t file;
    lh_u64_t size;

    lh_assert_runtime_ref(out_size);

    lh_os_fs_file_init(lh_addr_of(file));
    if (!lh_os_fs_file_open(lh_addr_of(file), path, lh_os_fs_file_mode_read))
    {
        return lh_bool_false;
    }
    if (!lh_os_fs_file_get_size(lh_addr_of(file), lh_addr_of(size)))
    {
        lh_os_fs_file_close(lh_addr_of(file));
        return lh_bool_false;
    }
    if (size > buf_size)
    {
        lh_os_fs_file_close(lh_addr_of(file));
        lh_os_fs_path_fail_too_small();
        return lh_bool_false;
    }
    if (!lh_os_fs_file_read_all(lh_addr_of(file), buf, lh_cast_static(lh_usize_t, size)))
    {
        lh_os_fs_file_close(lh_addr_of(file));
        return lh_bool_false;
    }
    lh_os_fs_file_close(lh_addr_of(file));
    *out_size = lh_cast_static(lh_usize_t, size);
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_path_remove(lh_str_cptr path)
{
    if (lh_null_eq(path))
    {
        lh_os_fs_path_fail_null();
        return lh_bool_false;
    }
    if (path[0] == '\0')
    {
        lh_os_fs_path_fail_empty();
        return lh_bool_false;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (!DeleteFileA(path))
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
#else
    if (unlink(path) != 0)
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
#endif
    return lh_bool_true;
}
