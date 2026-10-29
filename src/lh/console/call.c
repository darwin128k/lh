/**
 * @file call.c
 * @brief Accessors and printing of ::lh_console_call_t.
 */

#include <lh/console/call.h>

#include <lh/assert/runtime.h>
#include <lh/util/return.h>
#include <lh/str.h>
#include <lh/char/map.h>

const lh_char_t *
lh_console_call_get_scope(const lh_console_call_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scope;
}

lh_void
lh_console_call_set_scope(lh_console_call_t *self, const lh_char_t *scope)
{
    lh_assert_runtime_ref(self);
    self->scope = scope;
}

const lh_char_t *
lh_console_call_get_name(const lh_console_call_t *self)
{
    lh_assert_runtime_ref(self);
    return self->name;
}

lh_void
lh_console_call_set_name(lh_console_call_t *self, const lh_char_t *name)
{
    lh_assert_runtime_ref(self);
    self->name = name;
}

lh_usize_t
lh_console_call_get_argc(const lh_console_call_t *self)
{
    lh_assert_runtime_ref(self);
    return self->argc;
}

lh_void
lh_console_call_set_argc(lh_console_call_t *self, lh_usize_t argc)
{
    lh_assert_runtime_ref(self);
    self->argc = argc;
}

const lh_char_t *const *
lh_console_call_get_argv(const lh_console_call_t *self)
{
    lh_assert_runtime_ref(self);
    return self->argv;
}

lh_void
lh_console_call_set_argv(lh_console_call_t *self, const lh_char_t *const *argv)
{
    lh_assert_runtime_ref(self);
    self->argv = argv;
}

lh_console_sink_cb
lh_console_call_get_sink_fn(const lh_console_call_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sink;
}

lh_void
lh_console_call_set_sink_fn(lh_console_call_t *self, lh_console_sink_cb sink)
{
    lh_assert_runtime_ref(self);
    self->sink = sink;
}

lh_ptr
lh_console_call_get_out(const lh_console_call_t *self)
{
    lh_assert_runtime_ref(self);
    return self->out;
}

lh_void
lh_console_call_set_out(lh_console_call_t *self, lh_ptr out)
{
    lh_assert_runtime_ref(self);
    self->out = out;
}

lh_void
lh_console_call_init(lh_console_call_t *self, const lh_char_t *scope, const lh_char_t *name,
                     lh_usize_t argc, const lh_char_t *const *argv, lh_console_sink_cb sink,
                     lh_ptr out)
{
    lh_console_call_set_scope(self, scope);
    lh_console_call_set_name(self, name);
    lh_console_call_set_argc(self, lh_ptr_is_null(argv) ? 0U : argc);
    lh_console_call_set_argv(self, argv);
    lh_console_call_set_sink_fn(self, sink);
    lh_console_call_set_out(self, out);
}

const lh_char_t *
lh_console_call_get_arg(const lh_console_call_t *self, lh_usize_t index)
{
    const lh_char_t *arg;

    lh_return_if(lh_math_ge(index, lh_console_call_get_argc(self)), "");
    arg = lh_console_call_get_argv(self)[index];
    return lh_ptr_is_null(arg) ? "" : arg;
}

lh_void
lh_console_call_print(const lh_console_call_t *self, const lh_char_t *text)
{
    lh_console_sink_cb sink = lh_console_call_get_sink_fn(self);
    const lh_char_t newline[2] = {lh_char_map_lf, lh_char_map_nul};

    lh_return_if(lh_ptr_is_null(sink) || lh_ptr_is_null(text));
    sink(lh_console_call_get_out(self), text);
    sink(lh_console_call_get_out(self), newline);
}

lh_void
lh_console_call_printf_v(const lh_console_call_t *self, const lh_char_t *fmt, va_list args)
{
    lh_str_t line;

    lh_return_if(lh_ptr_is_null(lh_console_call_get_sink_fn(self)) || lh_ptr_is_null(fmt));
    lh_str_init(lh_addr_of(line));
    (void)lh_str_format_v(lh_addr_of(line), fmt, args);
    lh_console_call_print(self, lh_str_get_data(lh_addr_of(line)));
    lh_str_deinit(lh_addr_of(line));
}

lh_void
lh_console_call_printf(const lh_console_call_t *self, const lh_char_t *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    lh_console_call_printf_v(self, fmt, args);
    va_end(args);
}
