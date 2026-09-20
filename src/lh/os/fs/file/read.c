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
#else
#    include <unistd.h>
#endif

static lh_ssize_t
lh_os_fs_file_on_reader_read(lh_ptr context, lh_ptr buf, lh_usize_t size)
{
    return lh_os_fs_file_read(lh_ptr_cast(lh_os_fs_file_t, context), buf, size);
}

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
static DWORD
lh_os_fs_file_win_chunk(lh_usize_t size)
{
    return size > 0xFFFFFFFFU ? 0xFFFFFFFFU : lh_cast_static(DWORD, size);
}

lh_ssize_t
lh_os_fs_file_read(lh_os_fs_file_t *self, lh_ptr buf, lh_usize_t size)
{
    DWORD got;

    if (size == 0U)
    {
        return 0;
    }
    lh_assert_runtime_ref(buf);
    if (!lh_os_fs_file_is_valid(self))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_not_open,
                             lh_os_error_desc_lit("file is not open")));
        return lh_cast_static(lh_ssize_t, -1);
    }
    if (!ReadFile(lh_cast_reinterpret(HANDLE,
                                      lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self))),
                  buf, lh_os_fs_file_win_chunk(size), lh_addr_of(got), lh_null))
    {
        lh_os_system_error_capture();
        return lh_cast_static(lh_ssize_t, -1);
    }
    return lh_cast_static(lh_ssize_t, got);
}
#else
lh_ssize_t
lh_os_fs_file_read(lh_os_fs_file_t *self, lh_ptr buf, lh_usize_t size)
{
    lh_ssize_t n;

    if (size == 0U)
    {
        return 0;
    }
    lh_assert_runtime_ref(buf);
    if (!lh_os_fs_file_is_valid(self))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_not_open,
                             lh_os_error_desc_lit("file is not open")));
        return lh_cast_static(lh_ssize_t, -1);
    }
    n = lh_cast_static(
        lh_ssize_t,
        read(lh_cast_static(int, lh_ptr_deref(lh_os_fs_file_get_handle_as_const(self))), buf,
             lh_cast_static(size_t, size)));
    if (n < 0)
    {
        lh_os_system_error_capture();
    }
    return n;
}
#endif

lh_io_reader_t
lh_os_fs_file_get_reader(lh_os_fs_file_t *self)
{
    lh_io_reader_t reader;

    lh_assert_runtime_ref(self);
    lh_io_reader_init(lh_addr_of(reader), lh_os_fs_file_on_reader_read, self);
    return reader;
}
