#include <lh/os/shared.h>
#include <lh/assert.h>
#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/fs/path.h>
#include <lh/os/system/fs/path.h>
#include <lh/os/system/shared.h>
#include <lh/str.h>
#include <lh/util/addr.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_ATTRIBUTE_STATIC
lh_fs_path_t *
lh_os_shared_get_path(lh_os_shared_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

const lh_fs_path_t *
lh_os_shared_get_path_as_const(const lh_os_shared_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->path);
}

lh_os_system_shared_handle_t
lh_os_shared_get_handle(const lh_os_shared_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

LH_ATTRIBUTE_STATIC
void
lh_os_shared_set_handle(lh_os_shared_t *self, lh_os_system_shared_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

lh_bool_t
lh_os_shared_is_open(const lh_os_shared_t *self)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_os_shared_get_handle(self)));
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

void
lh_os_shared_init(lh_os_shared_t *self)
{
    lh_fs_path_init(lh_os_shared_get_path(self));
    lh_os_shared_set_handle(self, LH_OS_SYSTEM_SHARED_HANDLE_INVALID);
}

void
lh_os_shared_deinit(lh_os_shared_t *self)
{
    lh_os_shared_close(self);
    lh_fs_path_deinit(lh_os_shared_get_path(self));
}

/* ── open / close ────────────────────────────────────────────────────────── */

/* Open, bind and bind_executable all need a closed image. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_shared_is_free(const lh_os_shared_t *self)
{
    if (lh_os_shared_is_open(self))
    {
        lh_os_set_last_error_lit(lh_os_error_code_already_open, "image already open");
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_bool_t
lh_os_shared_open(lh_os_shared_t *self, const lh_fs_path_t *path)
{
    lh_str_t buf;

    if (!lh_os_shared_is_free(self))
    {
        return lh_bool_false;
    }
    lh_str_cptr const cstr = lh_os_fs_path_to_cstr(path, lh_addr_of(buf));
    const lh_os_system_shared_handle_t handle =
        lh_null_eq(cstr) ? LH_OS_SYSTEM_SHARED_HANDLE_INVALID : lh_os_system_shared_open(cstr);
    lh_str_deinit(lh_addr_of(buf));
    if (lh_null_eq(handle))
    {
        return lh_bool_false;
    }
    lh_os_shared_set_handle(self, handle);
    lh_fs_path_assign(lh_os_shared_get_path(self), path);
    return lh_bool_true;
}

/* Keep a handle the OS just handed out for an already-loaded image; its path
   comes from the OS too. On failure the handle's reference is given back. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_shared_adopt(lh_os_shared_t *self, lh_os_system_shared_handle_t handle)
{
    lh_str_t text;

    if (lh_null_eq(handle))
    {
        return lh_bool_false;
    }
    lh_str_init(lh_addr_of(text));
    const lh_bool_t ok = lh_os_system_shared_get_path(handle, lh_addr_of(text));
    if (ok)
    {
        lh_fs_path_set(lh_os_shared_get_path(self), lh_str_as_view(lh_addr_of(text)),
                       lh_os_system_fs_path_style_native());
        lh_os_shared_set_handle(self, handle);
    }
    else
    {
        lh_os_system_shared_close(handle);
    }
    lh_str_deinit(lh_addr_of(text));
    return ok;
}

lh_bool_t
lh_os_shared_bind(lh_os_shared_t *self, lh_ptr addr)
{
    if (!lh_os_shared_is_free(self))
    {
        return lh_bool_false;
    }
    return lh_os_shared_adopt(self, lh_os_system_shared_get_by_addr(addr));
}

lh_bool_t
lh_os_shared_bind_executable(lh_os_shared_t *self)
{
    if (!lh_os_shared_is_free(self))
    {
        return lh_bool_false;
    }
    return lh_os_shared_adopt(self, lh_os_system_shared_get_executable());
}

lh_bool_t
lh_os_shared_close(lh_os_shared_t *self)
{
    if (!lh_os_shared_is_open(self))
    {
        return lh_bool_true;
    }
    const lh_bool_t ok = lh_os_system_shared_close(lh_os_shared_get_handle(self));

    lh_os_shared_set_handle(self, LH_OS_SYSTEM_SHARED_HANDLE_INVALID);
    return ok;
}

/* ── symbols ─────────────────────────────────────────────────────────────── */

lh_ptr
lh_os_shared_get_sym(const lh_os_shared_t *self, lh_str_cptr name)
{
    if (!lh_os_shared_is_open(self))
    {
        lh_os_set_last_error_lit(lh_os_error_code_not_open, "image not open");
        return lh_null;
    }
    return lh_os_system_shared_get_sym(lh_os_shared_get_handle(self), name);
}

lh_bool_t
lh_os_shared_has_sym(const lh_os_shared_t *self, lh_str_cptr name)
{
    return lh_cast_static(lh_bool_t, lh_null_ne(lh_os_shared_get_sym(self, name)));
}
