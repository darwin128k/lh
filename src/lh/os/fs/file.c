#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/fs/file.h>
#include <lh/os/system/fs/path.h>
#include <lh/str.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

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

void
lh_os_fs_file_set_mode(lh_os_fs_file_t *self, lh_fs_file_mode_t mode)
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
    lh_ptr_deref(lh_os_fs_file_get_handle(self)) = LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID;
    lh_os_fs_file_set_mode(self, lh_fs_file_mode_none);
}

void
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

void
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
    lh_os_system_fs_file_handle_t handle;

    lh_assert_runtime_ref(self);
    if (lh_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    lh_os_fs_file_close(self);

    handle = lh_os_system_fs_file_open(lh_fs_path_to_cstr(path, lh_os_system_fs_path_style_native(), lh_addr_of(buf)), mode);
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

lh_ssize_t
lh_os_fs_file_read(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_fs_file_read(
        lh_ptr_deref(lh_os_fs_file_get_handle_as_const(lh_ptr_cast(lh_os_fs_file_t, context))), buf, size);
}

lh_ssize_t
lh_os_fs_file_write(lh_ptr context, const lh_ptr buf, lh_usize_t size)
{
    return lh_os_system_fs_file_write(
        lh_ptr_deref(lh_os_fs_file_get_handle_as_const(lh_ptr_cast(lh_os_fs_file_t, context))), buf, size);
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
    lh_io_reader_t reader;
    lh_io_writer_t writer;

    reader = lh_os_fs_file_get_reader(self);
    writer = lh_os_fs_file_get_writer(self);
    return lh_io_stream_make(lh_addr_of(reader), lh_addr_of(writer));
}
