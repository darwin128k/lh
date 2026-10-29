/**
 * @file bus.h
 * @brief Bus state for synchronous indexed event delivery.
 *
 * The bus owns entries, names and subscription records. Payloads are borrowed
 * for synchronous delivery. Structural state is managed through the public
 * functions; low-level setters require the caller to maintain bus invariants.
 */

#ifndef LH_OS_MODULE_EVENT_BUS_H
#define LH_OS_MODULE_EVENT_BUS_H

#include <lh/array.h>
#include <lh/os/module/event/entry.h>
#include <lh/os/module/event/subscription.h>
#include <lh/os/module/event/id.h>
#include <lh/os/module/event/match/cb.h>
#include <lh/os/module/event/bus/fields.h>

/** @brief Bus state; use accessors rather than fields. */
struct lh_os_module_event_bus
{
    lh_os_module_event_bus_fields(lh_array_t, lh_list_t, lh_usize_t, lh_usize_t);
};
/** @copybrief lh_os_module_event_bus */
typedef struct lh_os_module_event_bus lh_os_module_event_bus_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The table of entry pointers, indexed by ID minus one (level 1).
 *
 * The bus owns the table and the entries it points to.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 */
const lh_array_t *
lh_os_module_event_bus_get_entries(const lh_os_module_event_bus_t *self);

/**
 * @brief The table of entry pointers, to change in place (level 1).
 *
 * Changing it bypasses the registry: IDs already issued must keep
 * pointing at their entries.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 */
lh_array_t *
lh_os_module_event_bus_get_entries_mut(lh_os_module_event_bus_t *self);

/**
 * @brief The subscriptions removed during a publication (level 1).
 *
 * They are unlinked, disposed and freed when the outermost publication
 * returns (::lh_os_module_event_bus_collect).
 *
 * @param self The bus; must not be ::lh_null (not checked).
 */
const lh_list_t *
lh_os_module_event_bus_get_retired(const lh_os_module_event_bus_t *self);

/**
 * @brief The removed subscriptions, to change in place (level 1).
 *
 * @param self The bus; must not be ::lh_null (not checked).
 */
lh_list_t *
lh_os_module_event_bus_get_retired_mut(lh_os_module_event_bus_t *self);

/**
 * @brief How many publications are running, nested (level 1).
 *
 * Removal is deferred while it is above zero.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 */
lh_usize_t
lh_os_module_event_bus_get_depth(const lh_os_module_event_bus_t *self);

/**
 * @brief Store the publication depth (level 1).
 *
 * A level-1 setter: nothing is checked and nothing is collected.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_bus_set_depth(lh_os_module_event_bus_t *self, lh_usize_t value);

/**
 * @brief How many live subscriptions the bus holds, over all events (level 1).
 *
 * @param self The bus; must not be ::lh_null (not checked).
 */
lh_usize_t
lh_os_module_event_bus_get_count(const lh_os_module_event_bus_t *self);

/**
 * @brief Store the live subscription count (level 1).
 *
 * A level-1 setter: the count must stay equal to the live records.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_bus_set_count(lh_os_module_event_bus_t *self, lh_usize_t value);

/**
 * @brief Initialize an empty bus without allocating.
 * All calls require external serialization on one thread. The runtime allocator
 * and its context must remain compatible until deinit, as for ::lh_array_t.
 */
lh_void
lh_os_module_event_bus_init(lh_os_module_event_bus_t *self);

/**
 * @brief O(1) entry lookup; null for zero or an out-of-range ID.
 * Returned entry and its name stay owned by the bus until deinit.
 * An in-range ID from another bus is a caller error and cannot be distinguished.
 */
lh_os_module_event_entry_t *
lh_os_module_event_bus_get_entry(const lh_os_module_event_bus_t *self, lh_os_module_event_id_t id);

/**
 * @brief Destroy the subscriptions removed during a publication (level 2).
 *
 * Does nothing while a publication runs: they are destroyed once the outermost one
 * returns. Each is unlinked, its context disposed and its record freed.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 */
lh_void
lh_os_module_event_bus_collect(lh_os_module_event_bus_t *self);

/**
 * @brief Find a registered nonempty NUL-terminated name without allocating.
 * Returns zero for null, empty or unknown names. Linear in registered event count;
 * no subscription is examined. The input text is borrowed only for this call.
 */
lh_os_module_event_id_t
lh_os_module_event_bus_find_id(const lh_os_module_event_bus_t *self, const lh_char_t *name);

/**
 * @brief The first live subscription of event @p id (level 3).
 *
 * Removed records are skipped. The record is borrowed: it stays valid until it is
 * removed and collected, or the bus is cleared or deinitialized.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 * @param id   The event.
 * @return The subscription, or ::lh_null for none or an invalid ID.
 */
lh_os_module_event_subscription_t *
lh_os_module_event_bus_get_first(const lh_os_module_event_bus_t *self, lh_os_module_event_id_t id);

/**
 * @brief Next live subscription, or null. sub must still belong to id.
 * A removed record stays valid only while an enclosing publication is active.
 */
lh_os_module_event_subscription_t *
lh_os_module_event_bus_get_next(const lh_os_module_event_bus_t *self, lh_os_module_event_id_t id,
                                const lh_os_module_event_subscription_t *sub);

/**
 * @brief Append one subscription, or return null for invalid ID/callback or OOM.
 * Each successful call creates a distinct record, even for identical arguments.
 * The bus owns the record; optional dispose assumes context ownership on success
 * only. A newly appended record can run in an ongoing publication of this event.
 */
lh_os_module_event_subscription_t *
lh_os_module_event_bus_try_subscribe(lh_os_module_event_bus_t *self, lh_os_module_event_id_t id,
                                     lh_ptr owner, lh_os_module_event_cb callback, lh_ptr context,
                                     lh_os_module_event_dispose_cb dispose);

/**
 * @brief Remove a record belonging to self; already removed is a no-op.
 * Delivery stops immediately. Unlinking, disposal and freeing wait until the
 * outermost publication returns, so self-removal and nested publish are safe.
 * Outside publication the record is destroyed before this call returns.
 */
lh_void
lh_os_module_event_bus_remove(lh_os_module_event_bus_t *self,
                              lh_os_module_event_subscription_t *sub);

/**
 * @brief Intern a name and return its stable, one-based ID, or zero on failure.
 * Copies new names. Null/empty names, allocation failure and exhausted index space
 * return zero; existing IDs and subscriptions remain valid. Registration can be
 * nested inside publication. IDs survive clear and owner removal, until deinit.
 */
lh_os_module_event_id_t
lh_os_module_event_bus_try_get_id(lh_os_module_event_bus_t *self, const lh_char_t *name);

/**
 * @brief ::lh_os_module_event_bus_try_subscribe that fails at run time instead of returning
 * ::lh_null.
 *
 * @fails ::lh_runtime_error_code_memory_not_allocated
 *        The ID or the callback is invalid, or the record could not be allocated.
 */
lh_os_module_event_subscription_t *
lh_os_module_event_bus_subscribe(lh_os_module_event_bus_t *self, lh_os_module_event_id_t id,
                                 lh_ptr owner, lh_os_module_event_cb callback, lh_ptr context,
                                 lh_os_module_event_dispose_cb dispose);

/**
 * @brief Synchronously dispatch only id's ordered subscribers; no name comparisons.
 * Invalid IDs or no handlers return continue. Payload is borrowed and shared,
 * never copied/retained. Returns the strongest result; stop ends the chain.
 * Callbacks may register, subscribe, remove, clear and publish recursively.
 * Deinit and disposal-callback reentry are forbidden during dispatch.
 */
lh_os_module_event_result_t
lh_os_module_event_bus_publish(lh_os_module_event_bus_t *self, lh_os_module_event_id_t id,
                               lh_ptr payload);

/**
 * @brief Remove every subscription made under @p owner (level 4).
 *
 * A ::lh_null owner is a tag like any other. IDs and names stay. During a publication
 * the records stop being called at once and are destroyed when it returns.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 * @param owner The tag, compared by identity.
 */
lh_void
lh_os_module_event_bus_drop_owner(lh_os_module_event_bus_t *self, lh_ptr owner);

/**
 * @brief Remove every subscription of every event (level 4).
 *
 * The names and their IDs stay valid. During a publication the records stop being
 * called at once and are destroyed when it returns.
 *
 * @param self The bus; must not be ::lh_null (not checked).
 */
lh_void
lh_os_module_event_bus_clear(lh_os_module_event_bus_t *self);

/**
 * @brief Find the first live record of id tagged owner and accepted by match.
 * A null match accepts every context. Returns a borrowed record, or null if
 * absent/invalid. The matcher receives the record context and key and must not
 * mutate the bus. Search visits only this event's subscribers and allocates nothing.
 */
lh_os_module_event_subscription_t *
lh_os_module_event_bus_find_subscription(const lh_os_module_event_bus_t *self,
                                         lh_os_module_event_id_t id, lh_ptr owner,
                                         lh_os_module_event_match_cb match, lh_ptr key);

/**
 * @brief Resolve name only while *cache is zero, storing and returning the ID.
 * cache must be nonnull and initialized to zero, or hold this name's ID from
 * this bus lifetime. Failure leaves zero so a later call can retry. This is an
 * externally serialized scalar cache, with the same lifetime as the bus IDs.
 */
lh_os_module_event_id_t
lh_os_module_event_bus_try_get_cached_id(lh_os_module_event_bus_t *self,
                                         lh_os_module_event_id_t *cache, const lh_char_t *name);

/**
 * @brief ::lh_os_module_event_bus_try_get_id that fails at run time instead of returning zero.
 *
 * @fails ::lh_runtime_error_code_memory_not_allocated
 *        The name is empty or ::lh_null, or the registry could not grow.
 */
lh_os_module_event_id_t
lh_os_module_event_bus_get_id(lh_os_module_event_bus_t *self, const lh_char_t *name);

/**
 * @brief Clear subscriptions and free the registry. Invalidates all issued IDs.
 * Requires no active publish. The object may subsequently be initialized again;
 * IDs from the previous lifetime must not be used in the new one.
 */
lh_void
lh_os_module_event_bus_deinit(lh_os_module_event_bus_t *self);

/**
 * @brief ::lh_os_module_event_bus_try_get_cached_id that fails at run time instead of returning
 * zero.
 *
 * @fails ::lh_runtime_error_code_memory_not_allocated
 *        The name could not be registered.
 */
lh_os_module_event_id_t
lh_os_module_event_bus_get_cached_id(lh_os_module_event_bus_t *self, lh_os_module_event_id_t *cache,
                                     const lh_char_t *name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_EVENT_BUS_H */
