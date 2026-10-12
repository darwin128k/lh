#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/cast/static.h>
#include <lh/fs/path.h>
#include <lh/null.h>
#include <lh/os/fs/path.h>
#include <lh/os/system/fs/file.h>
#include <lh/os/system/fs/path.h>
#include <lh/self.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/math.h>
#include <lh/util/ptr.h>

#include <stdarg.h>

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

lh_os_system_fs_file_handle_t *
lh_os_fs_file_get_handle(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

const lh_os_system_fs_file_handle_t *
lh_os_fs_file_get_handle_as_const(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

lh_bool_t
lh_os_fs_file_is_valid(const lh_os_fs_file_t *self)
{
    return lh_cast_static(
        lh_bool_t, lh_math_ne(lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self)), LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID));
}

lh_fs_file_mode_t
lh_os_fs_file_get_mode(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

lh_void
lh_os_fs_file_set_mode(lh_os_fs_file_t *self, lh_fs_file_mode_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = mode;
}

lh_void
lh_os_fs_file_set_path(lh_os_fs_file_t *self, const lh_fs_path_t *path)
{
    lh_fs_path_assign(lh_os_fs_file_get_path(self), path);
}

lh_void
lh_os_fs_file_init(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    lh_fs_path_init(lh_os_fs_file_get_path(self));
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    lh_os_fs_file_set_mode(self, lh_fs_file_mode_none);
}

lh_void
lh_os_fs_file_close(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    if (!lh_os_fs_file_is_valid(self))
    {
        lh_os_fs_file_set_mode(self, lh_fs_file_mode_none);
        return;
    }
    lh_os_system_fs_file_close(lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self)));
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    lh_os_fs_file_set_mode(self, lh_fs_file_mode_none);
}

lh_void
lh_os_fs_file_deinit(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_fs_file_close(self);
    lh_fs_path_deinit(lh_os_fs_file_get_path(self));
}

lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, const lh_fs_path_t *path, lh_fs_file_mode_t mode)
{
    lh_str_t buf;

    lh_assert_runtime_ref(self);
    lh_str_cptr const cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));
    if (lh_null_eq(cstr))
    {
        lh_str_deinit(lh_addr_of(buf));
        return lh_bool_false;
    }
    lh_os_fs_file_close(self);

    const lh_os_system_fs_file_handle_t handle = lh_os_system_fs_file_open(cstr, mode);
    lh_str_deinit(lh_addr_of(buf));
    if (lh_math_eq(handle, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = handle;
    lh_fs_path_assign(lh_os_fs_file_get_path(self), path);
    lh_os_fs_file_set_mode(self, mode);
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_file_open_cstr(lh_os_fs_file_t *self, lh_str_cptr path, lh_fs_file_mode_t mode)
{
    lh_fs_path_t fp;
    lh_str_view_t text;
    lh_bool_t ok;

    lh_str_view_init(lh_addr_of(text), path);
    lh_fs_path_init(lh_addr_of(fp));
    lh_fs_path_set(lh_addr_of(fp), text, lh_os_system_fs_path_style_native());
    ok = lh_os_fs_file_open(self, lh_addr_of(fp), mode);
    lh_fs_path_deinit(lh_addr_of(fp));
    return ok;
}

lh_ssize_t
lh_os_fs_file_read(lh_self_ptr self, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_fs_file_read(
        lh_ptr_deref(lh_os_fs_file_get_handle_as_const(lh_ptr_cast(lh_os_fs_file_t, self))), buf, size);
}

lh_ssize_t
lh_os_fs_file_write(lh_self_ptr self, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_fs_file_write(
        lh_ptr_deref(lh_os_fs_file_get_handle_as_const(lh_ptr_cast(lh_os_fs_file_t, self))), buf, size);
}

lh_io_reader_t
lh_os_fs_file_get_reader(lh_os_fs_file_t *self)
{
    lh_io_reader_t reader;

    lh_assert_runtime_ref(self);
    lh_io_reader_init(lh_addr_of(reader), lh_os_fs_file_read, self);
    return reader;
}

lh_io_writer_t
lh_os_fs_file_get_writer(lh_os_fs_file_t *self)
{
    lh_io_writer_t writer;

    lh_assert_runtime_ref(self);
    lh_io_writer_init(lh_addr_of(writer), lh_os_fs_file_write, self);
    return writer;
}

lh_io_stream_t
lh_os_fs_file_get_stream(lh_os_fs_file_t *self)
{
    const lh_io_reader_t reader = lh_os_fs_file_get_reader(self);
    const lh_io_writer_t writer = lh_os_fs_file_get_writer(self);

    lh_io_stream_t stream;
    lh_io_stream_init(lh_addr_of(stream), lh_addr_of(reader), lh_addr_of(writer));
    return stream;
}

lh_bool_t
lh_os_fs_file_write_all(lh_os_fs_file_t *self, const lh_ptr buf, lh_usize_t size)
{
    lh_io_writer_t writer = lh_os_fs_file_get_writer(self);

    return lh_io_writer_write_all(lh_addr_of(writer), buf, size);
}

lh_bool_t
lh_os_fs_file_print_v(lh_os_fs_file_t *self, lh_str_cptr fmt, va_list args)
{
    lh_str_t text;
    lh_usize_t n;
    lh_bool_t ok;

    lh_str_init(lh_addr_of(text));
    n = lh_str_format_v(lh_addr_of(text), fmt, args);
    if (n == 0u && fmt[0] != '\0')
    {
        lh_str_deinit(lh_addr_of(text));
        return lh_bool_false;
    }
    ok = lh_os_fs_file_write_all(self, lh_cast_const(lh_ptr, lh_str_get_data(lh_addr_of(text))), n);
    lh_str_deinit(lh_addr_of(text));
    return ok;
}

lh_bool_t
lh_os_fs_file_print(lh_os_fs_file_t *self, lh_str_cptr fmt, ...)
{
    va_list args;
    lh_bool_t ok;

    va_start(args, fmt);
    ok = lh_os_fs_file_print_v(self, fmt, args);
    va_end(args);
    return ok;
}
