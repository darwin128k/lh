/**
 * @file fields.h
 * @brief Member fields of ::lh_os_window_t.
 */

#ifndef LH_OS_WINDOW_FIELDS_H
#define LH_OS_WINDOW_FIELDS_H

#include <lh/bool.h>
#include <lh/list.h>
#include <lh/list/node.h>

/**
 * @def lh_os_window_fields(handle_type, app_type, window_type)
 * @brief Native handle, owning app or parent, children, and list link.
 *
 * Top-level windows sit in the app's window list (`app` set, `parent` null).
 * Modal children sit in a parent's `children` list (`parent` set, `app` null).
 * Neither owns the child's memory.
 *
 * @param handle_type Type of `handle` (::lh_os_system_window_handle_t).
 * @param app_type    Type of the owning app pointer (::lh_os_app_t).
 * @param window_type Type of the parent / child window pointer.
 */
#define lh_os_window_fields(handle_type, app_type, window_type)                                     \
    handle_type handle;                                                                             \
    app_type *app;                                                                                  \
    window_type *parent;                                                                            \
    lh_list_t children;                                                                             \
    lh_list_node_t link;                                                                            \
    lh_bool_t modal

#endif /* LH_OS_WINDOW_FIELDS_H */
