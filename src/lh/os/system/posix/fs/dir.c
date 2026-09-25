#include <lh/os/system/fs/dir.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/system/error/capture.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr.h>

#include <dirent.h>
#include <errno.h>
#include <limits.h>

#ifndef NAME_MAX
#    define NAME_MAX 255
#endif

LH_ATTRIBUTE_STATIC
DIR *
lh_os_system_fs_dir_native(lh_os_system_fs_dir_handle_t handle)
{
    lh_assert_runtime_ref(handle);
    return lh_ptr_cast(DIR, handle);
}

LH_ATTRIBUTE_STATIC
lh_fs_kind_t
lh_os_system_fs_dir_kind(unsigned char type)
{
#ifdef DT_LNK
    if (lh_math_eq(type, DT_LNK))
    {
        return lh_fs_kind_symlink;
    }
#endif
#ifdef DT_DIR
    if (lh_math_eq(type, DT_DIR))
    {
        return lh_fs_kind_dir;
    }
#endif
#ifdef DT_REG
    if (lh_math_eq(type, DT_REG))
    {
        return lh_fs_kind_file;
    }
#endif
    (void)type;
    return lh_fs_kind_other;
}

lh_os_system_fs_dir_handle_t
lh_os_system_fs_dir_open(lh_str_cptr path)
{
    DIR *native;

    lh_assert_runtime_ref(path);
    native = opendir(path);
    if (lh_null_eq(native))
    {
        lh_os_system_error_capture();
        return LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID;
    }
    return native;
}

void
lh_os_system_fs_dir_close(lh_os_system_fs_dir_handle_t handle)
{
    (void)closedir(lh_os_system_fs_dir_native(handle));
}

lh_ssize_t
lh_os_system_fs_dir_read(lh_os_system_fs_dir_handle_t handle, lh_str_cptr *name, lh_fs_kind_t *kind)
{
    struct dirent *entry;

    lh_assert_runtime_ref(name);
    lh_assert_runtime_ref(kind);

    errno = 0;
    entry = readdir(lh_os_system_fs_dir_native(handle));
    if (lh_null_eq(entry))
    {
        if (lh_math_ne(errno, 0))
        {
            lh_os_system_error_capture();
            return -1;
        }
        return 0;
    }
    lh_ptr_deref(name) = entry->d_name;
    lh_ptr_deref(kind) = lh_os_system_fs_dir_kind(entry->d_type);
    return lh_cast_static(lh_ssize_t, lh_str_ptr_len(entry->d_name));
}

lh_usize_t
lh_os_system_fs_dir_name_max(void)
{
    return lh_cast_static(lh_usize_t, NAME_MAX);
}
