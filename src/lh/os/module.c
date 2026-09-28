#include <lh/os/module.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/path.h>
#include <lh/os/loader.h>
#include <lh/os/system/fs/path.h>
#include <lh/os/system/shared.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/error.h>
#include <lh/str.h>
#include <lh/util/addr.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_ATTRIBUTE_STATIC
lh_fs_path_t *
lh_os_module_get_path(lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

const lh_fs_path_t *
lh_os_module_get_path_as_const(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

lh_os_system_shared_handle_t
lh_os_module_get_handle(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_module_is_owned(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owned;
}

/* The handle and whether close must release it only ever change together. */
LH_ATTRIBUTE_STATIC
void
lh_os_module_set_handle(lh_os_module_t *self, lh_os_system_shared_handle_t handle, lh_bool_t owned)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
    self->owned = owned;
}

lh_bool_t
lh_os_module_is_loaded(const lh_os_module_t *self)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_os_module_get_handle(self)));
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

lh_os_module_t *
lh_os_module_get_parent(const lh_os_module_t *self)
{
    lh_assert_runtime_ref(self);
    return self->parent;
}

void
lh_os_module_set_parent(lh_os_module_t *self, lh_os_module_t *parent)
{
    lh_assert_runtime_ref(self);
    self->parent = parent;
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
    lh_fs_path_init(lh_os_module_get_path(self));
    lh_os_module_set_handle(self, LH_OS_SYSTEM_SHARED_HANDLE_INVALID, lh_bool_false);
    lh_os_module_set_started(self, lh_bool_false);
    lh_os_module_set_ops(self, lh_null);
    lh_os_module_set_data(self, lh_null);
    lh_os_module_set_parent(self, lh_null);
    lh_os_module_set_loader(self, lh_null);
}

void
lh_os_module_deinit(lh_os_module_t *self)
{
    lh_os_module_close(self);
    lh_fs_path_deinit(lh_os_module_get_path(self));
}

/* ── privilege ───────────────────────────────────────────────────────────── */

lh_os_loader_t *
lh_os_module_grant_loader(lh_os_module_t *self, lh_str_cptr entry)
{
    lh_os_loader_t *loader;

    if (lh_null_ne(lh_os_module_get_loader(self)))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_already_open, lh_os_error_desc_lit("loader already granted")));
        return lh_null;
    }
    loader = lh_ptr_cast(lh_os_loader_t, lh_runtime_allocator_alloc(sizeof(*loader)));
    if (lh_null_eq(loader))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_out_of_memory, lh_os_error_desc_lit("out of memory")));
        return lh_null;
    }
    lh_os_loader_init(loader, self, entry);
    lh_os_module_set_loader(self, loader);
    return loader;
}

/* Children go before their parent: they may still use what it provides. */
LH_ATTRIBUTE_STATIC
void
lh_os_module_revoke_loader(lh_os_module_t *self)
{
    lh_os_loader_t *loader;

    loader = lh_os_module_get_loader(self);
    if (lh_null_eq(loader))
    {
        return;
    }
    lh_os_loader_deinit(loader);
    lh_runtime_allocator_free(loader);
    lh_os_module_set_loader(self, lh_null);
}

/* ── lifecycle ───────────────────────────────────────────────────────────── */

lh_bool_t
lh_os_module_start(lh_os_module_t *self)
{
    const lh_os_module_ops_t *ops;

    if (lh_os_module_is_started(self))
    {
        return lh_bool_true;
    }
    ops = lh_os_module_get_ops(self);
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
    const lh_os_module_ops_t *ops;

    lh_os_module_revoke_loader(self);
    if (!lh_os_module_is_started(self))
    {
        return;
    }
    ops = lh_os_module_get_ops(self);
    if (lh_null_ne(ops) && lh_null_ne(ops->stop))
    {
        ops->stop(self);
    }
    lh_os_module_set_started(self, lh_bool_false);
}

/* ── image ───────────────────────────────────────────────────────────────── */

/* Open and bind both need a module without an image. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_module_is_free(const lh_os_module_t *self)
{
    if (lh_os_module_is_loaded(self))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_already_open, lh_os_error_desc_lit("already loaded")));
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_os_module_open(lh_os_module_t *self, const lh_fs_path_t *path)
{
    lh_str_t buf;
    lh_str_cptr cstr;
    lh_os_system_shared_handle_t handle;

    if (!lh_os_module_is_free(self))
    {
        return lh_bool_false;
    }
    cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));
    handle = lh_null_eq(cstr) ? LH_OS_SYSTEM_SHARED_HANDLE_INVALID : lh_os_system_shared_open(cstr);
    lh_str_deinit(lh_addr_of(buf));
    if (lh_null_eq(handle))
    {
        return lh_bool_false;
    }
    lh_os_module_set_handle(self, handle, lh_bool_true);
    lh_fs_path_assign(lh_os_module_get_path(self), path);
    return lh_bool_true;
}

lh_bool_t
lh_os_module_bind(lh_os_module_t *self, lh_ptr addr)
{
    lh_os_system_shared_handle_t handle;
    lh_bool_t owned;
    lh_str_t text;
    lh_bool_t ok;

    if (!lh_os_module_is_free(self))
    {
        return lh_bool_false;
    }
    handle = lh_os_system_shared_of_addr(addr, lh_addr_of(owned));
    if (lh_null_eq(handle))
    {
        return lh_bool_false;
    }
    lh_str_init(lh_addr_of(text));
    ok = lh_os_system_shared_path(handle, lh_addr_of(text));
    if (ok)
    {
        lh_fs_path_set(lh_os_module_get_path(self), lh_str_as_view(lh_addr_of(text)),
                       lh_os_system_fs_path_style_native());
        lh_os_module_set_handle(self, handle, owned);
    }
    else if (owned)
    {
        lh_os_system_shared_close(handle);
    }
    lh_str_deinit(lh_addr_of(text));
    return ok;
}

lh_bool_t
lh_os_module_close(lh_os_module_t *self)
{
    lh_bool_t ok;

    /* The methods live inside the image: stop while it is still mapped. */
    lh_os_module_stop(self);
    ok = lh_bool_true;
    if (lh_os_module_is_loaded(self) && lh_os_module_is_owned(self))
    {
        ok = lh_os_system_shared_close(lh_os_module_get_handle(self));
    }
    lh_os_module_set_handle(self, LH_OS_SYSTEM_SHARED_HANDLE_INVALID, lh_bool_false);
    /* The table normally lives in the image just released. */
    lh_os_module_set_ops(self, lh_null);
    return ok;
}

/* ── symbols ─────────────────────────────────────────────────────────────── */

lh_ptr
lh_os_module_get_sym(const lh_os_module_t *self, lh_str_cptr name)
{
    if (!lh_os_module_is_loaded(self))
    {
        lh_os_set_last_error(lh_os_error_make(lh_os_error_code_not_open, lh_os_error_desc_lit("not loaded")));
        return lh_null;
    }
    return lh_os_system_shared_sym(lh_os_module_get_handle(self), name);
}

lh_bool_t
lh_os_module_has_sym(const lh_os_module_t *self, lh_str_cptr name)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_os_module_get_sym(self, name)));
}
