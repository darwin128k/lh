#include <lh/os/loader.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/null.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_ATTRIBUTE_STATIC
lh_list_t *
lh_os_loader_get_modules(lh_os_loader_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->modules);
}

LH_ATTRIBUTE_STATIC
const lh_list_t *
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
    lh_list_init(lh_os_loader_get_modules(self));
    lh_os_loader_set_owner(self, owner);
    lh_str_init_by_view(lh_os_loader_get_entry_mut(self), lh_str_view_make(entry));
}

void
lh_os_loader_unload(lh_os_loader_t *self)
{
    lh_list_t *const modules = lh_os_loader_get_modules(self);

    /* Last loaded first: a later child may depend on an earlier one. */
    for (lh_os_module_t *child = lh_os_module_get_by_node(lh_list_pop_back(modules));
         lh_ptr_is_set(child); child = lh_os_module_get_by_node(lh_list_pop_back(modules)))
    {
        lh_os_module_destroy(child);
    }
}

void
lh_os_loader_unload_child(lh_os_loader_t *self, lh_os_module_t *child)
{
    lh_assert_runtime_if(lh_ptr_ne(lh_os_module_get_owner(child), self),
                         lh_runtime_error_code_invalid_argument);
    lh_list_node_unlink(lh_os_module_get_node(child));
    lh_os_module_destroy(child);
}

void
lh_os_loader_deinit(lh_os_loader_t *self)
{
    lh_os_loader_unload(self);
    lh_str_deinit(lh_os_loader_get_entry_mut(self));
}

/* ── load ────────────────────────────────────────────────────────────────── */

/* Open, take the method table the image exports under `entry`, start. The
   owner is set before start runs, so start may already reach its parent. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_loader_bring_up(lh_os_loader_t *self, lh_os_module_t *child, const lh_fs_path_t *path)
{
    lh_os_module_set_owner(child, self);
    if (!lh_os_module_open(child, path))
    {
        return lh_bool_false;
    }
    const lh_os_module_ops_t *const ops = lh_ptr_cast(
        const lh_os_module_ops_t, lh_os_module_get_sym(child, lh_os_loader_get_entry(self)));
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
    lh_os_module_t *const child = lh_os_module_create();

    if (lh_null_eq(child))
    {
        return lh_null;
    }
    if (!lh_os_loader_bring_up(self, child, path))
    {
        lh_os_module_destroy(child);
        return lh_null;
    }
    lh_list_push_back(lh_os_loader_get_modules(self), lh_os_module_get_node(child));
    return child;
}

/* ── children ────────────────────────────────────────────────────────────── */

lh_usize_t
lh_os_loader_get_loaded(const lh_os_loader_t *self)
{
    return lh_list_get_size(lh_os_loader_get_modules_as_const(self));
}

lh_os_module_t *
lh_os_loader_get_first(const lh_os_loader_t *self)
{
    return lh_os_module_get_by_node(lh_list_get_first(lh_os_loader_get_modules_as_const(self)));
}

lh_os_module_t *
lh_os_loader_get_next(const lh_os_loader_t *self, lh_os_module_t *child)
{
    return lh_os_module_get_by_node(
        lh_list_get_next(lh_os_loader_get_modules_as_const(self), lh_os_module_get_node(child)));
}

lh_os_module_t *
lh_os_loader_get_at(const lh_os_loader_t *self, lh_uindex_t index)
{
    return lh_os_module_get_by_node(lh_list_get_at(lh_os_loader_get_modules_as_const(self), index));
}
