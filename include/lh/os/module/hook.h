/**
 * @file hook.h
 * @brief ::lh_os_module_hook_t — a chain of handlers that stand in a call.
 *
 * An event tells modules that something happened (`lh/os/module/event/bus.h`); a
 * hook puts their handlers in the call itself. Each handler may change the call
 * and may stop the chain. The handler added last runs first. A handler that
 * spends longer than the chain's budget is dropped from the chain, so one module
 * cannot stall the caller's loop.
 *
 * The chain holds at most ::LH_OS_MODULE_HOOK_SIZE handlers and never allocates.
 * Owner-thread only: add, remove and call from one thread.
 */

#ifndef LH_OS_MODULE_HOOK_H
#define LH_OS_MODULE_HOOK_H

#include <lh/os/module/hook/handler.h>
#include <lh/core/size.h>

/** @brief How many handlers one chain holds. */
#define LH_OS_MODULE_HOOK_SIZE 16U

/** @brief The handlers of one call, in the order they were added. */
struct lh_os_module_hook
{
    lh_os_module_hook_handler_t handlers[LH_OS_MODULE_HOOK_SIZE]; /**< from the first added */
    lh_usize_t count;                                             /**< handlers held */
};
/** @copybrief lh_os_module_hook */
typedef struct lh_os_module_hook lh_os_module_hook_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The handler at @p index, from the first added (level 1).
 *
 * @param self  The chain; checked by ::lh_assert_runtime_ref.
 * @param index Below ::LH_OS_MODULE_HOOK_SIZE.
 */
const lh_os_module_hook_handler_t *
lh_os_module_hook_get_handler(const lh_os_module_hook_t *self, lh_usize_t index);

/**
 * @brief The handler at @p index, to change in place (level 1).
 *
 * @param self  The chain; checked by ::lh_assert_runtime_ref.
 * @param index Below ::LH_OS_MODULE_HOOK_SIZE.
 */
lh_os_module_hook_handler_t *
lh_os_module_hook_get_handler_mut(lh_os_module_hook_t *self, lh_usize_t index);

/**
 * @brief Handlers held (level 1).
 *
 * @param self The chain; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_os_module_hook_get_count(const lh_os_module_hook_t *self);

/**
 * @brief Store the handlers held (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The chain; checked by ::lh_assert_runtime_ref.
 * @param count The new value.
 */
lh_void
lh_os_module_hook_set_count(lh_os_module_hook_t *self, lh_usize_t count);

/**
 * @brief Make @p self an empty chain (level 2).
 *
 * @param self The chain; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_os_module_hook_init(lh_os_module_hook_t *self);

/**
 * @brief Take the handler at @p index out, keeping the order of the rest (level 2).
 *
 * @param self  The chain; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_os_module_hook_get_count.
 */
lh_void
lh_os_module_hook_erase(lh_os_module_hook_t *self, lh_usize_t index);

/**
 * @brief Add a handler; it runs before every handler added earlier (level 2).
 *
 * @param self    The chain; checked by ::lh_assert_runtime_ref.
 * @param owner   Who adds it.
 * @param tag     How @p owner is shown when the handler stops a call; may be
 *                ::lh_null; borrowed, it must outlive the handler.
 * @param fn      The handler.
 * @param context The value it is given.
 * @return ::lh_bool_false when @p fn is ::lh_null or the chain is full.
 */
lh_bool_t
lh_os_module_hook_try_add(lh_os_module_hook_t *self, lh_ptr owner, const lh_char_t *tag,
                          lh_os_module_hook_cb fn, lh_ptr context);

/**
 * @brief Remove the handler @p fn with @p context that @p owner added (level 3).
 *
 * @param self    The chain; checked by ::lh_assert_runtime_ref.
 * @param owner   Who added it.
 * @param fn      The handler.
 * @param context The value it is given.
 * @return ::lh_bool_false when the chain has no such handler.
 */
lh_bool_t
lh_os_module_hook_try_remove(lh_os_module_hook_t *self, lh_ptr owner, lh_os_module_hook_cb fn,
                             lh_ptr context);

/**
 * @brief Remove every handler @p owner added (level 3).
 *
 * @param self  The chain; checked by ::lh_assert_runtime_ref.
 * @param owner Whose handlers go.
 */
lh_void
lh_os_module_hook_remove_owner(lh_os_module_hook_t *self, lh_ptr owner);

/**
 * @brief Run the handlers on @p call, the last added first, until one stops it
 *        (level 3).
 *
 * A handler that spends more than @p budget_us microseconds is dropped from the
 * chain, whatever it returned, and the rest of the chain runs.
 *
 * @param self       The chain; checked by ::lh_assert_runtime_ref.
 * @param call       Handed to every handler.
 * @param budget_us  What one handler may spend; 0 for no limit.
 * @param stopped_by Receives the tag of the handler that stopped the call, or
 *                   ::lh_null when none did; may be ::lh_null.
 * @param overran    Receives whether a handler was dropped for its budget; may be
 *                   ::lh_null.
 * @return ::lh_bool_false when a handler stopped the call.
 */
lh_bool_t
lh_os_module_hook_call(lh_os_module_hook_t *self, lh_ptr call, lh_u64_t budget_us,
                       const lh_char_t **stopped_by, lh_bool_t *overran);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_HOOK_H */
