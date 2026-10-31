/**
 * @file app.c
 * @brief Portable ::lh_os_app_t — windows, logical timers, message pump.
 */

#include <lh/assert/runtime.h>
#include <lh/os/app.h>
#include <lh/os/tick.h>
#include <lh/os/window.h>
#include <lh/timer.h>

#include <lh/os/system/window.h>

lh_void
lh_os_app_init(lh_os_app_t *self)
{
    lh_list_init(lh_os_app_get_windows_mut(self));
    lh_timer_group_init(lh_os_app_get_timers(self));
    lh_os_app_set_quit(self, lh_bool_false);
}

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
lh_os_app_quit(lh_os_app_t *self)
{
    lh_os_app_set_quit(self, lh_bool_true);
    lh_os_system_window_post_quit();
}

lh_os_window_t *
lh_os_app_get_first_window(const lh_os_app_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_list_entry(lh_os_window_t, link, lh_list_get_first(lh_os_app_get_windows(self)));
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
    lh_os_app_set_quit(self, lh_bool_false);
}

lh_os_window_t *
lh_os_app_get_window(const lh_os_app_t *self)
{
    return lh_os_app_get_first_window(self);
}

/** @copydoc lh_os_app_get_next_window(const lh_os_app_t *, const struct lh_os_window *) */
lh_os_window_t *
lh_os_app_get_next_window(const lh_os_app_t *self, const lh_os_window_t *window)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(window);
    return lh_list_entry(
        lh_os_window_t, link,
        lh_list_get_next(lh_os_app_get_windows(self), lh_os_window_get_link(window)));
}

lh_void
lh_os_app_run(lh_os_app_t *self)
{
    while (!lh_os_app_is_quit(self) && !lh_list_is_empty(lh_os_app_get_windows(self)))
    {
        lh_tick_t now = lh_os_tick_ms();

        lh_timer_group_handler(lh_os_app_get_timers(self), now);
        lh_tick_t wait = lh_timer_group_until_next(lh_os_app_get_timers(self), lh_os_tick_ms());
        lh_os_system_window_pump_result_t result = lh_os_system_window_pump_wait(wait);

        if (result == lh_os_system_window_pump_quit)
        {
            lh_os_app_set_quit(self, lh_bool_true);
            break;
        }
    }
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
lh_os_app_set_quit(lh_os_app_t *self, lh_bool_t quit)
{
    lh_assert_runtime_ref(self);
    self->quit = quit;
}
