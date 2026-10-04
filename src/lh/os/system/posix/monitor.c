/**
 * @file monitor.c
 * @brief X11 screens as monitors. One entry per screen of the default
 *        display. Core X11 only: no XRandR. The work size is the screen
 *        size, because the window-manager strut is not read here.
 */

#include <lh/bool.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/system/monitor.h>
#include <lh/os/system/posix/x11.h>

lh_os_system_posix_display_p
lh_os_system_posix_monitor_display(void)
{
    return XOpenDisplay(lh_null);
}

lh_int_t
lh_os_system_monitor_get_count(void)
{
    lh_os_system_posix_display_p display = lh_os_system_posix_monitor_display();
    lh_int_t count = 0;

    if (lh_null_eq(display))
    {
        return 0;
    }
    count = XScreenCount(display);
    (void)XCloseDisplay(display);
    if (count < 0)
    {
        return 0;
    }
    return count;
}

lh_int_t
lh_os_system_posix_monitor_span(lh_int_t index, lh_bool_t horizontal)
{
    lh_os_system_posix_display_p display = lh_os_system_posix_monitor_display();
    lh_int_t span = 0;

    if (lh_null_eq(display))
    {
        return 0;
    }
    if (index >= 0 && index < XScreenCount(display))
    {
        span = horizontal != lh_bool_false ? XDisplayWidth(display, index)
                                           : XDisplayHeight(display, index);
    }
    (void)XCloseDisplay(display);
    return span;
}

lh_int_t
lh_os_system_monitor_get_width(lh_int_t index)
{
    return lh_os_system_posix_monitor_span(index, lh_bool_true);
}

lh_int_t
lh_os_system_monitor_get_height(lh_int_t index)
{
    return lh_os_system_posix_monitor_span(index, lh_bool_false);
}

lh_int_t
lh_os_system_monitor_get_work_width(lh_int_t index)
{
    return lh_os_system_monitor_get_width(index);
}

lh_int_t
lh_os_system_monitor_get_work_height(lh_int_t index)
{
    return lh_os_system_monitor_get_height(index);
}
