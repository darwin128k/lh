#include <lh/os/system/fs/file.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/os/system/error/capture.h>
#include <lh/math.h>

#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/util/str/ptr.h>

#ifndef NAME_MAX
#    define NAME_MAX 255
#endif

LH_ATTRIBUTE_STATIC
int
lh_os_system_fs_file_native(lh_os_system_fs_file_handle_t handle)
{
    return lh_cast_static(int, handle);
}

LH_ATTRIBUTE_STATIC
int
lh_os_system_fs_file_flags(lh_fs_file_mode_t mode)
{
    if (lh_math_eq(mode, lh_fs_file_mode_read))
    {
        return O_RDONLY;
    }
    if (lh_math_eq(mode, lh_fs_file_mode_write))
    {
        return O_WRONLY | O_CREAT | O_TRUNC;
    }
    return O_RDWR | O_CREAT;
}

lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open(lh_str_cptr path, lh_fs_file_mode_t mode)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_if(!lh_fs_file_mode_is_readable(mode) && !lh_fs_file_mode_is_writable(mode),
                         lh_runtime_error_code_invalid_argument);

    const int native = lh_math_eq(mode, lh_fs_file_mode_read)
                           ? open(path, lh_os_system_fs_file_flags(mode))
                           : open(path, lh_os_system_fs_file_flags(mode), 0644);
    if (lh_math_is_negative(native))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    }
    return lh_cast_static(lh_os_system_fs_file_handle_t, native);
}

lh_void
lh_os_system_fs_file_close(lh_os_system_fs_file_handle_t handle)
{
    (void)close(lh_os_system_fs_file_native(handle));
}

lh_ssize_t
lh_os_system_fs_file_read(lh_os_system_fs_file_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    const lh_ssize_t result = lh_cast_static(
        lh_ssize_t, read(lh_os_system_fs_file_native(handle), buf, lh_cast_static(size_t, size)));
    if (lh_math_is_negative(result))
    {
        lh_os_system_error_capture();
    }
    return result;
}

lh_ssize_t
lh_os_system_fs_file_write(lh_os_system_fs_file_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    const lh_ssize_t result = lh_cast_static(
        lh_ssize_t, write(lh_os_system_fs_file_native(handle), buf, lh_cast_static(size_t, size)));
    if (lh_math_is_negative(result))
    {
        lh_os_system_error_capture();
    }
    return result;
}

lh_os_system_fs_file_handle_t
lh_os_system_fs_file_open_read_at(lh_os_system_fs_file_handle_t root, lh_str_cptr path,
                                  lh_fs_size_t *size)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_ref(size);
    if (lh_math_is_negative(root))
    {
        lh_os_set_last_error_lit(lh_os_error_code_invalid_argument, "expected an open directory");
        return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    }
    /* Borrow root until the first component has been opened. Every subsequent
     * directory is owned here, including on error paths. */
    int directory = lh_os_system_fs_file_native(root);
    lh_bool_t owned = lh_bool_false;
    lh_str_cptr part = path;
    for (;;)
    {
        lh_char_t name[NAME_MAX + 1];
        lh_usize_t length = 0;
        while (part[length] && part[length] != '/')
        {
            if (part[length] == '\\' || part[length] == ':' || length == NAME_MAX)
            {
                break;
            }
            ++length;
        }
        if (!length || (part[length] && part[length] != '/') || (length == 1U && part[0] == '.') ||
            (length == 2U && part[0] == '.' && part[1] == '.'))
        {
            lh_os_set_last_error_lit(lh_os_error_code_invalid_argument,
                                     "expected a relative file path without dot components");
            if (owned)
            {
                (void)close(directory);
            }
            return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
        }
        lh_str_ptr_copy(name, sizeof name, part, length);
        name[length] = '\0';
        const lh_bool_t more = lh_cast_static(lh_bool_t, part[length] == '/');
        const int next =
            openat(directory, name,
                   O_RDONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK | (more ? O_DIRECTORY : 0));
        if (next < 0)
        {
            lh_os_system_error_capture();
        }
        if (owned)
        {
            (void)close(directory);
        }
        if (next < 0)
        {
            return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
        }
        if (!more)
        {
            struct stat info;
            if (fstat(next, lh_addr_of(info)))
            {
                lh_os_system_error_capture();
                (void)close(next);
                return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
            }
            if (!S_ISREG(info.st_mode) || info.st_size < 0)
            {
                lh_os_set_last_error_lit(lh_os_error_code_invalid_argument,
                                         "expected a regular file with nonnegative size");
                (void)close(next);
                return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
            }
            lh_ptr_deref(size) = lh_cast_static(lh_fs_size_t, info.st_size);
            return lh_cast_static(lh_os_system_fs_file_handle_t, next);
        }
        directory = next;
        owned = lh_bool_true;
        part += length + 1U;
    }
}
