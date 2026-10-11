/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xany_event_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XANY_EVENT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XANY_EVENT_FIELDS_H

/**
 * @def lh_os_system_posix_xany_event_fields
 * @brief `XAnyEvent`: the head every event starts with. `window` is the window the event is
 *        reported on (`event` in the structure-notify events).
 */
#define lh_os_system_posix_xany_event_fields \
    int type;                                                                                      \
    unsigned long serial;                                                                          \
    int send_event;                                                                                \
    lh_os_system_posix_display_t *display;                                                         \
    lh_os_system_posix_xid_t window

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XANY_EVENT_FIELDS_H */
