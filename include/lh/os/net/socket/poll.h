/**
 * @file poll.h
 * @brief One borrowed socket and its readiness interests/results.
 */

#ifndef LH_OS_NET_SOCKET_POLL_H
#define LH_OS_NET_SOCKET_POLL_H

#include <lh/os/system/net/socket/handle.h>
#include <lh/os/net/socket/poll/event.h>
#include <lh/os/net/socket/poll/fields.h>

/**
 * @def LH_OS_NET_SOCKET_POLL_MAX
 * @brief Maximum number of entries accepted by a socket poll call.
 */
#define LH_OS_NET_SOCKET_POLL_MAX 512U

/**
 * @brief Borrowed socket handle with separate input and output event masks.
 *
 * events accepts read/write only. ready is reset on every poll call. An invalid
 * handle is ignored. Read readiness also includes EOF: receive must still tell
 * data, pending and EOF apart. Error/hangup bits reflect native backend reports;
 * Windows select reports EOF as readable rather than a separate hangup bit.
 */
struct lh_os_net_socket_poll
{
    /** Members: ::lh_os_net_socket_poll_fields. */
    lh_os_net_socket_poll_fields(lh_os_system_net_socket_handle_t, lh_uint_t);
};

/** @copybrief lh_os_net_socket_poll */
typedef struct lh_os_net_socket_poll lh_os_net_socket_poll_t;

#endif /* LH_OS_NET_SOCKET_POLL_H */
