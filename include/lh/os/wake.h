/**
 * @file wake.h
 * @brief Non-blocking, pollable wake signal with coalescing notifications.
 *
 * Initialize before opening; never copy a live object. Signal may run on other
 * threads while one consumer polls/resets. Close only after callers and waiters
 * have stopped. Notifications are hints to recheck caller-owned predicates;
 * reset can consume concurrent notifications. Network initialization is required
 * on Windows, whose socket-pair backend uses a private loopback connection.
 */

#ifndef LH_OS_WAKE_H
#define LH_OS_WAKE_H

#include <lh/os/system/net/socket/handle.h>
#include <lh/os/wake/fields.h>
#include <lh/bool.h>
#include <lh/void.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/compiler/extern/c.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/wake.h requires LH_LIBRARY_OPTION_OS"
#endif

/** @brief Owner of the two non-blocking endpoints of a wake signal. */
struct lh_os_wake
{
    /** Members: ::lh_os_wake_fields. */
    lh_os_wake_fields(lh_os_system_net_socket_handle_t);
};

/** @copybrief lh_os_wake */
typedef struct lh_os_wake lh_os_wake_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Initialize empty endpoints without acquiring resources.
 * @param self Non-null uninitialized storage; do not reinitialize a live object.
 */
lh_void
lh_os_wake_init(lh_os_wake_t *self);

/**
 * @brief Open an initialized, empty wake signal.
 * @param self Object to open; unchanged on failure.
 * @param timeout_ms Maximum wait for each loopback setup phase on Windows;
 *                   ignored by the native POSIX socket-pair backend.
 * @return ::lh_bool_true on success; ::lh_bool_false with LH/native last error on failure.
 */
lh_bool_t
lh_os_wake_open(lh_os_wake_t *self, lh_u32_t timeout_ms);

/**
 * @brief Publish a notification without allocating or waiting.
 * @param self Non-null open signal; may be shared by producers.
 * @return ::lh_bool_true when queued/already pending; ::lh_bool_false on failure.
 */
lh_bool_t
lh_os_wake_signal(lh_os_wake_t *self);

/**
 * @brief Consume pending notifications without waiting.
 *
 * Only one consumer may reset. Signals can be coalesced or arrive after reset
 * because byte streams may still contain in-flight notifications. Recheck the
 * caller's predicate; this object is not a counted semaphore.
 *
 * @param self Non-null open signal, exclusively reset by the consumer.
 * @return ::lh_bool_true after draining available data; ::lh_bool_false on failure/EOF.
 */
lh_bool_t
lh_os_wake_reset(lh_os_wake_t *self);

/**
 * @brief Borrow the read endpoint for socket polling; ownership stays with self.
 * @param self Non-null initialized object.
 * @return Read handle, or ::LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID while empty.
 */
lh_os_system_net_socket_handle_t
lh_os_wake_get_handle(const lh_os_wake_t *self);

/**
 * @brief Close both endpoints; safe on an initialized empty object.
 * @param self Non-null initialized object; all producers and waiters must have stopped.
 */
lh_void
lh_os_wake_close(lh_os_wake_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WAKE_H */
