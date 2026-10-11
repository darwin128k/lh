/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xproperty_event_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XPROPERTY_EVENT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XPROPERTY_EVENT_FIELDS_H

/**
 * @def lh_os_system_posix_xproperty_event_fields
 * @brief `XPropertyEvent`: a property of the window changed (`_NET_WM_STATE`, for maximize).
 */
#define lh_os_system_posix_xproperty_event_fields \
    lh_os_system_posix_xany_event_fields;                                                          \
    lh_os_system_posix_xid_t atom;                                                                 \
    unsigned long time;                                                                            \
    int state

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XPROPERTY_EVENT_FIELDS_H */
