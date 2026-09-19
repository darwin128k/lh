#include <lh/os/shared/module.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os.h>
#include <lh/os/shared.h>
#include <lh/runtime/error.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/void.h>

lh_vector_t *
lh_os_shared_module_get_modules(lh_os_shared_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->modules);
}

const lh_vector_t *
lh_os_shared_module_get_modules_as_const(const lh_os_shared_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->modules);
}

lh_os_shared_handle_t *
lh_os_shared_module_get_handle(lh_os_shared_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

const lh_os_shared_handle_t *
lh_os_shared_module_get_handle_as_const(const lh_os_shared_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handle);
}

lh_bool_t *
lh_os_shared_module_get_owned(lh_os_shared_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->owned);
}

const lh_bool_t *
lh_os_shared_module_get_owned_as_const(const lh_os_shared_module_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->owned);
}

lh_bool_t
lh_os_shared_module_get_path(const lh_os_shared_module_t *self, lh_os_fs_path_t *out)
{
    lh_assert_runtime_ref(out);
    if (!lh_os_shared_module_is_loaded(self))
    {
        lh_os_fs_path_clear(out);
        lh_os_set_last_error(lh_os_error_code_not_loaded, lh_os_error_desc_lit("not loaded"));
        return lh_bool_false;
    }
    return lh_os_shared_get_path_of(lh_ptr_deref(lh_os_shared_module_get_handle_as_const(self)),
                                    out);
}

void
lh_os_shared_module_init(lh_os_shared_module_t *self, lh_usize_t type_size)
{
    lh_assert_runtime_if(type_size < sizeof(lh_os_shared_module_t),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    lh_ptr_deref(lh_os_shared_module_get_handle(self)) = lh_null;
    lh_ptr_deref(lh_os_shared_module_get_owned(self)) = lh_bool_false;
    lh_vector_init(lh_os_shared_module_get_modules(self), type_size);
}

void
lh_os_shared_module_deinit(lh_os_shared_module_t *self)
{
    (void)lh_os_shared_module_unload(self);
    lh_vector_deinit(lh_os_shared_module_get_modules(self));
}

lh_bool_t
lh_os_shared_module_is_loaded(const lh_os_shared_module_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_null_ne(lh_ptr_deref(lh_os_shared_module_get_handle_as_const(self))));
}

lh_bool_t
lh_os_shared_module_open(lh_os_shared_module_t *self, const lh_os_fs_path_t *path)
{
    lh_os_shared_handle_t handle;

    if (lh_os_shared_module_is_loaded(self))
    {
        lh_os_set_last_error(lh_os_error_code_already_loaded, lh_os_error_desc_lit("already loaded"));
        return lh_bool_false;
    }
    handle = lh_os_shared_open(path);
    if (lh_null_eq(handle))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(lh_os_shared_module_get_handle(self)) = handle;
    lh_ptr_deref(lh_os_shared_module_get_owned(self)) = lh_bool_true;
    return lh_bool_true;
}

lh_bool_t
lh_os_shared_module_bind(lh_os_shared_module_t *self, lh_ptr addr)
{
    lh_os_shared_handle_t handle;

    if (lh_os_shared_module_is_loaded(self))
    {
        lh_os_set_last_error(lh_os_error_code_already_loaded, lh_os_error_desc_lit("already loaded"));
        return lh_bool_false;
    }
    handle = lh_os_shared_of_addr(addr);
    if (lh_null_eq(handle))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(lh_os_shared_module_get_handle(self)) = handle;
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    lh_ptr_deref(lh_os_shared_module_get_owned(self)) = lh_bool_false;
#else
    lh_ptr_deref(lh_os_shared_module_get_owned(self)) = lh_bool_true;
#endif
    return lh_bool_true;
}

void
lh_os_shared_module_unload_all(lh_os_shared_module_t *self)
{
    lh_vector_t *modules;
    lh_uindex_t i;

    modules = lh_os_shared_module_get_modules(self);
    i = lh_vector_get_size(modules);
    while (i != 0U)
    {
        --i;
        lh_os_shared_module_deinit(lh_os_shared_module_get(self, i));
    }
    lh_vector_clear(modules);
}

lh_bool_t
lh_os_shared_module_close(lh_os_shared_module_t *self)
{
    lh_os_shared_handle_t *handle = lh_os_shared_module_get_handle(self);
    lh_bool_t *owned = lh_os_shared_module_get_owned(self);

    if (lh_null_eq(lh_ptr_deref(handle)))
    {
        return lh_bool_true;
    }
    if (lh_ptr_deref(owned))
    {
        if (!lh_os_shared_close(lh_ptr_deref(handle)))
        {
            return lh_bool_false;
        }
    }
    lh_ptr_deref(handle) = lh_null;
    lh_ptr_deref(owned) = lh_bool_false;
    return lh_bool_true;
}

lh_bool_t
lh_os_shared_module_unload(lh_os_shared_module_t *self)
{
    lh_os_shared_module_unload_all(self);
    return lh_os_shared_module_close(self);
}

void
lh_os_shared_module_push(lh_os_shared_module_t *self, const lh_os_shared_module_t *child)
{
    lh_assert_runtime_ref(child);
    lh_vector_push_back(lh_os_shared_module_get_modules(self), lh_ptr_cast(lh_void, child));
}

lh_usize_t
lh_os_shared_module_get_loaded(const lh_os_shared_module_t *self)
{
    return lh_vector_get_size(lh_os_shared_module_get_modules_as_const(self));
}

lh_os_shared_module_t *
lh_os_shared_module_get(const lh_os_shared_module_t *self, lh_uindex_t index)
{
    const lh_vector_t *modules = lh_os_shared_module_get_modules_as_const(self);

    if (!lh_vector_is_valid_index(modules, index))
    {
        return lh_null;
    }
    return lh_ptr_cast(lh_os_shared_module_t, lh_vector_get_ptr(modules, index));
}

lh_ptr
lh_os_shared_module_get_sym(const lh_os_shared_module_t *self, lh_str_cptr name)
{
    if (!lh_os_shared_module_is_loaded(self))
    {
        lh_os_set_last_error(lh_os_error_code_not_loaded, lh_os_error_desc_lit("not loaded"));
        return lh_null;
    }
    return lh_os_shared_get_sym(lh_ptr_deref(lh_os_shared_module_get_handle_as_const(self)), name);
}

lh_bool_t
lh_os_shared_module_has_sym(const lh_os_shared_module_t *self, lh_str_cptr name)
{
    if (!lh_os_shared_module_is_loaded(self))
    {
        lh_os_set_last_error(lh_os_error_code_not_loaded, lh_os_error_desc_lit("not loaded"));
        return lh_bool_false;
    }
    return lh_os_shared_has_sym(lh_ptr_deref(lh_os_shared_module_get_handle_as_const(self)), name);
}
