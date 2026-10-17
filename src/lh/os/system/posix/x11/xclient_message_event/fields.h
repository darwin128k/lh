/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xclient_message_event_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XCLIENT_MESSAGE_EVENT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XCLIENT_MESSAGE_EVENT_FIELDS_H

/**
 * @def lh_os_system_posix_xclient_message_event_fields
 * @brief `XClientMessageEvent`: a message from another client, the window manager's
 *        `WM_DELETE_WINDOW` among them. Only the `long` view of the data is mirrored.
 */
#define lh_os_system_posix_xclient_message_event_fields                                            \
    lh_os_system_posix_xany_event_fields;                                                          \
    lh_os_system_posix_xid_t message_type;                                                         \
    int format;                                                                                    \
    long data[5]

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XCLIENT_MESSAGE_EVENT_FIELDS_H */
