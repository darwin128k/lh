/**
 * @file app.c
 * @brief Portable ::lh_os_app_t — the windows, the step, the logical timers, and
 *        the loop over the contract ::lh_os_app_backend_t.
 */

#include <lh/os/app.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/timer.h>
#include <lh/core/util/return.h>
#include <lh/os/window.h>

lh_timer_group_t *
lh_os_app_get_timers(lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->timers);
}

lh_bool_t
lh_os_app_is_quit(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->quit;
}

lh_void
lh_os_app_set_quit(lh_os_app_t *self, lh_bool_t quit)
{
    lh_assert_runtime_ref(self);
    self->quit = quit;
}

lh_os_app_step_cb
lh_os_app_get_step_fn(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step_fn;
}

lh_void
lh_os_app_set_step_fn(lh_os_app_t *self, lh_os_app_step_cb fn)
{
    lh_assert_runtime_ref(self);
    self->step_fn = fn;
}

lh_ptr
lh_os_app_get_step_context(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step_context;
}

lh_void
lh_os_app_set_step_context(lh_os_app_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->step_context = context;
}

const lh_os_app_backend_t *
lh_os_app_get_backend(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->backend;
}

lh_void
lh_os_app_set_backend(lh_os_app_t *self, const lh_os_app_backend_t *backend)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
}

lh_ptr
lh_os_app_get_backend_context(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->backend_context;
}

lh_void
lh_os_app_set_backend_context(lh_os_app_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->backend_context = context;
}

const lh_os_window_backend_t *
lh_os_app_get_window_backend(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->window_backend;
}

lh_void
lh_os_app_set_window_backend(lh_os_app_t *self, const lh_os_window_backend_t *backend)
{
    lh_assert_runtime_ref(self);
    self->window_backend = backend;
}

lh_ptr
lh_os_app_get_window_backend_context(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return self->window_backend_context;
}

lh_void
lh_os_app_set_window_backend_context(lh_os_app_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->window_backend_context = context;
}

const lh_list_t *
lh_os_app_get_windows(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->windows);
}

lh_list_t *
lh_os_app_get_windows_mut(lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->windows);
}

lh_void
lh_os_app_quit(lh_os_app_t *self)
{
    lh_os_app_set_quit(self, lh_bool_true);
    const lh_os_app_backend_t *backend = lh_os_app_get_backend(self);

    lh_return_if(lh_null_eq(backend));
    lh_os_app_backend_wake_fn *wake = lh_os_app_backend_get_wake(backend);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, wake)));
    wake(lh_os_app_get_backend_context(self));
}

lh_void
lh_os_app_install_backend(lh_os_app_t *self, const lh_os_app_backend_t *backend, lh_ptr context)
{
    lh_os_app_set_backend(self, backend);
    lh_os_app_set_backend_context(self, context);
}

lh_void
lh_os_app_install_window_backend(lh_os_app_t *self, const lh_os_window_backend_t *backend,
                                 lh_ptr context)
{
    lh_os_app_set_window_backend(self, backend);
    lh_os_app_set_window_backend_context(self, context);
}

lh_void
lh_os_app_step(lh_os_app_t *self, lh_tick_t now)
{
    lh_timer_group_handler(lh_os_app_get_timers(self), now);
    lh_os_app_step_cb fn = lh_os_app_get_step_fn(self);

    lh_return_if(lh_null_eq(fn));
    fn(lh_os_app_get_step_context(self));
}

lh_os_window_t *
lh_os_app_get_first_window(const lh_os_app_t *self)
{
    return lh_list_entry(lh_os_window_t, link, lh_list_get_first(lh_os_app_get_windows(self)));
}

/** @copydoc lh_os_app_get_next_window(const lh_os_app_t *, const struct lh_os_window *) */
lh_os_window_t *
lh_os_app_get_next_window(const lh_os_app_t *self, const lh_os_window_t *window)
{
    lh_assert_runtime_ref(window);
    return lh_list_entry(
        lh_os_window_t, link,
        lh_list_get_next(lh_os_app_get_windows(self), lh_os_window_get_link(window)));
}

lh_bool_t
lh_os_app_is_running(const lh_os_app_t *self)
{
    lh_return_if(lh_os_app_is_quit(self), lh_bool_false);
    lh_return_if(lh_null_eq(lh_os_app_get_window_backend(self)), lh_bool_true);
    return lh_cast_static(lh_bool_t, !lh_list_is_empty(lh_os_app_get_windows(self)));
}

lh_void
lh_os_app_init(lh_os_app_t *self)
{
    lh_list_init(lh_os_app_get_windows_mut(self));
    lh_timer_group_init(lh_os_app_get_timers(self));
    lh_os_app_set_step_fn(self, lh_null);
    lh_os_app_set_step_context(self, lh_null);
    lh_os_app_install_backend(self, lh_null, lh_null);
    lh_os_app_install_window_backend(self, lh_null, lh_null);
    lh_os_app_set_quit(self, lh_bool_false);
}

lh_void
lh_os_app_deinit(lh_os_app_t *self)
{
    for (;;)
    {
        lh_os_window_t *window = lh_os_app_get_first_window(self);

        if (lh_null_eq(window))
        {
            break;
        }
        lh_os_window_close(window);
    }
    lh_timer_group_deinit(lh_os_app_get_timers(self));
    lh_os_app_set_step_fn(self, lh_null);
    lh_os_app_set_step_context(self, lh_null);
    lh_os_app_install_backend(self, lh_null, lh_null);
    lh_os_app_install_window_backend(self, lh_null, lh_null);
    lh_os_app_set_quit(self, lh_bool_false);
}

lh_os_window_t *
lh_os_app_get_window(const lh_os_app_t *self)
{
    return lh_os_app_get_first_window(self);
}

lh_void
lh_os_app_run(lh_os_app_t *self)
{
    const lh_os_app_backend_t *backend = lh_os_app_get_backend(self);

    /* Nothing is installed by default: an application that never said what its loop
       runs on is told so here, not left spinning or blocked on a wait nobody fills. */
    lh_runtime_check_ifn(lh_null_ne(backend) && lh_os_app_backend_is_complete(backend),
                         lh_runtime_error_code_invalid_argument,
                         "lh_os_app_run: the application has no loop backend");
    lh_timer_clock_fn *now = lh_os_app_backend_get_now(backend);
    lh_os_app_backend_wait_fn *wait = lh_os_app_backend_get_wait(backend);
    lh_ptr context = lh_os_app_get_backend_context(self);

    while (lh_os_app_is_running(self))
    {
        lh_os_app_step(self, now(context));
        /* The step may have quit, or closed the last window: no wait after that. */
        if (!lh_os_app_is_running(self))
        {
            break;
        }
        if (wait(context, lh_timer_group_until_next(lh_os_app_get_timers(self), now(context))))
        {
            lh_os_app_set_quit(self, lh_bool_true);
        }
    }
}
