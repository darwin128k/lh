#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/error/capture.h>
#include <lh/runtime/error.h>
#include <lh/str.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <fcntl.h>
#    include <unistd.h>
#endif

lh_fs_path_t *
lh_os_fs_file_get_path(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

const lh_fs_path_t *
lh_os_fs_file_get_path_as_const(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

lh_os_fs_file_handle_t *
lh_os_fs_file_get_handle(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

const lh_os_fs_file_handle_t *
lh_os_fs_file_get_handle_as_const(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

lh_bool_t
lh_os_fs_file_is_valid(const lh_os_fs_file_t *self)
{
    return lh_cast_static(
        lh_bool_t, lh_math_ne(lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self)), LH_OS_FS_FILE_HANDLE_INVALID));
}

lh_os_fs_file_mode_t
lh_os_fs_file_get_mode(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

void
lh_os_fs_file_set_mode(lh_os_fs_file_t *self, lh_os_fs_file_mode_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = mode;
}

void
lh_os_fs_file_set_path(lh_os_fs_file_t *self, const lh_fs_path_t *path)
{
    lh_fs_path_assign(lh_os_fs_file_get_path(self), path);
}

void
lh_os_fs_file_init(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    lh_fs_path_init(lh_os_fs_file_get_path(self));
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = LH_OS_FS_FILE_HANDLE_INVALID;
    lh_os_fs_file_set_mode(self, lh_os_fs_file_mode_none);
}

void
lh_os_fs_file_close(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    if (!lh_os_fs_file_is_valid(self))
    {
        lh_os_fs_file_set_mode(self, lh_os_fs_file_mode_none);
        return;
    }
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    (void)CloseHandle(
        lh_cast_reinterpret(HANDLE, lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self))));
#else
    (void)close(lh_cast_static(int, lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self))));
#endif
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = LH_OS_FS_FILE_HANDLE_INVALID;
    lh_os_fs_file_set_mode(self, lh_os_fs_file_mode_none);
}

void
lh_os_fs_file_deinit(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_fs_file_close(self);
    lh_fs_path_deinit(lh_os_fs_file_get_path(self));
}

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static DWORD
lh_os_fs_file_win_access(lh_os_fs_file_mode_t mode)
{
    if (lh_math_eq(mode, lh_os_fs_file_mode_read))
    {
        return GENERIC_READ;
    }
    if (lh_math_eq(mode, lh_os_fs_file_mode_write))
    {
        return GENERIC_WRITE;
    }
    return GENERIC_READ | GENERIC_WRITE;
}

static DWORD
lh_os_fs_file_win_disposition(lh_os_fs_file_mode_t mode)
{
    if (lh_math_eq(mode, lh_os_fs_file_mode_read))
    {
        return OPEN_EXISTING;
    }
    if (lh_math_eq(mode, lh_os_fs_file_mode_write))
    {
        return CREATE_ALWAYS;
    }
    return OPEN_ALWAYS;
}

static lh_bool_t
lh_os_fs_file_open_native(lh_os_fs_file_t *self, lh_str_cptr cstr, lh_os_fs_file_mode_t mode)
{
    HANDLE native;

    native = CreateFileA(cstr, lh_os_fs_file_win_access(mode), FILE_SHARE_READ, lh_null,
                         lh_os_fs_file_win_disposition(mode), FILE_ATTRIBUTE_NORMAL, lh_null);
    if (lh_math_eq(native, INVALID_HANDLE_VALUE))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) =
        lh_cast_reinterpret(lh_os_fs_file_handle_t, native);
    return lh_bool_true;
}
#else
static int
lh_os_fs_file_posix_flags(lh_os_fs_file_mode_t mode)
{
    if (lh_math_eq(mode, lh_os_fs_file_mode_read))
    {
        return O_RDONLY;
    }
    if (lh_math_eq(mode, lh_os_fs_file_mode_write))
    {
        return O_WRONLY | O_CREAT | O_TRUNC;
    }
    return O_RDWR | O_CREAT;
}

static lh_bool_t
lh_os_fs_file_open_native(lh_os_fs_file_t *self, lh_str_cptr cstr, lh_os_fs_file_mode_t mode)
{
    int native;

    native = lh_math_eq(mode, lh_os_fs_file_mode_read)
                 ? open(cstr, lh_os_fs_file_posix_flags(mode))
                 : open(cstr, lh_os_fs_file_posix_flags(mode), 0644);
    if (lh_math_lt(native, 0))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = lh_cast_static(lh_os_fs_file_handle_t, native);
    return lh_bool_true;
}
#endif

lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, const lh_fs_path_t *path, lh_os_fs_file_mode_t mode)
{
    lh_str_t buf;
    lh_str_cptr cstr;
    lh_bool_t ok;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(!lh_os_fs_file_mode_is_readable(mode) && !lh_os_fs_file_mode_is_writable(mode),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (lh_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    lh_os_fs_file_close(self);

    cstr = lh_fs_path_to_cstr(path, lh_addr_of(buf));
    ok = lh_os_fs_file_open_native(self, cstr, mode);
    lh_str_deinit(lh_addr_of(buf));
    if (!ok)
    {
        lh_os_fs_file_set_mode(self, lh_os_fs_file_mode_none);
        return lh_bool_false;
    }
    lh_fs_path_assign(lh_os_fs_file_get_path(self), path);
    lh_os_fs_file_set_mode(self, mode);
    return lh_bool_true;
}
