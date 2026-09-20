#include <lh/os/fs/access.h>
#include <lh/assert.h>
#include <lh/compiler/os.h>
#include <lh/os.h>
#include <lh/os/fs/file.h>
#include <lh/os/fs/stat.h>
#include <lh/runtime/error.h>
#include <lh/str.h>
#include <lh/util/addr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <unistd.h>
#endif

static lh_bool_t
lh_os_fs_is_open(const lh_os_fs_path_t *path, lh_os_fs_file_mode_t mode)
{
    lh_os_fs_file_t file;
    lh_bool_t ok;

    lh_os_fs_file_init(lh_addr_of(file));
    ok = lh_os_fs_file_open(lh_addr_of(file), path, mode);
    lh_os_fs_file_deinit(lh_addr_of(file));
    return ok;
}

static lh_bool_t
lh_os_fs_is_executable(const lh_os_fs_path_t *path)
{
    lh_str_cptr cstr;

    cstr = lh_str_get_data(lh_os_fs_path_get_text_as_const(path));
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        DWORD type;
        lh_os_fs_stat_t st;

        if (GetBinaryTypeA(cstr, lh_addr_of(type)))
        {
            return lh_bool_true;
        }
        if (!lh_os_fs_stat(path, lh_addr_of(st)))
        {
            return lh_bool_false;
        }
        if (lh_os_fs_stat_get_kind(lh_addr_of(st)) == lh_os_fs_kind_dir)
        {
            return lh_os_fs_is_open(path, lh_os_fs_file_mode_read);
        }
        lh_os_system_error_capture();
        return lh_bool_false;
    }
#else
    if (access(cstr, X_OK) != 0)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_bool_true;
#endif
}

lh_bool_t
lh_os_fs_is(const lh_os_fs_path_t *path, lh_os_fs_access_t access)
{
    lh_assert_runtime_ref(path);
    lh_assert_runtime_if(access != lh_os_fs_access_readable &&
                             access != lh_os_fs_access_writable &&
                             access != lh_os_fs_access_executable,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    if (lh_os_fs_path_is_empty(path))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty,
                             lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    if (access == lh_os_fs_access_readable)
    {
        return lh_os_fs_is_open(path, lh_os_fs_file_mode_read);
    }
    if (access == lh_os_fs_access_writable)
    {
        lh_os_fs_stat_t st;

        if (!lh_os_fs_stat(path, lh_addr_of(st)))
        {
            return lh_bool_false;
        }
        return lh_os_fs_is_open(path, lh_os_fs_file_mode_readwrite);
    }
    return lh_os_fs_is_executable(path);
}
