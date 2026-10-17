/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xconfigure_event_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XCONFIGURE_EVENT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XCONFIGURE_EVENT_FIELDS_H

/**
 * @def lh_os_system_posix_xconfigure_event_fields
 * @brief `XConfigureEvent`: the window moved or changed size. The head's `window` is the
 *        window reporting it (`event`); `configured` is the one that changed.
 */
#define lh_os_system_posix_xconfigure_event_fields                                                 \
    lh_os_system_posix_xany_event_fields;                                                          \
    lh_os_system_posix_xid_t configured;                                                           \
    int x;                                                                                         \
    int y;                                                                                         \
    int width;                                                                                     \
    int height;                                                                                    \
    int border_width;                                                                              \
    lh_os_system_posix_xid_t above;                                                                \
    int override_redirect

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XCONFIGURE_EVENT_FIELDS_H */
