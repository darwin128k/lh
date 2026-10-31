#include <lh/assert.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/fs/dir.h>
#include <lh/math.h>
#include <lh/util/return.h>
#include <lh/util/str/ptr/dot.h>

lh_void
lh_os_fs_dir_init(lh_os_fs_dir_t *self)
{
    lh_os_fs_dir_set_handle(self, LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID);
}

lh_bool_t
lh_os_fs_dir_open(lh_os_fs_dir_t *self, const lh_fs_path_t *path)
{
    lh_str_t buf;

    lh_str_cptr const cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));
    const lh_os_system_fs_dir_handle_t handle =
        lh_null_eq(cstr) ? LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID : lh_os_system_fs_dir_open(cstr);
    lh_str_deinit(lh_addr_of(buf));
    if (lh_null_eq(handle))
    {
        return lh_bool_false;
    }
    lh_os_fs_dir_set_handle(self, handle);
    return lh_bool_true;
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

lh_void
lh_os_fs_dir_close(lh_os_fs_dir_t *self)
{
    if (!lh_os_fs_dir_is_valid(self))
    {
        return;
    }
    lh_os_system_fs_dir_close(lh_os_fs_dir_get_handle(self));
    lh_os_fs_dir_set_handle(self, LH_OS_SYSTEM_FS_DIR_HANDLE_INVALID);
}

lh_ssize_t
lh_os_fs_dir_read(lh_os_fs_dir_t *self, lh_str_view_t *name, lh_os_fs_dir_entry_kind_t *kind)
{
    lh_assert_runtime_ref(name);

    if (!lh_os_fs_dir_is_valid(self))
    {
        lh_os_set_last_error_lit(lh_os_error_code_not_open, "directory is not open");
        return LH_OS_RESULT_INVALID;
    }

    for (;;)
    {
        lh_str_cptr entry;
        lh_os_fs_dir_entry_kind_t entry_kind;
        lh_ssize_t n = lh_os_system_fs_dir_read(lh_os_fs_dir_get_handle(self), lh_addr_of(entry),
                                                lh_addr_of(entry_kind));

        if (!lh_math_gt(n, 0))
        {
            if (lh_math_is_zero(n))
            {
                lh_ptr_deref(name) = lh_str_view_empty();
            }
            return n;
        }
        if (lh_str_ptr_is_dot(entry) || lh_str_ptr_is_double_dot(entry))
        {
            continue;
        }

        /* The entry is consumed either way: an over-long name is reported once,
           and the next read moves on. */
        if (lh_math_gt(lh_cast_static(lh_usize_t, n), lh_os_system_fs_dir_name_max()))
        {
            lh_os_set_last_error_lit(lh_os_error_code_name_too_long, "name is too long");
            return LH_OS_RESULT_INVALID;
        }
        lh_str_view_init(name, entry);
        if (lh_null_ne(kind))
        {
            lh_ptr_deref(kind) = entry_kind;
        }
        return n;
    }
}

lh_bool_t
lh_os_fs_dir_create(const lh_fs_path_t *path)
{
    lh_str_t buf;

    lh_str_cptr const cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));
    const lh_bool_t created =
        lh_cast_static(lh_bool_t, lh_null_ne(cstr) && lh_os_system_fs_dir_create(cstr));
    lh_str_deinit(lh_addr_of(buf));
    if (created)
    {
        return lh_bool_true;
    }
    /* Already there is fine — as long as it is a directory. */
    lh_fs_stat_t info;

    lh_fs_stat_init(lh_addr_of(info));
    return lh_cast_static(lh_bool_t, lh_os_fs_stat(path, lh_addr_of(info)) &&
                                         lh_math_eq(lh_fs_stat_get_kind(lh_addr_of(info)),
                                                    lh_fs_kind_dir));
}

lh_bool_t
lh_os_fs_dir_create_tree(const lh_fs_path_t *path)
{
    lh_return_if(lh_os_fs_dir_create(path), lh_bool_true);
    lh_fs_path_t parent;

    lh_fs_path_init(lh_addr_of(parent));
    /* No parent to make (a root, one relative segment): the error stands. */
    lh_bool_t created = lh_cast_static(
        lh_bool_t, lh_fs_path_parent(lh_addr_of(parent), path) &&
                       !lh_fs_path_is_empty(lh_addr_of(parent)) &&
                       !lh_fs_path_equals(lh_addr_of(parent), path, lh_bool_false) &&
                       lh_os_fs_dir_create_tree(lh_addr_of(parent)) && lh_os_fs_dir_create(path));
    lh_fs_path_deinit(lh_addr_of(parent));
    return created;
}

lh_void
lh_os_fs_dir_set_handle(lh_os_fs_dir_t *self, lh_os_system_fs_dir_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}
