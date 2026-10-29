/**
 * @file entry.c
 * @brief Accessors and lifetime of ::lh_console_entry_t.
 */

#include <lh/console/entry.h>

#include <lh/assert/runtime.h>
#include <lh/util/return.h>
#include <lh/runtime/allocator.h>

const lh_list_node_t *
lh_console_entry_get_node(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_list_node_t *
lh_console_entry_get_node_mut(lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->node);
}

lh_ptr
lh_console_entry_get_owner(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

lh_void
lh_console_entry_set_owner(lh_console_entry_t *self, lh_ptr owner)
{
    lh_assert_runtime_ref(self);
    self->owner = owner;
}

const lh_str_t *
lh_console_entry_get_scope(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->scope);
}

lh_str_t *
lh_console_entry_get_scope_mut(lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->scope);
}

const lh_str_t *
lh_console_entry_get_name(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_str_t *
lh_console_entry_get_name_mut(lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

const lh_str_t *
lh_console_entry_get_usage(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->usage);
}

lh_str_t *
lh_console_entry_get_usage_mut(lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->usage);
}

const lh_str_t *
lh_console_entry_get_help(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->help);
}

lh_str_t *
lh_console_entry_get_help_mut(lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->help);
}

lh_console_cb
lh_console_entry_get_fn(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fn;
}

lh_void
lh_console_entry_set_fn(lh_console_entry_t *self, lh_console_cb fn)
{
    lh_assert_runtime_ref(self);
    self->fn = fn;
}

lh_ptr
lh_console_entry_get_user(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_console_entry_set_user(lh_console_entry_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

lh_bool_t
lh_console_entry_is_dead(const lh_console_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dead;
}

lh_void
lh_console_entry_set_dead(lh_console_entry_t *self, lh_bool_t dead)
{
    lh_assert_runtime_ref(self);
    self->dead = dead;
}

lh_console_entry_t *
lh_console_entry_try_create(lh_ptr owner, const lh_char_t *scope,
                            const lh_console_command_t *command)
{
    lh_console_entry_t *self = lh_ptr_rcast(
        lh_console_entry_t, lh_runtime_allocator_try_alloc_zero(1U, sizeof(lh_ptr_deref(self))));
    lh_str_view_t text;

    lh_return_if(lh_ptr_is_null(self), lh_null);
    lh_list_node_init(lh_console_entry_get_node_mut(self));
    lh_console_entry_set_owner(self, owner);
    lh_str_view_init(lh_addr_of(text), scope);
    lh_str_init_by_view(lh_console_entry_get_scope_mut(self), text);
    lh_str_view_init(lh_addr_of(text), lh_console_command_get_name(command));
    lh_str_init_by_view(lh_console_entry_get_name_mut(self), text);
    lh_str_view_init(lh_addr_of(text), lh_ptr_is_null(lh_console_command_get_usage(command))
                                           ? ""
                                           : lh_console_command_get_usage(command));
    lh_str_init_by_view(lh_console_entry_get_usage_mut(self), text);
    lh_str_view_init(lh_addr_of(text), lh_ptr_is_null(lh_console_command_get_help(command))
                                           ? ""
                                           : lh_console_command_get_help(command));
    lh_str_init_by_view(lh_console_entry_get_help_mut(self), text);
    lh_console_entry_set_fn(self, lh_console_command_get_fn(command));
    lh_console_entry_set_user(self, lh_console_command_get_user(command));
    lh_console_entry_set_dead(self, lh_bool_false);
    return self;
}

lh_void
lh_console_entry_destroy(lh_console_entry_t *self)
{
    lh_str_deinit(lh_console_entry_get_scope_mut(self));
    lh_str_deinit(lh_console_entry_get_name_mut(self));
    lh_str_deinit(lh_console_entry_get_usage_mut(self));
    lh_str_deinit(lh_console_entry_get_help_mut(self));
    lh_runtime_allocator_free(self);
}

lh_bool_t
lh_console_entry_is_named(const lh_console_entry_t *self, const lh_char_t *scope,
                          const lh_char_t *name)
{
    lh_return_ifn(
        lh_str_ptr_equals(lh_str_get_data(lh_console_entry_get_scope(self)), scope, lh_bool_true),
        lh_bool_false);
    return lh_ptr_is_null(name) ||
                   lh_str_ptr_equals(lh_str_get_data(lh_console_entry_get_name(self)), name,
                                     lh_bool_true)
               ? lh_bool_true
               : lh_bool_false;
}

lh_bool_t
lh_console_entry_is_same(const lh_console_entry_t *self, lh_ptr owner,
                         const lh_console_command_t *command)
{
    return lh_math_eq(lh_console_entry_get_owner(self), owner) &&
                   lh_math_eq(lh_console_entry_get_fn(self), lh_console_command_get_fn(command)) &&
                   lh_math_eq(lh_console_entry_get_user(self), lh_console_command_get_user(command))
               ? lh_bool_true
               : lh_bool_false;
}
