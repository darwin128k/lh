#include <lh/os/fs/file.h>
#include <lh/assert.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
typedef HANDLE lh_os_fs_native_handle_t;
#else
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <unistd.h>
typedef int lh_os_fs_native_handle_t;
#endif

static lh_os_fs_native_handle_t
lh_os_fs_file_native_handle(const lh_os_fs_file_t *self)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return lh_cast_reinterpret(lh_os_fs_native_handle_t, self->handle);
#else
    return lh_cast_static(lh_os_fs_native_handle_t, self->handle);
#endif
}

void
lh_os_fs_file_init(lh_os_fs_file_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_FS_FILE_HANDLE_INVALID;
}

lh_bool_t
lh_os_fs_file_open(lh_os_fs_file_t *self, lh_str_cptr path, lh_os_fs_file_mode_t mode)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(mode != lh_os_fs_file_mode_read && mode != lh_os_fs_file_mode_write,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    if (lh_null_eq(path))
    {
        lh_os_set_last_error(1, lh_os_error_desc_lit("path is null"));
        return lh_bool_false;
    }
    if (path[0] == '\0')
    {
        lh_os_set_last_error(1, lh_os_error_desc_lit("path is empty"));
        return lh_bool_false;
    }

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
        else
        {
            access = GENERIC_WRITE;
            disposition = CREATE_ALWAYS;
        }

        native = CreateFileA(path, access, FILE_SHARE_READ, lh_null, disposition,
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
            native = open(path, flags);
        }
        else
        {
            flags = O_WRONLY | O_CREAT | O_TRUNC;
            native = open(path, flags, 0644);
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
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        LARGE_INTEGER size;

        if (!GetFileSizeEx(lh_os_fs_file_native_handle(self), lh_addr_of(size)))
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (size.QuadPart < 0)
        {
            lh_os_set_last_error(1, lh_os_error_desc_lit("file size is negative"));
            return lh_bool_false;
        }
        *out = lh_cast_static(lh_u64_t, size.QuadPart);
        return lh_bool_true;
    }
#else
    {
        struct stat info;

        if (fstat(lh_os_fs_file_native_handle(self), lh_addr_of(info)) != 0)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (info.st_size < 0)
        {
            lh_os_set_last_error(1, lh_os_error_desc_lit("file size is negative"));
            return lh_bool_false;
        }
        *out = lh_cast_static(lh_u64_t, info.st_size);
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
