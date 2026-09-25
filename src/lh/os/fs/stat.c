#include <lh/os/fs/stat.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/os/fs/path.h>
#include <lh/os/system/fs/stat.h>
#include <lh/str.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/ptr.h>

lh_bool_t
lh_os_fs_stat(const lh_fs_path_t *path, lh_fs_stat_t *out)
{
    lh_str_t buf;
    lh_str_cptr cstr;
    lh_bool_t ok;

    cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));
    ok = lh_null_ne(cstr) && lh_os_system_fs_stat(cstr, out);
    lh_str_deinit(lh_addr_of(buf));
    if (ok && lh_fs_path_is_hidden(path))
    {
        lh_fs_stat_set_attr(out, lh_bit_or(lh_fs_stat_get_attr(out), lh_fs_attr_hidden));
    }
    return ok;
}

lh_bool_t
lh_os_fs_mtime(const lh_fs_path_t *path, lh_fs_time_t *out)
{
    lh_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_fs_stat_get_mtime(lh_addr_of(st));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_is_readable(const lh_fs_path_t *path, lh_bool_t *out)
{
    lh_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_fs_perm_is_readable(lh_fs_stat_get_perm(lh_addr_of(st)));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_is_writable(const lh_fs_path_t *path, lh_bool_t *out)
{
    lh_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_fs_perm_is_writable(lh_fs_stat_get_perm(lh_addr_of(st)));
    return lh_bool_true;
}

lh_bool_t
lh_os_fs_is_executable(const lh_fs_path_t *path, lh_bool_t *out)
{
    lh_fs_stat_t st;

    lh_assert_runtime_ref(out);
    if (!lh_os_fs_stat(path, lh_addr_of(st)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_fs_perm_is_executable(lh_fs_stat_get_perm(lh_addr_of(st)));
    return lh_bool_true;
}
