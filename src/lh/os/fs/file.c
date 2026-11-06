#include <lh/os/fs/file.h>
#include <lh/core/char/map.h>
#include <lh/core/assert.h>
#include <lh/core/cast/const.h>
#include <lh/os/fs/path.h>
#include <lh/os/system/fs/file.h>
#include <lh/os/system/fs/path.h>
#include <lh/core/math.h>

#include <stdarg.h>

lh_fs_path_t *
lh_os_fs_file_get_path_mut(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

const lh_fs_path_t *
lh_os_fs_file_get_path(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

lh_os_system_fs_file_handle_t *
lh_os_fs_file_get_handle_mut(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

const lh_os_system_fs_file_handle_t *
lh_os_fs_file_get_handle(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

lh_void
lh_os_fs_file_set_handle(lh_os_fs_file_t *self, lh_os_system_fs_file_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

lh_bool_t
lh_os_fs_file_is_valid(const lh_os_fs_file_t *self)
{
    return lh_cast_static(lh_bool_t, lh_math_ne(lh_ptr_deref(lh_os_fs_file_get_handle(self)),
                                                LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID));
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
    lh_fs_path_assign(lh_os_fs_file_get_path_mut(self), path);
}

lh_void
lh_os_fs_file_init(lh_os_fs_file_t *self)
{
    lh_fs_path_init(lh_os_fs_file_get_path_mut(self));
    lh_os_fs_file_set_handle(self, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    lh_os_fs_file_set_mode(self, lh_fs_file_mode_none);
}

lh_void
lh_os_fs_file_close(lh_os_fs_file_t *self)
{
    if (!lh_os_fs_file_is_valid(self))
    {
        lh_os_fs_file_set_mode(self, lh_fs_file_mode_none);
        return;
    }
    lh_os_system_fs_file_close(lh_ptr_deref(lh_os_fs_file_get_handle(self)));
    lh_os_fs_file_set_handle(self, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    lh_os_fs_file_set_mode(self, lh_fs_file_mode_none);
}

lh_void
lh_os_fs_file_deinit(lh_os_fs_file_t *self)
{
    lh_os_fs_file_close(self);
    lh_fs_path_deinit(lh_os_fs_file_get_path_mut(self));
}

lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, const lh_fs_path_t *path, lh_fs_file_mode_t mode)
{
    lh_str_t buf;

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
    lh_os_fs_file_set_handle(self, handle);
    lh_fs_path_assign(lh_os_fs_file_get_path_mut(self), path);
    lh_os_fs_file_set_mode(self, mode);
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_file_try_open_root(lh_os_fs_file_t *self, const lh_fs_path_t *path)
{
    lh_str_t text;
    lh_str_cptr native = lh_os_fs_path_to_cstr(path, lh_addr_of(text));
    lh_os_fs_file_close(self);
    if (lh_null_eq(native))
    {
        lh_str_deinit(lh_addr_of(text));
        return lh_bool_false;
    }
    lh_os_system_fs_file_handle_t handle = lh_os_system_fs_file_open_root(native);
    lh_str_deinit(lh_addr_of(text));
    if (lh_math_eq(handle, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID))
    {
        return lh_bool_false;
    }
    lh_os_fs_file_set_handle(self, handle);
    lh_os_fs_file_set_path(self, path);
    lh_os_fs_file_set_mode(self, lh_fs_file_mode_read);
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_file_try_open_read_at(lh_os_fs_file_t *self, const lh_os_fs_file_t *root,
                               const lh_fs_path_t *path, lh_fs_size_t *size)
{
    lh_assert_runtime_if(lh_math_eq(self, root), lh_runtime_error_code_invalid_argument);
    lh_str_t text;
    lh_str_cptr relative = lh_fs_path_to_cstr(path, lh_fs_path_style_posix, lh_addr_of(text));
    lh_os_fs_file_close(self);
    lh_os_system_fs_file_handle_t handle = lh_os_system_fs_file_open_read_at(
        lh_ptr_deref(lh_os_fs_file_get_handle(root)), relative, size);
    lh_str_deinit(lh_addr_of(text));
    if (lh_math_eq(handle, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID))
    {
        return lh_bool_false;
    }
    lh_os_fs_file_set_handle(self, handle);
    lh_os_fs_file_set_path(self, path);
    lh_os_fs_file_set_mode(self, lh_fs_file_mode_read);
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_file_open_cstr(lh_os_fs_file_t *self, lh_str_cptr path, lh_fs_file_mode_t mode)
{
    lh_fs_path_t fp;

    lh_fs_path_init(lh_addr_of(fp));
    lh_str_view_t text;

    lh_str_view_init(lh_addr_of(text), path);
    lh_fs_path_set(lh_addr_of(fp), text, lh_os_system_fs_path_style_native());
    lh_bool_t ok = lh_os_fs_file_open(self, lh_addr_of(fp), mode);

    lh_fs_path_deinit(lh_addr_of(fp));
    return ok;
}

lh_void
lh_os_fs_file_open_root(lh_os_fs_file_t *self, const lh_fs_path_t *path)
{
    lh_runtime_check_if(!lh_os_fs_file_try_open_root(self, path),
                        lh_runtime_error_code_not_accepted);
}

lh_bool_t
lh_os_fs_file_try_open_root_cstr(lh_os_fs_file_t *self, lh_str_cptr path)
{
    lh_fs_path_t parsed;
    lh_fs_path_init(lh_addr_of(parsed));
    lh_str_view_t text;
    lh_str_view_init(lh_addr_of(text), path);
    lh_fs_path_set(lh_addr_of(parsed), text, lh_os_system_fs_path_style_native());
    lh_bool_t opened = lh_os_fs_file_try_open_root(self, lh_addr_of(parsed));
    lh_fs_path_deinit(lh_addr_of(parsed));
    return opened;
}

lh_void
lh_os_fs_file_open_read_at(lh_os_fs_file_t *self, const lh_os_fs_file_t *root,
                           const lh_fs_path_t *path, lh_fs_size_t *size)
{
    lh_runtime_check_if(!lh_os_fs_file_try_open_read_at(self, root, path, size),
                        lh_runtime_error_code_not_accepted);
}

lh_void
lh_os_fs_file_open_root_cstr(lh_os_fs_file_t *self, lh_str_cptr path)
{
    lh_runtime_check_if(!lh_os_fs_file_try_open_root_cstr(self, path),
                        lh_runtime_error_code_not_accepted);
}

lh_ssize_t
lh_os_fs_file_read(lh_self_ptr self, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_fs_file_read(
        lh_ptr_deref(lh_os_fs_file_get_handle(lh_ptr_cast(lh_os_fs_file_t, self))), buf, size);
}

lh_ssize_t
lh_os_fs_file_write(lh_self_ptr self, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_fs_file_write(
        lh_ptr_deref(lh_os_fs_file_get_handle(lh_ptr_cast(lh_os_fs_file_t, self))), buf, size);
}

lh_void
lh_os_fs_file_get_reader(lh_os_fs_file_t *self, lh_io_reader_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);

    lh_io_reader_t reader;

    lh_io_reader_init(lh_addr_of(reader), lh_os_fs_file_read, self);
    lh_ptr_deref(out) = reader;
}

lh_void
lh_os_fs_file_get_writer(lh_os_fs_file_t *self, lh_io_writer_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ref(self);

    lh_io_writer_t writer;

    lh_io_writer_init(lh_addr_of(writer), lh_os_fs_file_write, self);
    lh_ptr_deref(out) = writer;
}

lh_void
lh_os_fs_file_get_stream(lh_os_fs_file_t *self, lh_io_stream_t *out)
{
    lh_assert_runtime_ref(out);
    lh_io_reader_t reader;
    lh_os_fs_file_get_reader(self, lh_addr_of(reader));
    lh_io_writer_t writer;
    lh_os_fs_file_get_writer(self, lh_addr_of(writer));

    lh_io_stream_t stream;
    lh_io_stream_init(lh_addr_of(stream), lh_addr_of(reader), lh_addr_of(writer));
    lh_ptr_deref(out) = stream;
}

lh_bool_t
lh_os_fs_file_write_all(lh_os_fs_file_t *self, const lh_ptr buf, lh_usize_t size)
{
    lh_io_writer_t writer;
    lh_os_fs_file_get_writer(self, lh_addr_of(writer));

    return lh_io_writer_write_all(lh_addr_of(writer), buf, size);
}

lh_bool_t
lh_os_fs_file_print_v(lh_os_fs_file_t *self, lh_str_cptr fmt, va_list args)
{
    lh_str_t text;

    lh_str_init(lh_addr_of(text));
    lh_usize_t n = lh_str_format_v(lh_addr_of(text), fmt, args);

    if (n == 0u && fmt[0] != lh_char_map_nul)
    {
        lh_str_deinit(lh_addr_of(text));
        return lh_bool_false;
    }
    lh_bool_t ok =
        lh_os_fs_file_write_all(self, lh_cast_const(lh_ptr, lh_str_get_data(lh_addr_of(text))), n);

    lh_str_deinit(lh_addr_of(text));
    return ok;
}

lh_bool_t
lh_os_fs_file_print(lh_os_fs_file_t *self, lh_str_cptr fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    lh_bool_t ok = lh_os_fs_file_print_v(self, fmt, args);

    va_end(args);
    return ok;
}

lh_bool_t
lh_os_fs_file_remove(const lh_fs_path_t *path)
{
    lh_str_t buf;

    lh_str_cptr const cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));
    const lh_bool_t removed =
        lh_cast_static(lh_bool_t, lh_null_ne(cstr) && lh_os_system_fs_file_remove(cstr));
    lh_str_deinit(lh_addr_of(buf));
    return removed;
}
