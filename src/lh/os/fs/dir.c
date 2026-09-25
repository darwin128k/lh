#include <lh/os/fs/dir.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/path.h>
#include <lh/os/system/fs/dir.h>
#include <lh/os/system/fs/path.h>
#include <lh/str.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/str/ptr/dot.h>

void
lh_os_fs_dir_init(lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    self->handle = LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID;
}

lh_bool_t
lh_os_fs_dir_open(lh_os_fs_dir_t *self, const lh_fs_path_t *path)
{
    lh_str_t buf;
    lh_str_cptr cstr;
    lh_os_system_fs_dir_handle_t handle;

    lh_assert_runtime_ref(self);
    cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));
    handle = lh_null_eq(cstr) ? LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID : lh_os_system_fs_dir_open(cstr);
    lh_str_deinit(lh_addr_of(buf));
    if (lh_null_eq(handle))
    {
        return lh_bool_false;
    }
    self->handle = handle;
    return lh_bool_true;
}

void
lh_os_fs_dir_close(lh_os_fs_dir_t *self)
{
    if (!lh_os_fs_dir_is_valid(self))
    {
        return;
    }
    lh_os_system_fs_dir_close(lh_os_fs_dir_get_handle(self));
    self->handle = LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID;
}

lh_os_system_fs_dir_handle_t
lh_os_fs_dir_get_handle(const lh_os_fs_dir_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_bool_t
lh_os_fs_dir_is_valid(const lh_os_fs_dir_t *self)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_os_fs_dir_get_handle(self)));
}

lh_ssize_t
lh_os_fs_dir_read(lh_os_fs_dir_t *self, lh_fs_path_t *name, lh_os_fs_dir_entry_kind_t *kind)
{
    lh_str_cptr entry;
    lh_os_fs_dir_entry_kind_t entry_kind;
    lh_ssize_t n;

    lh_assert_runtime_ref(name);
    if (!lh_os_fs_dir_is_valid(self))
    {
        lh_os_set_last_error(
            lh_os_error_make(lh_os_error_code_not_open, lh_os_error_desc_lit("directory is not open")));
        return -1;
    }

    do
    {
        n = lh_os_system_fs_dir_read(lh_os_fs_dir_get_handle(self), lh_addr_of(entry), lh_addr_of(entry_kind));
        if (!lh_math_gt(n, 0))
        {
            if (lh_math_is_zero(n))
            {
                lh_fs_path_clear(name);
            }
            return n;
        }
    } while (lh_str_ptr_is_dot(entry) || lh_str_ptr_is_double_dot(entry));

    /* The entry is consumed either way: an over-long name is reported once,
       and the next read moves on. */
    if (lh_math_gt(lh_cast_static(lh_usize_t, n), lh_os_system_fs_dir_name_max()))
    {
        lh_os_set_last_error(
            lh_os_error_make(lh_os_error_code_name_too_long, lh_os_error_desc_lit("name is too long")));
        return -1;
    }
    lh_fs_path_set(name, lh_str_view_make(entry), lh_os_system_fs_path_style_native());
    if (lh_null_ne(kind))
    {
        lh_ptr_deref(kind) = entry_kind;
    }
    return n;
}
