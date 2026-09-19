#include <lh/os/fs/path.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
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

lh_bool_t
lh_os_fs_path_exe_dir(lh_str_ptr out, lh_usize_t out_size)
{
    const lh_char_t *slash;
    lh_usize_t len;

    if (!lh_os_fs_path_exe(out, out_size))
    {
        return lh_bool_false;
    }

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
