/**
 * @file event.h
 * @brief Socket readiness bits used by ::lh_os_net_socket_poll_t.
 */

#ifndef LH_OS_NET_SOCKET_POLL_EVENT_H
#define LH_OS_NET_SOCKET_POLL_EVENT_H

/** @brief Readiness interests and results; error/hangup are output-only. */
enum lh_os_net_socket_poll_event
{
    /** Input/read readiness, including EOF. */
    lh_os_net_socket_poll_read = 1U,
    /** Output/write readiness. */
    lh_os_net_socket_poll_write = 2U,
    /** Native error or exceptional condition (output only). */
    lh_os_net_socket_poll_error = 4U,
    /** Native peer hangup report (output only). */
    lh_os_net_socket_poll_hangup = 8U
};

/** @copybrief lh_os_net_socket_poll_event */
typedef enum lh_os_net_socket_poll_event lh_os_net_socket_poll_event_t;

#endif /* LH_OS_NET_SOCKET_POLL_EVENT_H */
