/**
 * @file peer.h
 * @brief ::lh_http_limit_peer_t — what a server counts for one client address:
 *        its open connections and its connections in the current one-second window.
 *
 * A server keeps a fixed table of peers and never allocates for a new address:
 * ::lh_http_limit_peer_try_acquire takes a free slot, else the idle peer whose
 * window began first. An address over its limits is refused.
 */

#ifndef LH_HTTP_LIMIT_PEER_H
#define LH_HTTP_LIMIT_PEER_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/size.h>
#include <lh/core/void.h>

/** @brief The length of a rate window, in milliseconds. */
#define LH_HTTP_LIMIT_PEER_WINDOW_MS 1000

/** @brief One client address and its counters. */
struct lh_http_limit_peer
{
    lh_u32_t key;     /**< the address, as its owner keys it */
    lh_bool_t used;   /**< whether the slot holds an address */
    lh_u32_t active;  /**< connections of the address now open */
    lh_u32_t count;   /**< connections of the address in the current window */
    lh_u32_t pending; /**< open connections whose request has not arrived yet */
    lh_s64_t window;  /**< when the current window began, in milliseconds */
};
/** @copybrief lh_http_limit_peer */
typedef struct lh_http_limit_peer lh_http_limit_peer_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The address, as its owner keys it (level 1).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_http_limit_peer_get_key(const lh_http_limit_peer_t *self);

/**
 * @brief Store the address, as its owner keys it (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 * @param key The new value.
 */
lh_void
lh_http_limit_peer_set_key(lh_http_limit_peer_t *self, lh_u32_t key);

/**
 * @brief Whether the slot holds an address (level 1).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_http_limit_peer_is_used(const lh_http_limit_peer_t *self);

/**
 * @brief Store whether the slot holds an address (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 * @param used The new value.
 */
lh_void
lh_http_limit_peer_set_used(lh_http_limit_peer_t *self, lh_bool_t used);

/**
 * @brief Connections of the address now open (level 1).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_http_limit_peer_get_active(const lh_http_limit_peer_t *self);

/**
 * @brief Store connections of the address now open (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 * @param active The new value.
 */
lh_void
lh_http_limit_peer_set_active(lh_http_limit_peer_t *self, lh_u32_t active);

/**
 * @brief Connections of the address in the current window (level 1).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_http_limit_peer_get_count(const lh_http_limit_peer_t *self);

/**
 * @brief Store connections of the address in the current window (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 * @param count The new value.
 */
lh_void
lh_http_limit_peer_set_count(lh_http_limit_peer_t *self, lh_u32_t count);

/**
 * @brief Open connections whose request has not arrived yet (level 1).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_http_limit_peer_get_pending(const lh_http_limit_peer_t *self);

/**
 * @brief Store open connections whose request has not arrived yet (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 * @param pending The new value.
 */
lh_void
lh_http_limit_peer_set_pending(lh_http_limit_peer_t *self, lh_u32_t pending);

/**
 * @brief When the current window began, in milliseconds (level 1).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_http_limit_peer_get_window(const lh_http_limit_peer_t *self);

/**
 * @brief Store when the current window began, in milliseconds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 * @param window The new value.
 */
lh_void
lh_http_limit_peer_set_window(lh_http_limit_peer_t *self, lh_s64_t window);

/**
 * @brief Whether the slot may be given to another address at @p now: unused, or
 *        no connection open and its window over (level 2).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 * @param now  Milliseconds, on the clock the windows were taken on.
 */
lh_bool_t
lh_http_limit_peer_is_free(const lh_http_limit_peer_t *self, lh_s64_t now);

/**
 * @brief Make @p self the address @p key with its first connection open (level 2).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 * @param key  The address.
 * @param now  Milliseconds: the window begins.
 */
lh_void
lh_http_limit_peer_init(lh_http_limit_peer_t *self, lh_u32_t key, lh_s64_t now);

/**
 * @brief Close one connection of the address (level 2).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_http_limit_peer_release(lh_http_limit_peer_t *self);

/**
 * @brief Count one more open connection whose request has not arrived (level 2).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_http_limit_peer_add_pending(lh_http_limit_peer_t *self);

/**
 * @brief Count one connection less whose request has not arrived; none stays none
 *        (level 2).
 *
 * @param self The peer; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_http_limit_peer_drop_pending(lh_http_limit_peer_t *self);

/**
 * @brief Open one more connection of the address, unless it is over a limit
 *        (level 3).
 *
 * The window starts over when a whole one has passed.
 *
 * @param self       The peer; checked by ::lh_assert_runtime_ref.
 * @param now        Milliseconds.
 * @param per_ip     Connections the address may hold open at once.
 * @param per_second Connections the address may open in one window.
 * @return ::lh_bool_false when the address is at one of the limits.
 */
lh_bool_t
lh_http_limit_peer_try_enter(lh_http_limit_peer_t *self, lh_s64_t now, lh_u32_t per_ip,
                             lh_u32_t per_second);

/**
 * @brief Open a connection of @p key in a table of @p count peers (level 4).
 *
 * The address's own slot when it has one, else a free slot, else the idle peer
 * whose window began first.
 *
 * @param peers      The table; checked by ::lh_assert_runtime_ref.
 * @param count      Peers in @p peers.
 * @param key        The address.
 * @param now        Milliseconds.
 * @param per_ip     Connections the address may hold open at once.
 * @param per_second Connections the address may open in one window.
 * @param slot       Receives the slot of the address; checked by ::lh_assert_runtime_ref.
 * @return ::lh_bool_false when the address is over a limit or no slot is left;
 *         @p slot is untouched then.
 */
lh_bool_t
lh_http_limit_peer_try_acquire(lh_http_limit_peer_t *peers, lh_usize_t count, lh_u32_t key,
                               lh_s64_t now, lh_u32_t per_ip, lh_u32_t per_second,
                               lh_usize_t *slot);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_LIMIT_PEER_H */
