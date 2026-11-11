/**
 * @file handler.c
 * @brief ::lh_os_module_hook_handler_t: its fields.
 */

#include <lh/os/module/hook/handler.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>

lh_os_module_hook_cb
lh_os_module_hook_handler_get_fn(const lh_os_module_hook_handler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fn;
}

lh_void
lh_os_module_hook_handler_set_fn(lh_os_module_hook_handler_t *self, lh_os_module_hook_cb fn)
{
    lh_assert_runtime_ref(self);
    self->fn = fn;
}

lh_ptr
lh_os_module_hook_handler_get_context(const lh_os_module_hook_handler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_os_module_hook_handler_set_context(lh_os_module_hook_handler_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_ptr
lh_os_module_hook_handler_get_owner(const lh_os_module_hook_handler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

lh_void
lh_os_module_hook_handler_set_owner(lh_os_module_hook_handler_t *self, lh_ptr owner)
{
    lh_assert_runtime_ref(self);
    self->owner = owner;
}

const lh_char_t *
lh_os_module_hook_handler_get_tag(const lh_os_module_hook_handler_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tag;
}

lh_void
lh_os_module_hook_handler_set_tag(lh_os_module_hook_handler_t *self, const lh_char_t *tag)
{
    lh_assert_runtime_ref(self);
    self->tag = tag;
}

lh_bool_t
lh_os_module_hook_handler_is(const lh_os_module_hook_handler_t *self, lh_ptr owner,
                             lh_os_module_hook_cb fn, lh_ptr context)
{
    return (lh_math_eq(lh_os_module_hook_handler_get_fn(self), fn) &&
            lh_math_eq(lh_os_module_hook_handler_get_context(self), context) &&
            lh_math_eq(lh_os_module_hook_handler_get_owner(self), owner))
               ? lh_bool_true
               : lh_bool_false;
}
