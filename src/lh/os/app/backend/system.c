/**
 * @file system.c
 * @brief Implementation of `lh/os/app/backend/system.h`.
 */

#include <lh/os/app/backend/system.h>

#include <lh/core/attribute/unused.h>
#include <lh/os/system/window.h>
#include <lh/os/tick.h>

lh_tick_t
lh_os_app_backend_system_now(lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(context);
    return lh_os_tick_ms();
}

lh_bool_t
lh_os_app_backend_system_wait(lh_ptr context, lh_tick_t timeout)
{
    LH_ATTRIBUTE_UNUSED(context);
    return lh_cast_static(lh_bool_t,
                          lh_os_system_window_pump_wait(timeout) == lh_os_system_window_pump_quit);
}

lh_void
lh_os_app_backend_system_wake(lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_os_system_window_post_quit();
}

lh_void
lh_os_app_backend_system_init(lh_os_app_backend_t *table)
{
    lh_os_app_backend_init(table);
    lh_os_app_backend_set_now(table, lh_os_app_backend_system_now);
    lh_os_app_backend_set_wait(table, lh_os_app_backend_system_wait);
    lh_os_app_backend_set_wake(table, lh_os_app_backend_system_wake);
}
