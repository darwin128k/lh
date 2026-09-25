#include <lh/os/system/fs/file.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/os/system/error/capture.h>
#include <lh/runtime/error.h>
#include <lh/util/math.h>

#include <fcntl.h>
#include <unistd.h>

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
    int native;

    lh_assert_runtime_ref(path);
    lh_assert_runtime_if(!lh_fs_file_mode_is_readable(mode) &&
                             !lh_fs_file_mode_is_writable(mode),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    native = lh_math_eq(mode, lh_fs_file_mode_read)
                 ? open(path, lh_os_system_fs_file_flags(mode))
                 : open(path, lh_os_system_fs_file_flags(mode), 0644);
    if (lh_math_is_negative(native))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    }
    return lh_cast_static(lh_os_system_fs_file_handle_t, native);
}

void
lh_os_system_fs_file_close(lh_os_system_fs_file_handle_t handle)
{
    (void)close(lh_os_system_fs_file_native(handle));
}

lh_ssize_t
lh_os_system_fs_file_read(lh_os_system_fs_file_handle_t handle, lh_ptr buf, lh_usize_t size)
{
    lh_ssize_t result;

    result = lh_cast_static(lh_ssize_t, read(lh_os_system_fs_file_native(handle), buf, lh_cast_static(size_t, size)));
    if (lh_math_is_negative(result))
    {
        lh_os_system_error_capture();
    }
    return result;
}

lh_ssize_t
lh_os_system_fs_file_write(lh_os_system_fs_file_handle_t handle, const lh_ptr buf, lh_usize_t size)
{
    lh_ssize_t result;

    result = lh_cast_static(lh_ssize_t, write(lh_os_system_fs_file_native(handle), buf, lh_cast_static(size_t, size)));
    if (lh_math_is_negative(result))
    {
        lh_os_system_error_capture();
    }
    return result;
}
