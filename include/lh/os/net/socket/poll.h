/**
 * @file poll.h
 * @brief One borrowed socket and its readiness interests/results.
 */

#ifndef LH_OS_NET_SOCKET_POLL_H
#define LH_OS_NET_SOCKET_POLL_H

#include <lh/core/bool.h>
#include <lh/core/void.h>
#include <lh/core/compiler/extern/c.h>
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

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The borrowed socket handle (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_os_system_net_socket_handle_t
lh_os_net_socket_poll_get_handle(const lh_os_net_socket_poll_t *self);

/**
 * @brief Store the borrowed socket handle (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_os_net_socket_poll_set_handle(lh_os_net_socket_poll_t *self,
                                 lh_os_system_net_socket_handle_t value);

/**
 * @brief The interests asked for (read, write) (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_uint_t
lh_os_net_socket_poll_get_events(const lh_os_net_socket_poll_t *self);

/**
 * @brief Store the interests asked for (read, write) (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_os_net_socket_poll_set_events(lh_os_net_socket_poll_t *self, lh_uint_t value);

/**
 * @brief The readiness the last poll found (level 1).
 * @param self Must not be null.
 * @return The value.
 */
lh_uint_t
lh_os_net_socket_poll_get_ready(const lh_os_net_socket_poll_t *self);

/**
 * @brief Store the readiness the last poll found (level 1).
 * @param self  Must not be null.
 * @param value The new value.
 */
lh_void
lh_os_net_socket_poll_set_ready(lh_os_net_socket_poll_t *self, lh_uint_t value);

/**
 * @brief Fill @p self: watch @p handle for @p events, nothing ready yet.
 * @param self   Entry; must not be null.
 * @param handle Borrowed socket handle, or the invalid handle to skip the entry.
 * @param events ::lh_os_net_socket_poll_read and/or ::lh_os_net_socket_poll_write.
 */
lh_void
lh_os_net_socket_poll_init(lh_os_net_socket_poll_t *self, lh_os_system_net_socket_handle_t handle,
                           lh_uint_t events);

/**
 * @brief Fill @p self as an entry the poll skips: no socket, no interest.
 *
 * ::lh_os_net_socket_poll_init with the invalid handle and no events. A slot of
 * a fixed poll array whose connection is gone stays in place this way, so the
 * slots keep their indices.
 *
 * @param self Entry; must not be null.
 */
lh_void
lh_os_net_socket_poll_init_empty(lh_os_net_socket_poll_t *self);

/**
 * @brief Add readiness @p bits to what @p self reports (backends, after a poll).
 * @param self Entry; must not be null.
 * @param bits ::lh_os_net_socket_poll_event_t bits.
 */
lh_void
lh_os_net_socket_poll_add_ready(lh_os_net_socket_poll_t *self, lh_uint_t bits);

/**
 * @brief Whether the last poll found @p self readable (data or EOF).
 * @param self Entry; must not be null.
 * @return ::lh_bool_true when ::lh_os_net_socket_poll_read is in the readiness.
 */
lh_bool_t
lh_os_net_socket_poll_is_readable(const lh_os_net_socket_poll_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_NET_SOCKET_POLL_H */
