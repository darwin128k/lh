/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xfocus_change_event_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XFOCUS_CHANGE_EVENT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XFOCUS_CHANGE_EVENT_FIELDS_H

/**
 * @def lh_os_system_posix_xfocus_change_event_fields
 * @brief `XFocusChangeEvent`: `FocusIn` / `FocusOut`. `mode` says whether a grab
 *        caused it, `detail` how the focus moved relative to this window.
 */
#define lh_os_system_posix_xfocus_change_event_fields                                              \
    lh_os_system_posix_xany_event_fields;                                                          \
    int mode;                                                                                      \
    int detail

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XFOCUS_CHANGE_EVENT_FIELDS_H */
