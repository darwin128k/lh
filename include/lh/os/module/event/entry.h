/**
 * @file entry.h
 * @brief Entry state for synchronous indexed event delivery.
 *
 * The bus owns entries, names and subscription records. Payloads are borrowed
 * for synchronous delivery. Structural state is managed through the public
 * functions; low-level setters require the caller to maintain bus invariants.
 */

#ifndef LH_OS_MODULE_EVENT_ENTRY_H
#define LH_OS_MODULE_EVENT_ENTRY_H

#include <lh/list.h>
#include <lh/os/module/event/entry/fields.h>

/** @brief Entry state; use accessors rather than fields. */
struct lh_os_module_event_entry
{
    lh_os_module_event_entry_fields(lh_char_t *, lh_usize_t, lh_list_t);
};
/** @copybrief lh_os_module_event_entry */
typedef struct lh_os_module_event_entry lh_os_module_event_entry_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The NUL-terminated name of the event (level 1).
 *
 * Owned by the entry; valid until the bus is deinitialized.
 *
 * @param self The entry; must not be ::lh_null (not checked).
 */
lh_char_t *
lh_os_module_event_entry_get_name(const lh_os_module_event_entry_t *self);

/**
 * @brief Store the name of the event (level 1).
 *
 * A level-1 setter: the old name is not freed and the new one is
 * not copied; the entry owns what it is given.
 *
 * @param self The entry; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_entry_set_name(lh_os_module_event_entry_t *self, lh_char_t *value);

/**
 * @brief How many characters the name has, without the NUL (level 1).
 *
 * @param self The entry; must not be ::lh_null (not checked).
 */
lh_usize_t
lh_os_module_event_entry_get_length(const lh_os_module_event_entry_t *self);

/**
 * @brief Store the length of the name (level 1).
 *
 * A level-1 setter: it must match the name.
 *
 * @param self The entry; must not be ::lh_null (not checked).
 * @param value The new value.
 */
lh_void
lh_os_module_event_entry_set_length(lh_os_module_event_entry_t *self, lh_usize_t value);

/**
 * @brief The subscribers of the event, in order (level 1).
 *
 * Removed records stay linked until they are collected.
 *
 * @param self The entry; must not be ::lh_null (not checked).
 */
const lh_list_t *
lh_os_module_event_entry_get_subscriptions(const lh_os_module_event_entry_t *self);

/**
 * @brief The subscribers of the event, to change in place (level 1).
 *
 * @param self The entry; must not be ::lh_null (not checked).
 */
lh_list_t *
lh_os_module_event_entry_get_subscriptions_mut(lh_os_module_event_entry_t *self);

/**
 * @brief Set up an entry over @p name, which it takes ownership of (level 2).
 *
 * Allocates nothing; the subscriber list starts empty.
 *
 * @param self   The entry; must not be ::lh_null (not checked).
 * @param name   A NUL-terminated name from the runtime allocator.
 * @param length Its length without the NUL.
 */
lh_void
lh_os_module_event_entry_init(lh_os_module_event_entry_t *self, lh_char_t *name, lh_usize_t length);

/**
 * @brief Free the name and the entry itself (level 2).
 *
 * The subscriber list must be empty: its records are not touched.
 *
 * @param self The entry, from ::lh_os_module_event_entry_try_create.
 */
lh_void
lh_os_module_event_entry_destroy(lh_os_module_event_entry_t *self);

/**
 * @brief Allocate an entry and copy length characters plus their NUL terminator.
 * name must point to length characters followed by NUL. Returns null for a null
 * name, zero/overflowing length, or allocation failure. Owns the copy on success;
 * caller must destroy the entry after its subscribers have been removed.
 */
lh_os_module_event_entry_t *
lh_os_module_event_entry_try_create(const lh_char_t *name, lh_usize_t length);

/**
 * @brief ::lh_os_module_event_entry_try_create that fails at run time instead of returning
 * ::lh_null.
 *
 * @fails ::lh_runtime_error_code_memory_not_allocated
 *        The name is invalid or the entry could not be allocated.
 */
lh_os_module_event_entry_t *
lh_os_module_event_entry_create(const lh_char_t *name, lh_usize_t length);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_EVENT_ENTRY_H */
