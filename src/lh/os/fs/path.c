#include <lh/os/fs/path.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/fs/path.h>

lh_str_cptr
lh_os_fs_path_to_cstr(const lh_fs_path_t *path, lh_str_t *scratch)
{
    if (lh_fs_path_is_empty(path))
    {
        lh_str_init(scratch);
        lh_os_set_last_error_lit(lh_os_error_code_path_empty, "path is empty");
        return lh_null;
    }
    return lh_fs_path_to_cstr(path, lh_os_system_fs_path_style_native(), scratch);
}

lh_bool_t
lh_os_fs_path_canonical(const lh_fs_path_t *path, lh_fs_path_t *out)
{
    lh_str_t native;

    lh_str_cptr text = lh_os_fs_path_to_cstr(path, lh_addr_of(native));
    lh_str_t resolved;

    lh_str_init(lh_addr_of(resolved));
    lh_bool_t ok = lh_bool_false;

    if (lh_null_ne(text))
    {
        ok = lh_os_system_fs_path_canonical(text, lh_addr_of(resolved));
    }
    if (ok)
    {
        lh_str_view_t view;

        lh_str_as_view(lh_addr_of(resolved), lh_addr_of(view));
        lh_fs_path_set(out, view, lh_os_system_fs_path_style_native());
    }
    lh_str_deinit(lh_addr_of(resolved));
    lh_str_deinit(lh_addr_of(native));
    return ok;
}
