#include <lh/os/loader.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/null.h>
#include <lh/os/alloc.h>
#include <lh/runtime/allocator.h>
#include <lh/util/addr.h>
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
    lh_str_init_by_view(lh_os_loader_get_entry_mut(self), lh_str_view_make(entry));
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

    child = lh_ptr_cast(lh_os_module_t, lh_os_alloc(sizeof(*child)));
    if (lh_null_eq(child))
    {
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
