/**
 * @file subscription.h
 * @brief Subscription state for synchronous indexed event delivery.
 *
 * The bus owns entries, names and subscription records. Payloads are borrowed
 * for synchronous delivery. Structural state is managed through the public
 * functions; low-level setters require the caller to maintain bus invariants.
 */

#ifndef LH_EVENT_SUBSCRIPTION_H
#define LH_EVENT_SUBSCRIPTION_H

#include <lh/list.h>
#include <lh/event/cb.h>
#include <lh/event/dispose/cb.h>
#include <lh/event/subscription/fields.h>

/** @brief Subscription state; use accessors rather than fields. */
struct lh_event_subscription
{
    lh_event_subscription_fields(lh_list_node_t, lh_list_node_t, lh_ptr, lh_event_cb, lh_event_dispose_cb, lh_ptr, lh_bool_t);
};
/** @copybrief lh_event_subscription */
typedef struct lh_event_subscription lh_event_subscription_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Link in the event subscriber list. Level-1 borrowed member address; self must be nonnull. */
const lh_list_node_t *
lh_event_subscription_get_node(const lh_event_subscription_t *self);

/** @brief Link in the event subscriber list. Level-1 borrowed member address; self must be nonnull. */
lh_list_node_t *
lh_event_subscription_get_node_mut(lh_event_subscription_t *self);

/** @brief Link in the deferred disposal list. Level-1 borrowed member address; self must be nonnull. */
const lh_list_node_t *
lh_event_subscription_get_retired(const lh_event_subscription_t *self);

/** @brief Link in the deferred disposal list. Level-1 borrowed member address; self must be nonnull. */
lh_list_node_t *
lh_event_subscription_get_retired_mut(lh_event_subscription_t *self);

/** @brief Borrowed ownership tag, compared by identity. Level-1 read; self must be nonnull. */
lh_ptr
lh_event_subscription_get_owner(const lh_event_subscription_t *self);

/** @brief Store owner without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_subscription_set_owner(lh_event_subscription_t *self, lh_ptr value);

/** @brief Synchronous subscriber callback. Level-1 read; self must be nonnull. */
lh_event_cb
lh_event_subscription_get_callback(const lh_event_subscription_t *self);

/** @brief Store callback without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_subscription_set_callback(lh_event_subscription_t *self, lh_event_cb value);

/** @brief Optional context release callback. Level-1 read; self must be nonnull. */
lh_event_dispose_cb
lh_event_subscription_get_dispose(const lh_event_subscription_t *self);

/** @brief Store dispose without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_subscription_set_dispose(lh_event_subscription_t *self, lh_event_dispose_cb value);

/** @brief Subscriber context transferred on successful insertion. Level-1 read; self must be nonnull. */
lh_ptr
lh_event_subscription_get_context(const lh_event_subscription_t *self);

/** @brief Store context without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_subscription_set_context(lh_event_subscription_t *self, lh_ptr value);

/** @brief Removed flag; removed records are not dispatched. Level-1 read; self must be nonnull. */
lh_bool_t
lh_event_subscription_get_dead(const lh_event_subscription_t *self);

/** @brief Store dead without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_subscription_set_dead(lh_event_subscription_t *self, lh_bool_t value);

/** @brief Recover a subscription from its event link; null maps to null (level 1). */
lh_event_subscription_t *
lh_event_subscription_get_by_node(lh_list_node_t *node);

/** @brief Recover a subscription from its deferred-disposal link; null maps to null (level 1). */
lh_event_subscription_t *
lh_event_subscription_get_by_retired(lh_list_node_t *node);

/**
 * @brief Initialize unlinked subscription state without allocating.
 * Context ownership is transferred only when insertion into a bus succeeds.
 */
lh_void
lh_event_subscription_init(lh_event_subscription_t *self, lh_ptr owner, lh_event_cb callback,
                           lh_ptr context, lh_event_dispose_cb dispose);

/**
 * @brief Unlink the record, dispose its context if requested, then free it.
 * No active dispatch may still reference the record. Disposal must not reenter the bus.
 */
lh_void
lh_event_subscription_destroy(lh_event_subscription_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EVENT_SUBSCRIPTION_H */
