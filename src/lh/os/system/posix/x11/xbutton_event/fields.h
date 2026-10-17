/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xbutton_event_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XBUTTON_EVENT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XBUTTON_EVENT_FIELDS_H

/**
 * @def lh_os_system_posix_xbutton_event_fields
 * @brief `XButtonEvent`: `ButtonPress` and `ButtonRelease`; buttons 4 to 7 are the wheel.
 */
#define lh_os_system_posix_xbutton_event_fields                                                    \
    lh_os_system_posix_xany_event_fields;                                                          \
    lh_os_system_posix_xid_t root;                                                                 \
    lh_os_system_posix_xid_t subwindow;                                                            \
    unsigned long time;                                                                            \
    int x;                                                                                         \
    int y;                                                                                         \
    int x_root;                                                                                    \
    int y_root;                                                                                    \
    unsigned int state;                                                                            \
    unsigned int button;                                                                           \
    int same_screen

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XBUTTON_EVENT_FIELDS_H */
