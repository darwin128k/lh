/**
 * @file fields.h
 * @brief Member fields of ::lh_os_net_socket_poll_t.
 */

#ifndef LH_OS_NET_SOCKET_POLL_FIELDS_H
#define LH_OS_NET_SOCKET_POLL_FIELDS_H

/**
 * @def lh_os_net_socket_poll_fields(handle_type, events_type)
 * @brief Borrowed handle, requested interests and returned readiness mask.
 * @param handle_type Native socket handle type.
 * @param events_type Unsigned event-mask type.
 */
#define lh_os_net_socket_poll_fields(handle_type, events_type)                                     \
    handle_type handle;                                                                            \
    events_type events;                                                                            \
    events_type ready

#endif /* LH_OS_NET_SOCKET_POLL_FIELDS_H */
