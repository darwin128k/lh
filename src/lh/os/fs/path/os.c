#include <lh/os/fs/path.h>
#include "local.h"
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/os/fs/file.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

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

static lh_bool_t
lh_os_fs_path_from_os_buf(lh_os_fs_path_t *out, lh_str_cptr buf)
{
    return lh_os_fs_path_set(out, lh_str_view_make(buf));
}

lh_bool_t
lh_os_fs_path_exe(lh_os_fs_path_t *out)
{
    lh_char_t buf[LH_OS_FS_PATH_MAX];

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD cap;
        DWORD n;

        cap = LH_OS_FS_PATH_MAX > 0xFFFFFFFFU ? 0xFFFFFFFFU
                                              : lh_cast_static(DWORD, LH_OS_FS_PATH_MAX);
        n = GetModuleFileNameA(lh_null, buf, cap);
        if (n == 0)
        {
            lh_os_fs_path_clear(out);
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (n >= cap)
        {
            lh_os_fs_path_clear(out);
            lh_os_set_last_error(lh_os_error_code_too_small,
                                 lh_os_error_desc_lit("path buffer is too small"));
            return lh_bool_false;
        }
        return lh_os_fs_path_from_os_buf(out, buf);
    }
#elif LH_COMPILER_OS == LH_COMPILER_OS_MAC
    {
        uint32_t cap = lh_cast_static(uint32_t, LH_OS_FS_PATH_MAX);

        if (_NSGetExecutablePath(buf, lh_addr_of(cap)) != 0)
        {
            lh_os_fs_path_clear(out);
            lh_os_set_last_error(lh_os_error_code_too_small,
                                 lh_os_error_desc_lit("path buffer is too small"));
            return lh_bool_false;
        }
        return lh_os_fs_path_from_os_buf(out, buf);
    }
#else
    {
        lh_ssize_t n;

        n = lh_cast_static(lh_ssize_t, readlink("/proc/self/exe", buf, LH_OS_FS_PATH_MAX - 1U));
        if (n <= 0)
        {
            lh_os_fs_path_clear(out);
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        lh_ptr_deref(lh_ptr_add_by_offset(lh_char_t, buf, lh_cast_static(lh_usize_t, n))) = '\0';
        return lh_os_fs_path_from_os_buf(out, buf);
    }
#endif
}

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static lh_bool_t
lh_os_fs_path_attrs(lh_str_cptr cstr, DWORD *attrs)
{
    WIN32_FILE_ATTRIBUTE_DATA info;

    if (!GetFileAttributesExA(cstr, GetFileExInfoStandard, lh_addr_of(info)))
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
    *attrs = info.dwFileAttributes;
    return lh_bool_true;
}
#endif

lh_bool_t
lh_os_fs_path_is(const lh_os_fs_path_t *path, lh_os_fs_kind_t kind)
{
    lh_str_cptr cstr;

    if (kind == lh_os_fs_kind_other)
    {
        return lh_bool_false;
    }
    if (kind != lh_os_fs_kind_file && kind != lh_os_fs_kind_dir &&
        kind != lh_os_fs_kind_symlink && kind != lh_os_fs_kind_shortcut)
    {
        lh_os_set_last_error(lh_os_error_code_invalid_kind, lh_os_error_desc_lit("kind is invalid"));
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

    cstr = lh_os_fs_path_cstr(path);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD attrs;

        if (!lh_os_fs_path_attrs(cstr, lh_addr_of(attrs)))
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

            find = FindFirstFileA(cstr, lh_addr_of(data));
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
            if (lstat(cstr, lh_addr_of(info)) != 0)
            {
                lh_os_capture_last_error();
                return lh_bool_false;
            }
            return lh_cast_static(lh_bool_t, S_ISLNK(info.st_mode));
        }
        if (stat(cstr, lh_addr_of(info)) != 0)
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
lh_os_fs_path_is_dir(const lh_os_fs_path_t *path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_dir);
}

lh_bool_t
lh_os_fs_path_is_file(const lh_os_fs_path_t *path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_file);
}

lh_bool_t
lh_os_fs_path_is_symlink(const lh_os_fs_path_t *path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_symlink);
}

lh_bool_t
lh_os_fs_path_is_shortcut(const lh_os_fs_path_t *path)
{
    return lh_os_fs_path_is(path, lh_os_fs_kind_shortcut);
}

lh_bool_t
lh_os_fs_path_mtime(const lh_os_fs_path_t *path, lh_s64_t *out)
{
    lh_str_cptr cstr;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_path_require(path))
    {
        return lh_bool_false;
    }
    cstr = lh_os_fs_path_cstr(path);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        WIN32_FILE_ATTRIBUTE_DATA info;
        ULARGE_INTEGER ticks;

        if (!GetFileAttributesExA(cstr, GetFileExInfoStandard, lh_addr_of(info)))
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

        if (stat(cstr, lh_addr_of(info)) != 0)
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
lh_os_fs_path_read(const lh_os_fs_path_t *path, lh_ptr buf, lh_usize_t buf_size, lh_usize_t *out_size)
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
        lh_os_set_last_error(lh_os_error_code_too_small,
                             lh_os_error_desc_lit("path buffer is too small"));
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
lh_os_fs_path_remove(const lh_os_fs_path_t *path)
{
    lh_str_cptr cstr;

    if (!lh_os_fs_path_require(path))
    {
        return lh_bool_false;
    }
    cstr = lh_os_fs_path_cstr(path);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    if (!DeleteFileA(cstr))
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
#else
    if (unlink(cstr) != 0)
    {
        lh_os_capture_last_error();
        return lh_bool_false;
    }
#endif
    return lh_bool_true;
}
