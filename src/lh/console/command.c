/**
 * @file command.c
 * @brief Accessors of ::lh_console_command_t.
 */

#include <lh/console/command.h>

#include <lh/assert/runtime.h>

const lh_char_t *
lh_console_command_get_name(const lh_console_command_t *self)
{
    lh_assert_runtime_ref(self);
    return self->name;
}

lh_void
lh_console_command_set_name(lh_console_command_t *self, const lh_char_t *name)
{
    lh_assert_runtime_ref(self);
    self->name = name;
}

const lh_char_t *
lh_console_command_get_usage(const lh_console_command_t *self)
{
    lh_assert_runtime_ref(self);
    return self->usage;
}

lh_void
lh_console_command_set_usage(lh_console_command_t *self, const lh_char_t *usage)
{
    lh_assert_runtime_ref(self);
    self->usage = usage;
}

const lh_char_t *
lh_console_command_get_help(const lh_console_command_t *self)
{
    lh_assert_runtime_ref(self);
    return self->help;
}

lh_void
lh_console_command_set_help(lh_console_command_t *self, const lh_char_t *help)
{
    lh_assert_runtime_ref(self);
    self->help = help;
}

lh_console_cb
lh_console_command_get_fn(const lh_console_command_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fn;
}

lh_void
lh_console_command_set_fn(lh_console_command_t *self, lh_console_cb fn)
{
    lh_assert_runtime_ref(self);
    self->fn = fn;
}

lh_ptr
lh_console_command_get_user(const lh_console_command_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_console_command_set_user(lh_console_command_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}
