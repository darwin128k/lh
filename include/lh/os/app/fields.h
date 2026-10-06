/**
 * @file fields.h
 * @brief Member fields of ::lh_os_app_t.
 */

#ifndef LH_OS_APP_FIELDS_H
#define LH_OS_APP_FIELDS_H

#include <lh/bool.h>
#include <lh/list.h>
#include <lh/timer/group.h>

/**
 * @def lh_os_app_fields(list_type, timers_type)
 * @brief Top-level windows (index 0 is main), logical timers, and quit.
 *
 * Windows and timers are added from outside. The app does not own their
 * memory — only the links / group head.
 *
 * @param list_type   Type of the window list (::lh_list_t).
 * @param timers_type Type of the timer group (::lh_timer_group_t).
 */
#define lh_os_app_fields(list_type, timers_type)                                                   \
    list_type windows;                                                                              \
    timers_type timers;                                                                             \
    lh_bool_t quit

#endif /* LH_OS_APP_FIELDS_H */
