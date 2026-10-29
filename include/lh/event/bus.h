/**
 * @file bus.h
 * @brief Bus state for synchronous indexed event delivery.
 *
 * The bus owns entries, names and subscription records. Payloads are borrowed
 * for synchronous delivery. Structural state is managed through the public
 * functions; low-level setters require the caller to maintain bus invariants.
 */

#ifndef LH_EVENT_BUS_H
#define LH_EVENT_BUS_H

#include <lh/array.h>
#include <lh/event/entry.h>
#include <lh/event/subscription.h>
#include <lh/event/id.h>
#include <lh/event/match/cb.h>
#include <lh/event/bus/fields.h>

/** @brief Bus state; use accessors rather than fields. */
struct lh_event_bus
{
    lh_event_bus_fields(lh_array_t, lh_list_t, lh_usize_t, lh_usize_t);
};
/** @copybrief lh_event_bus */
typedef struct lh_event_bus lh_event_bus_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Owned table of stable entry pointers, indexed by ID minus one. Level-1 borrowed member
 * address; self must be nonnull. */
const lh_array_t *
lh_event_bus_get_entries(const lh_event_bus_t *self);

/** @brief Owned table of stable entry pointers, indexed by ID minus one. Level-1 borrowed member
 * address; self must be nonnull. */
lh_array_t *
lh_event_bus_get_entries_mut(lh_event_bus_t *self);

/** @brief Subscriptions awaiting outermost-dispatch cleanup. Level-1 borrowed member address; self
 * must be nonnull. */
const lh_list_t *
lh_event_bus_get_retired(const lh_event_bus_t *self);

/** @brief Subscriptions awaiting outermost-dispatch cleanup. Level-1 borrowed member address; self
 * must be nonnull. */
lh_list_t *
lh_event_bus_get_retired_mut(lh_event_bus_t *self);

/** @brief Current nested publication depth. Level-1 read; self must be nonnull. */
lh_usize_t
lh_event_bus_get_depth(const lh_event_bus_t *self);

/** @brief Store depth without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_bus_set_depth(lh_event_bus_t *self, lh_usize_t value);

/** @brief Number of active subscriptions across all events. Level-1 read; self must be nonnull. */
lh_usize_t
lh_event_bus_get_count(const lh_event_bus_t *self);

/** @brief Store count without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_bus_set_count(lh_event_bus_t *self, lh_usize_t value);

/**
 * @brief Initialize an empty bus without allocating.
 * All calls require external serialization on one thread. The runtime allocator
 * and its context must remain compatible until deinit, as for ::lh_array_t.
 */
lh_void
lh_event_bus_init(lh_event_bus_t *self);

/**
 * @brief O(1) entry lookup; null for zero or an out-of-range ID.
 * Returned entry and its name stay owned by the bus until deinit.
 * An in-range ID from another bus is a caller error and cannot be distinguished.
 */
lh_event_entry_t *
lh_event_bus_get_entry(const lh_event_bus_t *self, lh_event_id_t id);

/** @brief Dispose deferred records when depth is zero; otherwise no-op. */
lh_void
lh_event_bus_collect(lh_event_bus_t *self);

/**
 * @brief Find a registered nonempty NUL-terminated name without allocating.
 * Returns zero for null, empty or unknown names. Linear in registered event count;
 * no subscription is examined. The input text is borrowed only for this call.
 */
lh_event_id_t
lh_event_bus_find_id(const lh_event_bus_t *self, const lh_char_t *name);

/** @brief First live subscription of id, or null. Borrowed until removal/clear/deinit. */
lh_event_subscription_t *
lh_event_bus_get_first(const lh_event_bus_t *self, lh_event_id_t id);

/**
 * @brief Next live subscription, or null. sub must still belong to id.
 * A removed record stays valid only while an enclosing publication is active.
 */
lh_event_subscription_t *
lh_event_bus_get_next(const lh_event_bus_t *self, lh_event_id_t id,
                      const lh_event_subscription_t *sub);

/**
 * @brief Append one subscription, or return null for invalid ID/callback or OOM.
 * Each successful call creates a distinct record, even for identical arguments.
 * The bus owns the record; optional dispose assumes context ownership on success
 * only. A newly appended record can run in an ongoing publication of this event.
 */
lh_event_subscription_t *
lh_event_bus_try_subscribe(lh_event_bus_t *self, lh_event_id_t id, lh_ptr owner,
                           lh_event_cb callback, lh_ptr context, lh_event_dispose_cb dispose);

/**
 * @brief Remove a record belonging to self; already removed is a no-op.
 * Delivery stops immediately. Unlinking, disposal and freeing wait until the
 * outermost publication returns, so self-removal and nested publish are safe.
 * Outside publication the record is destroyed before this call returns.
 */
lh_void
lh_event_bus_remove(lh_event_bus_t *self, lh_event_subscription_t *sub);

/**
 * @brief Intern a name and return its stable, one-based ID, or zero on failure.
 * Copies new names. Null/empty names, allocation failure and exhausted index space
 * return zero; existing IDs and subscriptions remain valid. Registration can be
 * nested inside publication. IDs survive clear and owner removal, until deinit.
 */
lh_event_id_t
lh_event_bus_try_get_id(lh_event_bus_t *self, const lh_char_t *name);

/** @brief Checked ::lh_event_bus_try_subscribe; runtime failure for a null result. */
lh_event_subscription_t *
lh_event_bus_subscribe(lh_event_bus_t *self, lh_event_id_t id, lh_ptr owner, lh_event_cb callback,
                       lh_ptr context, lh_event_dispose_cb dispose);

/**
 * @brief Synchronously dispatch only id's ordered subscribers; no name comparisons.
 * Invalid IDs or no handlers return continue. Payload is borrowed and shared,
 * never copied/retained. Returns the strongest result; stop ends the chain.
 * Callbacks may register, subscribe, remove, clear and publish recursively.
 * Deinit and disposal-callback reentry are forbidden during dispatch.
 */
lh_event_result_t
lh_event_bus_publish(lh_event_bus_t *self, lh_event_id_t id, lh_ptr payload);

/** @brief Remove all subscriptions tagged owner (including a null tag); keep IDs. */
lh_void
lh_event_bus_drop_owner(lh_event_bus_t *self, lh_ptr owner);

/** @brief Remove all subscriptions; retain the name registry and every issued ID. */
lh_void
lh_event_bus_clear(lh_event_bus_t *self);

/**
 * @brief Find the first live record of id tagged owner and accepted by match.
 * A null match accepts every context. Returns a borrowed record, or null if
 * absent/invalid. The matcher receives the record context and key and must not
 * mutate the bus. Search visits only this event's subscribers and allocates nothing.
 */
lh_event_subscription_t *
lh_event_bus_find_subscription(const lh_event_bus_t *self, lh_event_id_t id, lh_ptr owner,
                               lh_event_match_cb match, lh_ptr key);

/**
 * @brief Resolve name only while *cache is zero, storing and returning the ID.
 * cache must be nonnull and initialized to zero, or hold this name's ID from
 * this bus lifetime. Failure leaves zero so a later call can retry. This is an
 * externally serialized scalar cache, with the same lifetime as the bus IDs.
 */
lh_event_id_t
lh_event_bus_try_get_cached_id(lh_event_bus_t *self, lh_event_id_t *cache, const lh_char_t *name);

/** @brief Checked ::lh_event_bus_try_get_id; runtime failure if the result is zero. */
lh_event_id_t
lh_event_bus_get_id(lh_event_bus_t *self, const lh_char_t *name);

/**
 * @brief Clear subscriptions and free the registry. Invalidates all issued IDs.
 * Requires no active publish. The object may subsequently be initialized again;
 * IDs from the previous lifetime must not be used in the new one.
 */
lh_void
lh_event_bus_deinit(lh_event_bus_t *self);

/** @brief Checked cached lookup; runtime failure if the result remains zero. */
lh_event_id_t
lh_event_bus_get_cached_id(lh_event_bus_t *self, lh_event_id_t *cache, const lh_char_t *name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EVENT_BUS_H */
