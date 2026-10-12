/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xexpose_event_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XEXPOSE_EVENT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XEXPOSE_EVENT_FIELDS_H

/**
 * @def lh_os_system_posix_xexpose_event_fields
 * @brief `XExposeEvent`: part of the window the server lost and wants painted again.
 */
#define lh_os_system_posix_xexpose_event_fields \
    lh_os_system_posix_xany_event_fields;                                                          \
    int x;                                                                                         \
    int y;                                                                                         \
    int width;                                                                                     \
    int height;                                                                                    \
    int count

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XEXPOSE_EVENT_FIELDS_H */
