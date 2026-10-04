/**
 * @file monitor.c
 * @brief Public wrappers over ::lh_os_system_monitor_get_count and its siblings.
 */

#include <lh/os/monitor.h>

lh_int_t
lh_os_monitor_get_count(void)
{
    return lh_os_system_monitor_get_count();
}

lh_int_t
lh_os_monitor_get_width(lh_int_t index)
{
    return lh_os_system_monitor_get_width(index);
}

lh_int_t
lh_os_monitor_get_height(lh_int_t index)
{
    return lh_os_system_monitor_get_height(index);
}

lh_int_t
lh_os_monitor_get_work_width(lh_int_t index)
{
    return lh_os_system_monitor_get_work_width(index);
}

lh_int_t
lh_os_monitor_get_work_height(lh_int_t index)
{
    return lh_os_system_monitor_get_work_height(index);
}
