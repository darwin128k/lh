/**
 * @file entry.h
 * @brief Entry state for synchronous indexed event delivery.
 *
 * The bus owns entries, names and subscription records. Payloads are borrowed
 * for synchronous delivery. Structural state is managed through the public
 * functions; low-level setters require the caller to maintain bus invariants.
 */

#ifndef LH_EVENT_ENTRY_H
#define LH_EVENT_ENTRY_H

#include <lh/list.h>
#include <lh/char.h>
#include <lh/event/entry/fields.h>

/** @brief Entry state; use accessors rather than fields. */
struct lh_event_entry
{
    lh_event_entry_fields(lh_char_t *, lh_usize_t, lh_list_t);
};
/** @copybrief lh_event_entry */
typedef struct lh_event_entry lh_event_entry_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Owned NUL-terminated event name. Level-1 read; self must be nonnull. */
lh_char_t *
lh_event_entry_get_name(const lh_event_entry_t *self);

/** @brief Store name without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_entry_set_name(lh_event_entry_t *self, lh_char_t * value);

/** @brief Number of characters excluding NUL. Level-1 read; self must be nonnull. */
lh_usize_t
lh_event_entry_get_length(const lh_event_entry_t *self);

/** @brief Store length without validation or ownership changes; self must be nonnull. */
lh_void
lh_event_entry_set_length(lh_event_entry_t *self, lh_usize_t value);

/** @brief Ordered subscriber links; bus-owned. Level-1 borrowed member address; self must be nonnull. */
const lh_list_t *
lh_event_entry_get_subscriptions(const lh_event_entry_t *self);

/** @brief Ordered subscriber links; bus-owned. Level-1 borrowed member address; self must be nonnull. */
lh_list_t *
lh_event_entry_get_subscriptions_mut(lh_event_entry_t *self);

/** @brief Initialize an allocated entry, taking ownership of name. No allocation. */
lh_void
lh_event_entry_init(lh_event_entry_t *self, lh_char_t *name, lh_usize_t length);

/** @brief Release the name and entry. Its subscription list must be empty. */
lh_void
lh_event_entry_destroy(lh_event_entry_t *self);

/**
 * @brief Allocate an entry and copy length characters plus their NUL terminator.
 * name must point to length characters followed by NUL. Returns null for a null
 * name, zero/overflowing length, or allocation failure. Owns the copy on success;
 * caller must destroy the entry after its subscribers have been removed.
 */
lh_event_entry_t *
lh_event_entry_try_create(const lh_char_t *name, lh_usize_t length);

/** @brief Checked entry creation; runtime failure when try_create returns null. */
lh_event_entry_t *
lh_event_entry_create(const lh_char_t *name, lh_usize_t length);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EVENT_ENTRY_H */
