/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xmotion_event_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XMOTION_EVENT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XMOTION_EVENT_FIELDS_H

/**
 * @def lh_os_system_posix_xmotion_event_fields
 * @brief `XMotionEvent`: `MotionNotify`.
 */
#define lh_os_system_posix_xmotion_event_fields                                                    \
    lh_os_system_posix_xany_event_fields;                                                          \
    lh_os_system_posix_xid_t root;                                                                 \
    lh_os_system_posix_xid_t subwindow;                                                            \
    unsigned long time;                                                                            \
    int x;                                                                                         \
    int y;                                                                                         \
    int x_root;                                                                                    \
    int y_root;                                                                                    \
    unsigned int state;                                                                            \
    char is_hint;                                                                                  \
    int same_screen

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XMOTION_EVENT_FIELDS_H */
