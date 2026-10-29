/**
 * @file subscription.h
 * @brief Subscription state for synchronous indexed event delivery.
 *
 * The bus owns entries, names and subscription records. Payloads are borrowed
 * for synchronous delivery. Structural state is managed through the public
 * functions; low-level setters require the caller to maintain bus invariants.
 */

#ifndef LH_OS_MODULE_EVENT_SUBSCRIPTION_H
#define LH_OS_MODULE_EVENT_SUBSCRIPTION_H

#include <lh/list/node.h>
#include <lh/os/module/event/cb.h>
#include <lh/os/module/event/dispose/cb.h>
#include <lh/os/module/event/subscription/fields.h>

/** @brief Subscription state; use accessors rather than fields. */
struct lh_os_module_event_subscription
{
    lh_os_module_event_subscription_fields(lh_list_node_t, lh_list_node_t, lh_ptr,
                                           lh_os_module_event_cb, lh_os_module_event_dispose_cb,
                                           lh_ptr, lh_bool_t);
};
/** @copybrief lh_os_module_event_subscription */
typedef struct lh_os_module_event_subscription lh_os_module_event_subscription_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The link in the subscriber list of its event (level 1).
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
const lh_list_node_t *
lh_os_module_event_subscription_get_node(const lh_os_module_event_subscription_t *self);

/**
 * @brief The link in the subscriber list, to change in place (level 1).
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
lh_list_node_t *
lh_os_module_event_subscription_get_node_mut(lh_os_module_event_subscription_t *self);

/**
 * @brief The link in the list of removed records (level 1).
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
const lh_list_node_t *
lh_os_module_event_subscription_get_retired(const lh_os_module_event_subscription_t *self);

/**
 * @brief The link in the list of removed records, to change in place (level 1).
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
lh_list_node_t *
lh_os_module_event_subscription_get_retired_mut(lh_os_module_event_subscription_t *self);

/**
 * @brief The tag the subscription was made under (level 1).
 *
 * Compared by identity, never dereferenced; ::lh_os_module_event_bus_drop_owner removes by it.
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
lh_ptr
lh_os_module_event_subscription_get_owner(const lh_os_module_event_subscription_t *self);

/**
 * @brief Store the owner tag (level 1).
 *
 * A level-1 setter: nothing is checked.
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_subscription_set_owner(lh_os_module_event_subscription_t *self, lh_ptr value);

/**
 * @brief The handler called on every publication (level 1).
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
lh_os_module_event_cb
lh_os_module_event_subscription_get_callback(const lh_os_module_event_subscription_t *self);

/**
 * @brief Store the handler (level 1).
 *
 * A level-1 setter: ::lh_null is not refused here, and a publication would call it.
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_subscription_set_callback(lh_os_module_event_subscription_t *self,
                                             lh_os_module_event_cb value);

/**
 * @brief The call that releases the context, or ::lh_null for none (level 1).
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
lh_os_module_event_dispose_cb
lh_os_module_event_subscription_get_dispose(const lh_os_module_event_subscription_t *self);

/**
 * @brief Store the context release call (level 1).
 *
 * A level-1 setter: nothing is checked.
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_subscription_set_dispose(lh_os_module_event_subscription_t *self,
                                            lh_os_module_event_dispose_cb value);

/**
 * @brief The value passed to the handler (level 1).
 *
 * Owned by the subscription once insertion succeeded; released by the dispose call.
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
lh_ptr
lh_os_module_event_subscription_get_context(const lh_os_module_event_subscription_t *self);

/**
 * @brief Store the handler value (level 1).
 *
 * A level-1 setter: the old context is not released.
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_subscription_set_context(lh_os_module_event_subscription_t *self, lh_ptr value);

/**
 * @brief Whether the subscription was removed (level 1).
 *
 * A removed record is never called again, though it can stay
 * linked until the outermost publication returns.
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 */
lh_bool_t
lh_os_module_event_subscription_is_dead(const lh_os_module_event_subscription_t *self);

/**
 * @brief Store whether the subscription was removed (level 1).
 *
 * A level-1 setter: it does not unlink, count or retire the record; ::lh_os_module_event_bus_remove
 * does.
 *
 * @param self The subscription; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_subscription_set_dead(lh_os_module_event_subscription_t *self, lh_bool_t value);

/**
 * @brief The subscription a subscriber-list link belongs to (level 1).
 *
 * @param node The link, or ::lh_null.
 * @return The subscription, or ::lh_null for ::lh_null.
 */
lh_os_module_event_subscription_t *
lh_os_module_event_subscription_get_by_node(lh_list_node_t *node);

/**
 * @brief The subscription a removed-list link belongs to (level 1).
 *
 * @param node The link, or ::lh_null.
 * @return The subscription, or ::lh_null for ::lh_null.
 */
lh_os_module_event_subscription_t *
lh_os_module_event_subscription_get_by_retired(lh_list_node_t *node);

/**
 * @brief Initialize unlinked subscription state without allocating.
 * Context ownership is transferred only when insertion into a bus succeeds.
 */
lh_void
lh_os_module_event_subscription_init(lh_os_module_event_subscription_t *self, lh_ptr owner,
                                     lh_os_module_event_cb callback, lh_ptr context,
                                     lh_os_module_event_dispose_cb dispose);

/**
 * @brief Unlink the record, dispose its context if requested, then free it.
 * No active dispatch may still reference the record. Disposal must not reenter the bus.
 */
lh_void
lh_os_module_event_subscription_destroy(lh_os_module_event_subscription_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_EVENT_SUBSCRIPTION_H */
