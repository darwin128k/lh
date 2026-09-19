#include <lh/os/fs/file.h>
#include "file/local.h"
#include <lh/assert.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/char.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/fs/path.h>
#include <lh/runtime/error.h>
#include <lh/str.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#else
#    include <fcntl.h>
#    include <unistd.h>
#endif

void
lh_os_fs_file_init(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_FS_FILE_HANDLE_INVALID;
}

lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, const lh_os_fs_path_t *path, lh_os_fs_file_mode_t mode)
{
    lh_str_cptr cstr;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(mode != lh_os_fs_file_mode_read && mode != lh_os_fs_file_mode_write &&
                             mode != lh_os_fs_file_mode_readwrite,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    lh_assert_runtime_ref(path);
    if (lh_os_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_code_path_empty, lh_os_error_desc_lit("path is empty"));
        return lh_bool_false;
    }
    cstr = lh_str_get_data(lh_os_fs_path_get_text_as_const(path));

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        HANDLE native;
        DWORD access;
        DWORD disposition;

        if (mode == lh_os_fs_file_mode_read)
        {
            access = GENERIC_READ;
            disposition = OPEN_EXISTING;
        }
        else if (mode == lh_os_fs_file_mode_write)
        {
            access = GENERIC_WRITE;
            disposition = CREATE_ALWAYS;
        }
        else
        {
            access = GENERIC_READ | GENERIC_WRITE;
            disposition = OPEN_ALWAYS;
        }

        native = CreateFileA(cstr, access, FILE_SHARE_READ, lh_null, disposition,
                             FILE_ATTRIBUTE_NORMAL, lh_null);
        if (native == INVALID_HANDLE_VALUE)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        self->handle = lh_cast_reinterpret(lh_os_fs_file_handle_t, native);
        return lh_bool_true;
    }
#else
    {
        int native;
        int flags;

        if (mode == lh_os_fs_file_mode_read)
        {
            flags = O_RDONLY;
            native = open(cstr, flags);
        }
        else if (mode == lh_os_fs_file_mode_write)
        {
            flags = O_WRONLY | O_CREAT | O_TRUNC;
            native = open(cstr, flags, 0644);
        }
        else
        {
            flags = O_RDWR | O_CREAT;
            native = open(cstr, flags, 0644);
        }
        if (native < 0)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        self->handle = lh_cast_static(lh_os_fs_file_handle_t, native);
        return lh_bool_true;
    }
#endif
}

void
lh_os_fs_file_close(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);

    if (self->handle != LH_OS_FS_FILE_HANDLE_INVALID)
    {
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
        (void)CloseHandle(lh_os_fs_file_native_handle(self));
#else
        (void)close(lh_os_fs_file_native_handle(self));
#endif
        self->handle = LH_OS_FS_FILE_HANDLE_INVALID;
    }
}

lh_os_fs_file_handle_t
lh_os_fs_file_get_handle(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_bool_t
lh_os_fs_file_is_valid(const lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_cast_static(lh_bool_t, self->handle != LH_OS_FS_FILE_HANDLE_INVALID);
}

lh_bool_t
lh_os_fs_file_get_size(const lh_os_fs_file_t *self, lh_u64_t *out)
{
    lh_os_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_file_stat(self, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    *out = lh_os_fs_stat_get_size(lh_addr_of(st));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_file_seek(lh_os_fs_file_t *self, lh_s64_t offset, lh_os_fs_file_seek_whence_t whence,
                   lh_u64_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_if(whence != lh_os_fs_file_seek_set && whence != lh_os_fs_file_seek_cur &&
                             whence != lh_os_fs_file_seek_end,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (!lh_os_fs_file_is_valid(self))
    {
        lh_os_set_last_error(lh_os_error_code_not_open, lh_os_error_desc_lit("file is not open"));
        return lh_bool_false;
    }

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        LARGE_INTEGER dist;
        LARGE_INTEGER pos;
        DWORD method;

        dist.QuadPart = offset;
        method = whence == lh_os_fs_file_seek_set
                     ? FILE_BEGIN
                     : (whence == lh_os_fs_file_seek_cur ? FILE_CURRENT : FILE_END);
        if (!SetFilePointerEx(lh_os_fs_file_native_handle(self), dist, lh_addr_of(pos), method))
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (pos.QuadPart < 0)
        {
            lh_os_set_last_error(lh_os_error_code_negative_size,
                                 lh_os_error_desc_lit("file size is negative"));
            return lh_bool_false;
        }
        *out = lh_cast_static(lh_u64_t, pos.QuadPart);
        return lh_bool_true;
    }
#else
    {
        off_t dist;
        off_t pos;
        int method;

        dist = lh_cast_static(off_t, offset);
        if (lh_cast_static(lh_s64_t, dist) != offset)
        {
            lh_os_set_last_error(lh_os_error_code_invalid_offset,
                                 lh_os_error_desc_lit("offset does not fit"));
            return lh_bool_false;
        }
        method = whence == lh_os_fs_file_seek_set
                     ? SEEK_SET
                     : (whence == lh_os_fs_file_seek_cur ? SEEK_CUR : SEEK_END);
        pos = lseek(lh_os_fs_file_native_handle(self), dist, method);
        if (pos == (off_t)-1)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (pos < 0)
        {
            lh_os_set_last_error(lh_os_error_code_negative_size,
                                 lh_os_error_desc_lit("file size is negative"));
            return lh_bool_false;
        }
        *out = lh_cast_static(lh_u64_t, pos);
        return lh_bool_true;
    }
#endif
}

lh_ssize_t
lh_os_fs_file_read(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    lh_os_fs_file_t *self;

    lh_assert_runtime_ref(context);
    self = lh_ptr_cast(lh_os_fs_file_t, context);

    if (size == 0U)
    {
        return 0;
    }
    lh_assert_runtime_ref(buf);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD chunk;
        DWORD got;

        chunk = size > 0xFFFFFFFFU ? 0xFFFFFFFFU : lh_cast_static(DWORD, size);
        if (!ReadFile(lh_os_fs_file_native_handle(self), buf, chunk, lh_addr_of(got), lh_null))
        {
            lh_os_capture_last_error();
            return lh_cast_static(lh_ssize_t, -1);
        }
        return lh_cast_static(lh_ssize_t, got);
    }
#else
    {
        lh_ssize_t n;

        n = lh_cast_static(lh_ssize_t,
                           read(lh_os_fs_file_native_handle(self), buf, lh_cast_static(size_t, size)));
        if (n < 0)
        {
            lh_os_capture_last_error();
        }
        return n;
    }
#endif
}

lh_ssize_t
lh_os_fs_file_write(lh_ptr context, const lh_ptr buf, lh_usize_t size)
{
    lh_os_fs_file_t *self;

    lh_assert_runtime_ref(context);
    self = lh_ptr_cast(lh_os_fs_file_t, context);

    if (size == 0U)
    {
        return 0;
    }
    lh_assert_runtime_ref(buf);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD chunk;
        DWORD got;

        chunk = size > 0xFFFFFFFFU ? 0xFFFFFFFFU : lh_cast_static(DWORD, size);
        if (!WriteFile(lh_os_fs_file_native_handle(self), buf, chunk, lh_addr_of(got), lh_null))
        {
            lh_os_capture_last_error();
            return lh_cast_static(lh_ssize_t, -1);
        }
        return lh_cast_static(lh_ssize_t, got);
    }
#else
    {
        lh_ssize_t n;

        n = lh_cast_static(
            lh_ssize_t, write(lh_os_fs_file_native_handle(self), buf, lh_cast_static(size_t, size)));
        if (n < 0)
        {
            lh_os_capture_last_error();
        }
        return n;
    }
#endif
}

lh_bool_t
lh_os_fs_file_read_all(lh_os_fs_file_t *self, lh_ptr buf, lh_usize_t size)
{
    lh_uchar_t *bytes;
    lh_usize_t got;
    lh_ssize_t n;

    lh_assert_runtime_ref(self);

    if (size == 0U)
    {
        return lh_bool_true;
    }
    lh_assert_runtime_ref(buf);

    bytes = lh_ptr_cast(lh_uchar_t, buf);
    got = 0U;
    while (got < size)
    {
        n = lh_os_fs_file_read(self, bytes + got, size - got);
        if (n <= 0)
        {
            if (n == 0)
            {
                lh_os_set_last_error(lh_os_error_code_short_read,
                                     lh_os_error_desc_lit("file ended before all bytes were read"));
            }
            return lh_bool_false;
        }
        got += lh_cast_static(lh_usize_t, n);
    }
    return lh_bool_true;
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

    lh_assert_runtime_ref(self);
    reader = lh_os_fs_file_get_reader(self);
    writer = lh_os_fs_file_get_writer(self);
    return lh_io_stream_make(lh_addr_of(reader), lh_addr_of(writer));
}
