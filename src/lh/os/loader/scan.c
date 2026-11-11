#include <lh/os/loader/scan.h>
#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/loader/image.h>

lh_bool_t
lh_os_loader_scan(const lh_fs_path_t *dir, lh_os_loader_scan_cb cb, lh_ptr user)
{
    lh_assert_runtime_ref(cb);
    if (lh_fs_path_is_empty(dir))
    {
        lh_os_set_last_error_lit(lh_os_error_code_path_empty, "path is empty");
        return lh_bool_false;
    }
    if (!lh_os_fs_is_dir(dir))
    {
        return lh_bool_true;
    }

    lh_os_fs_dir_t listing;

    lh_os_fs_dir_init(lh_addr_of(listing));
    if (!lh_os_fs_dir_open(lh_addr_of(listing), dir))
    {
        return lh_bool_false;
    }

    lh_fs_path_t entry;

    lh_fs_path_init(lh_addr_of(entry));

    lh_fs_path_t image;

    lh_fs_path_init(lh_addr_of(image));

    lh_bool_t ok = lh_bool_true;

    for (;;)
    {
        lh_str_view_t name;
        lh_os_fs_dir_entry_kind_t kind;
        const lh_ssize_t got =
            lh_os_fs_dir_read(lh_addr_of(listing), lh_addr_of(name), lh_addr_of(kind));

        if (lh_math_le(got, 0))
        {
            ok = lh_cast_static(lh_bool_t, lh_math_is_zero(got));
            break;
        }
        lh_fs_path_set(lh_addr_of(entry), name, lh_fs_path_style_posix);
        if (lh_os_loader_image_try_resolve(dir, lh_addr_of(entry), lh_addr_of(image)) &&
            lh_os_fs_is_file(lh_addr_of(image)))
        {
            cb(user, lh_addr_of(image));
        }
    }
    lh_os_fs_dir_close(lh_addr_of(listing));
    lh_fs_path_deinit(lh_addr_of(image));
    lh_fs_path_deinit(lh_addr_of(entry));
    return ok;
}
