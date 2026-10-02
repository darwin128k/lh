#include <lh/os/module.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/list.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/alloc.h>
#include <lh/os/error/code.h>
#include <lh/os/loader.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_ATTRIBUTE_STATIC
lh_os_shared_t *
lh_os_module_get_image(lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->image);
}

const lh_os_shared_t *
lh_os_module_get_image_as_const(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->image);
}

const lh_fs_path_t *
lh_os_module_get_path_as_const(const lh_os_module_t *self)
{
    return lh_os_shared_get_path_as_const(lh_os_module_get_image_as_const(self));
}

lh_os_system_shared_handle_t
lh_os_module_get_handle(const lh_os_module_t *self)
{
    return lh_os_shared_get_handle(lh_os_module_get_image_as_const(self));
}

lh_bool_t
lh_os_module_is_loaded(const lh_os_module_t *self)
{
    return lh_os_shared_is_open(lh_os_module_get_image_as_const(self));
}

lh_bool_t
lh_os_module_is_started(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return self->started;
}

LH_ATTRIBUTE_STATIC
void
lh_os_module_set_started(lh_os_module_t *self, lh_bool_t started)
{
    lh_assert_runtime_ref(self);
    self->started = started;
}

const lh_os_module_ops_t *
lh_os_module_get_ops(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ops;
}

void
lh_os_module_set_ops(lh_os_module_t *self, const lh_os_module_ops_t *ops)
{
    lh_assert_runtime_if(lh_os_module_is_started(self), lh_runtime_error_code_invalid_argument);
    self->ops = ops;
}

lh_ptr
lh_os_module_get_data(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return self->data;
}

void
lh_os_module_set_data(lh_os_module_t *self, lh_ptr data)
{
    lh_assert_runtime_ref(self);
    self->data = data;
}

lh_os_loader_t *
lh_os_module_get_owner(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

void
lh_os_module_set_owner(lh_os_module_t *self, lh_os_loader_t *owner)
{
    lh_assert_runtime_ref(self);
    self->owner = owner;
}

lh_list_node_t *
lh_os_module_get_node(lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_os_module_t *
lh_os_module_get_by_node(lh_list_node_t *node)
{
    return lh_list_entry(lh_os_module_t, node, node);
}

lh_os_module_t *
lh_os_module_get_parent(const lh_os_module_t *self)
{
    lh_os_loader_t *const owner = lh_os_module_get_owner(self);

    return lh_null_eq(owner) ? lh_null : lh_os_loader_get_owner(owner);
}

lh_os_loader_t *
lh_os_module_get_loader(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return self->loader;
}

LH_ATTRIBUTE_STATIC
void
lh_os_module_set_loader(lh_os_module_t *self, lh_os_loader_t *loader)
{
    lh_assert_runtime_ref(self);
    self->loader = loader;
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

void
lh_os_module_init(lh_os_module_t *self)
{
    lh_os_shared_init(lh_os_module_get_image(self));
    lh_os_module_set_started(self, lh_bool_false);
    lh_os_module_set_ops(self, lh_null);
    lh_os_module_set_data(self, lh_null);
    lh_os_module_set_owner(self, lh_null);
    lh_os_module_set_loader(self, lh_null);
    lh_list_node_init(lh_os_module_get_node(self));
}

void
lh_os_module_deinit(lh_os_module_t *self)
{
    /* A child still in its owner's list would leave the list pointing at freed memory. */
    lh_assert_runtime_if(
        lh_list_node_is_linked(lh_os_module_get_node(self)), lh_runtime_error_code_invalid_argument,
        "module is still held by its loader: unload it with lh_os_loader_unload_child");
    lh_os_module_close(self);
    lh_os_shared_deinit(lh_os_module_get_image(self));
}

lh_os_module_t *
lh_os_module_create(void)
{
    lh_os_module_t *const self = lh_ptr_cast(lh_os_module_t, lh_os_alloc(sizeof(*self)));

    if (lh_null_ne(self))
    {
        lh_os_module_init(self);
    }
    return self;
}

void
lh_os_module_destroy(lh_os_module_t *self)
{
    lh_os_module_deinit(self);
    lh_runtime_allocator_free(self);
}

/* ── privilege ───────────────────────────────────────────────────────────── */

lh_os_loader_t *
lh_os_module_grant_loader(lh_os_module_t *self, lh_str_cptr entry)
{
    if (lh_null_ne(lh_os_module_get_loader(self)))
    {
        lh_os_set_last_error_lit(lh_os_error_code_already_open, "loader already granted");
        return lh_null;
    }
    lh_os_loader_t *const loader = lh_os_loader_create(self, entry);

    if (lh_null_ne(loader))
    {
        lh_os_module_set_loader(self, loader);
    }
    return loader;
}

/* Children go before their parent: they may still use what it provides. */
LH_ATTRIBUTE_STATIC
void
lh_os_module_revoke_loader(lh_os_module_t *self)
{
    lh_os_loader_t *const loader = lh_os_module_get_loader(self);

    if (lh_null_eq(loader))
    {
        return;
    }
    lh_os_loader_destroy(loader);
    lh_os_module_set_loader(self, lh_null);
}

/* ── lifecycle ───────────────────────────────────────────────────────────── */

lh_bool_t
lh_os_module_start(lh_os_module_t *self)
{
    if (lh_os_module_is_started(self))
    {
        return lh_bool_true;
    }
    const lh_os_module_ops_t *const ops = lh_os_module_get_ops(self);
    if (lh_null_ne(ops) && lh_null_ne(ops->start) && !ops->start(self))
    {
        /* A refused start may have been granted a loader before refusing. */
        lh_os_module_revoke_loader(self);
        return lh_bool_false;
    }
    lh_os_module_set_started(self, lh_bool_true);
    return lh_bool_true;
}

void
lh_os_module_stop(lh_os_module_t *self)
{
    lh_os_module_revoke_loader(self);
    if (!lh_os_module_is_started(self))
    {
        return;
    }
    const lh_os_module_ops_t *const ops = lh_os_module_get_ops(self);
    if (lh_null_ne(ops) && lh_null_ne(ops->stop))
    {
        ops->stop(self);
    }
    lh_os_module_set_started(self, lh_bool_false);
}

/* ── image ───────────────────────────────────────────────────────────────── */

lh_bool_t
lh_os_module_open(lh_os_module_t *self, const lh_fs_path_t *path)
{
    return lh_os_shared_open(lh_os_module_get_image(self), path);
}

lh_bool_t
lh_os_module_bind(lh_os_module_t *self, lh_ptr addr)
{
    return lh_os_shared_bind(lh_os_module_get_image(self), addr);
}

lh_bool_t
lh_os_module_bind_executable(lh_os_module_t *self)
{
    return lh_os_shared_bind_executable(lh_os_module_get_image(self));
}

lh_bool_t
lh_os_module_close(lh_os_module_t *self)
{
    /* The methods live inside the image: stop while it is still mapped. */
    lh_os_module_stop(self);
    const lh_bool_t ok = lh_os_shared_close(lh_os_module_get_image(self));

    /* The table normally lives in the image just released. */
    lh_os_module_set_ops(self, lh_null);
    return ok;
}

/* ── symbols ─────────────────────────────────────────────────────────────── */

lh_ptr
lh_os_module_get_sym(const lh_os_module_t *self, lh_str_cptr name)
{
    return lh_os_shared_get_sym(lh_os_module_get_image_as_const(self), name);
}

lh_bool_t
lh_os_module_has_sym(const lh_os_module_t *self, lh_str_cptr name)
{
    return lh_os_shared_has_sym(lh_os_module_get_image_as_const(self), name);
}
