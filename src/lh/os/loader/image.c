#include <lh/os/loader/image.h>
#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/os/fs/stat.h>
#include <lh/os/system/shared.h>

lh_bool_t
lh_os_loader_image_has_ext(const lh_fs_path_t *path)
{
    lh_str_view_t name;

    if (!lh_fs_path_try_get_name(path, lh_addr_of(name)))
    {
        return lh_bool_false;
    }

    lh_str_view_t ext;

    lh_os_system_shared_get_ext(lh_addr_of(ext));
    return lh_str_view_ends_with(lh_addr_of(name), lh_addr_of(ext), lh_bool_true);
}

lh_bool_t
lh_os_loader_image_is_named(const lh_fs_path_t *path, const lh_str_view_t *stem)
{
    lh_assert_runtime_ref(stem);

    lh_str_view_t name;

    if (!lh_fs_path_try_get_name(path, lh_addr_of(name)))
    {
        return lh_bool_false;
    }

    lh_str_view_t ext;

    lh_os_system_shared_get_ext(lh_addr_of(ext));
    return lh_cast_static(
        lh_bool_t, lh_math_eq(lh_str_view_get_size(lh_addr_of(name)),
                              lh_math_add(lh_str_view_get_size(stem),
                                          lh_str_view_get_size(lh_addr_of(ext)))) &&
                       lh_str_view_starts_with(lh_addr_of(name), stem, lh_bool_true) &&
                       lh_str_view_ends_with(lh_addr_of(name), lh_addr_of(ext), lh_bool_true));
}

lh_bool_t
lh_os_loader_image_try_resolve(const lh_fs_path_t *base, const lh_fs_path_t *entry,
                               lh_fs_path_t *out)
{
    lh_str_view_t name;

    if (!lh_fs_path_try_get_name(entry, lh_addr_of(name)))
    {
        return lh_bool_false;
    }

    lh_fs_path_t folder;

    lh_fs_path_init(lh_addr_of(folder));
    if (lh_null_eq(base))
    {
        lh_fs_path_assign(lh_addr_of(folder), entry);
    }
    else if (!lh_fs_path_join(lh_addr_of(folder), base, entry))
    {
        lh_fs_path_deinit(lh_addr_of(folder));
        return lh_bool_false;
    }
    if (lh_os_fs_is_file(lh_addr_of(folder)))
    {
        lh_fs_path_assign(out, lh_addr_of(folder));
        lh_fs_path_deinit(lh_addr_of(folder));
        return lh_bool_true;
    }

    /* <folder>/<name><ext>: `name` is a view into `entry`, which stays put. */
    lh_str_view_t ext;

    lh_os_system_shared_get_ext(lh_addr_of(ext));

    lh_str_t leaf_text;

    lh_str_init(lh_addr_of(leaf_text));
    lh_str_append_view(lh_addr_of(leaf_text), name);
    lh_str_append_view(lh_addr_of(leaf_text), ext);

    lh_str_view_t leaf_view;

    lh_str_as_view(lh_addr_of(leaf_text), lh_addr_of(leaf_view));

    lh_fs_path_t leaf;

    lh_fs_path_init(lh_addr_of(leaf));
    lh_fs_path_set(lh_addr_of(leaf), leaf_view, lh_fs_path_style_posix);

    const lh_bool_t ok = lh_fs_path_join(out, lh_addr_of(folder), lh_addr_of(leaf));

    lh_fs_path_deinit(lh_addr_of(leaf));
    lh_str_deinit(lh_addr_of(leaf_text));
    lh_fs_path_deinit(lh_addr_of(folder));
    return ok;
}
