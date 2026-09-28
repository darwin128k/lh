#include <lh/os/loader.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/fs/path/style.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/dir.h>
#include <lh/os/system.h>
#include <lh/os/system/error/kind.h>
#include <lh/os/system/shared.h>
#include <lh/runtime/allocator.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_ATTRIBUTE_STATIC
lh_vector_t *
lh_os_loader_get_modules(lh_os_loader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->modules);
}

LH_ATTRIBUTE_STATIC
const lh_vector_t *
lh_os_loader_get_modules_as_const(const lh_os_loader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->modules);
}

lh_os_module_t *
lh_os_loader_get_owner(const lh_os_loader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

LH_ATTRIBUTE_STATIC
void
lh_os_loader_set_owner(lh_os_loader_t *self, lh_os_module_t *owner)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(owner);
    self->owner = owner;
}

LH_ATTRIBUTE_STATIC
lh_str_t *
lh_os_loader_get_entry_mut(lh_os_loader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entry);
}

lh_str_cptr
lh_os_loader_get_entry(const lh_os_loader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_str_get_data(lh_addr_of(self->entry));
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

void
lh_os_loader_init(lh_os_loader_t *self, lh_os_module_t *owner, lh_str_cptr entry)
{
    lh_assert_runtime_ref(entry);
    lh_vector_init(lh_os_loader_get_modules(self), sizeof(lh_os_module_t *));
    lh_os_loader_set_owner(self, owner);
    lh_str_init(lh_os_loader_get_entry_mut(self));
    lh_str_append_view(lh_os_loader_get_entry_mut(self), lh_str_view_make(entry));
}

void
lh_os_loader_unload(lh_os_loader_t *self)
{
    lh_vector_t *modules;
    lh_os_module_t *child;

    modules = lh_os_loader_get_modules(self);
    while (!lh_vector_is_empty(modules))
    {
        lh_vector_pop_back(modules, lh_addr_of(child));
        lh_os_module_deinit(child);
        lh_runtime_allocator_free(child);
    }
}

void
lh_os_loader_deinit(lh_os_loader_t *self)
{
    lh_os_loader_unload(self);
    lh_vector_deinit(lh_os_loader_get_modules(self));
    lh_str_deinit(lh_os_loader_get_entry_mut(self));
}

/* ── load ────────────────────────────────────────────────────────────────── */

/* Open, take the method table the image exports under `entry`, start. The
   owner is set before start runs, so start may already reach its parent. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_loader_bring_up(lh_os_loader_t *self, lh_os_module_t *child, const lh_fs_path_t *path)
{
    const lh_os_module_ops_t *ops;

    lh_os_module_set_owner(child, self);
    if (!lh_os_module_open(child, path))
    {
        return lh_bool_false;
    }
    ops = lh_ptr_cast(const lh_os_module_ops_t, lh_os_module_get_sym(child, lh_os_loader_get_entry(self)));
    if (lh_null_eq(ops))
    {
        return lh_bool_false; /* not a module: a plain shared library */
    }
    lh_os_module_set_ops(child, ops);
    return lh_os_module_start(child);
}

lh_os_module_t *
lh_os_loader_load(lh_os_loader_t *self, const lh_fs_path_t *path)
{
    lh_os_module_t *child;

    child = lh_ptr_cast(lh_os_module_t, lh_runtime_allocator_alloc(sizeof(*child)));
    if (lh_null_eq(child))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_out_of_memory, lh_os_error_desc_lit("out of memory")));
        return lh_null;
    }
    lh_os_module_init(child);
    if (!lh_os_loader_bring_up(self, child, path))
    {
        lh_os_module_deinit(child);
        lh_runtime_allocator_free(child);
        return lh_null;
    }
    lh_vector_push_back(lh_os_loader_get_modules(self), lh_addr_of(child));
    return child;
}

/* A regular entry whose name ends with the platform suffix (`ext`). */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_loader_is_image(lh_os_fs_dir_entry_kind_t kind, const lh_str_view_t *name, const lh_str_view_t *ext)
{
    if (lh_math_eq(kind, lh_os_fs_dir_entry_kind_dir) || lh_math_eq(kind, lh_os_fs_dir_entry_kind_symlink))
    {
        return lh_bool_false;
    }
    return lh_str_view_ends_with(name, ext, lh_bool_true);
}

/* Called right after lh_os_fs_dir_open failed: true when the OS said there
   is no directory there (missing, or not a directory) — a scan of nothing.
   The caller clears the native slot before opening: our own checks (out of
   memory) never touch it, so a stale "not found" from an earlier call must
   not pass for this one. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_loader_is_absent_dir(void)
{
    lh_os_system_error_code_t code;

    code = lh_os_system_get_last_error_code();
    return lh_cast_static(lh_bool_t,
                          lh_os_system_error_code_is_not_found(code) || lh_os_system_error_code_is_not_dir(code));
}

lh_bool_t
lh_os_loader_load_dir(lh_os_loader_t *self, const lh_fs_path_t *dir, const lh_fs_path_t *skip)
{
    lh_os_fs_dir_t listing;
    lh_fs_path_t name;
    lh_fs_path_t full;
    lh_str_view_t ext;
    lh_str_view_t entry;
    lh_os_fs_dir_entry_kind_t kind;
    lh_ssize_t n;

    if (lh_fs_path_is_empty(dir))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty, lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    lh_os_fs_dir_init(lh_addr_of(listing));
    lh_os_system_error_clear(lh_os_system_last_error());
    if (!lh_os_fs_dir_open(lh_addr_of(listing), dir))
    {
        return lh_os_loader_is_absent_dir();
    }

    ext = lh_os_system_shared_ext();
    lh_fs_path_init(lh_addr_of(name));
    lh_fs_path_init(lh_addr_of(full));
    for (;;)
    {
        n = lh_os_fs_dir_read(lh_addr_of(listing), lh_addr_of(entry), lh_addr_of(kind));
        if (!lh_math_gt(n, 0))
        {
            break;
        }
        if (!lh_os_loader_is_image(kind, lh_addr_of(entry), lh_addr_of(ext)))
        {
            continue;
        }
        lh_fs_path_set(lh_addr_of(name), entry, lh_fs_path_style_posix);
        if (!lh_fs_path_join(lh_addr_of(full), dir, lh_addr_of(name)) ||
            (lh_null_ne(skip) && lh_fs_path_equals(lh_addr_of(full), skip, lh_bool_false)))
        {
            continue;
        }
        lh_os_loader_load(self, lh_addr_of(full));
    }
    lh_os_fs_dir_close(lh_addr_of(listing));
    lh_fs_path_deinit(lh_addr_of(name));
    lh_fs_path_deinit(lh_addr_of(full));
    /* 0 is the end of the listing, a negative count a failed read. */
    return lh_cast_static(lh_bool_t, lh_math_is_zero(n));
}

lh_bool_t
lh_os_loader_load_beside(lh_os_loader_t *self, const lh_fs_path_t *image, const lh_fs_path_t *subdir)
{
    lh_fs_path_t dir;
    lh_bool_t ok;

    if (lh_fs_path_is_empty(image))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_path_empty, lh_os_error_desc_lit("path is empty")));
        return lh_bool_false;
    }
    /* `image` is non-empty, so parent can only fail on a bare root: nothing beside it. */
    lh_fs_path_init(lh_addr_of(dir));
    ok = lh_cast_static(lh_bool_t, !lh_fs_path_parent(lh_addr_of(dir), image) ||
                                       (lh_fs_path_join(lh_addr_of(dir), lh_addr_of(dir), subdir) &&
                                        lh_os_loader_load_dir(self, lh_addr_of(dir), image)));
    lh_fs_path_deinit(lh_addr_of(dir));
    return ok;
}

/* ── children ────────────────────────────────────────────────────────────── */

lh_usize_t
lh_os_loader_get_loaded(const lh_os_loader_t *self)
{
    return lh_vector_get_size(lh_os_loader_get_modules_as_const(self));
}

lh_os_module_t *
lh_os_loader_get(const lh_os_loader_t *self, lh_uindex_t index)
{
    const lh_vector_t *modules;

    modules = lh_os_loader_get_modules_as_const(self);
    if (!lh_vector_is_valid_index(modules, index))
    {
        return lh_null;
    }
    return lh_ptr_deref(lh_ptr_cast(lh_os_module_t *, lh_vector_get_ptr(modules, index)));
}
