/**
 * @file fields.h
 * @brief Member fields of ::lh_os_app_t.
 */

#ifndef LH_OS_APP_FIELDS_H
#define LH_OS_APP_FIELDS_H

#include <lh/bool.h>
#include <lh/list.h>

/**
 * @def lh_os_app_fields(list_type)
 * @brief Top-level windows and a quit flag for the message pump.
 *
 * The app does not own window memory — only the links.
 *
 * @param list_type Type of the window list (::lh_list_t).
 */
#define lh_os_app_fields(list_type)                                                                \
    list_type windows;                                                                              \
    lh_bool_t quit

#endif /* LH_OS_APP_FIELDS_H */
