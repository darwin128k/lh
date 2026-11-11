/**
 * @file handler.h
 * @brief ::lh_os_module_hook_handler_t — one handler of a hook chain: the
 *        function, its context, the module that added it and that module's tag.
 */

#ifndef LH_OS_MODULE_HOOK_HANDLER_H
#define LH_OS_MODULE_HOOK_HANDLER_H

#include <lh/os/module/hook/cb.h>

/** @brief One handler of a hook chain. */
struct lh_os_module_hook_handler
{
    lh_os_module_hook_cb fn; /**< the handler */
    lh_ptr context;          /**< the value it is given */
    lh_ptr owner;            /**< who added it; their handlers go together */
    const lh_char_t *tag;    /**< how the owner is shown when the handler stops a call */
};
/** @copybrief lh_os_module_hook_handler */
typedef struct lh_os_module_hook_handler lh_os_module_hook_handler_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The handler (level 1).
 *
 * @param self The handler; checked by ::lh_assert_runtime_ref.
 */
lh_os_module_hook_cb
lh_os_module_hook_handler_get_fn(const lh_os_module_hook_handler_t *self);

/**
 * @brief Store the handler (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The handler; checked by ::lh_assert_runtime_ref.
 * @param fn   The new value.
 */
lh_void
lh_os_module_hook_handler_set_fn(lh_os_module_hook_handler_t *self, lh_os_module_hook_cb fn);

/**
 * @brief The value the handler is given (level 1).
 *
 * @param self The handler; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_module_hook_handler_get_context(const lh_os_module_hook_handler_t *self);

/**
 * @brief Store the value the handler is given (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self    The handler; checked by ::lh_assert_runtime_ref.
 * @param context The new value.
 */
lh_void
lh_os_module_hook_handler_set_context(lh_os_module_hook_handler_t *self, lh_ptr context);

/**
 * @brief Who added the handler (level 1).
 *
 * @param self The handler; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_module_hook_handler_get_owner(const lh_os_module_hook_handler_t *self);

/**
 * @brief Store who added the handler (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The handler; checked by ::lh_assert_runtime_ref.
 * @param owner The new value.
 */
lh_void
lh_os_module_hook_handler_set_owner(lh_os_module_hook_handler_t *self, lh_ptr owner);

/**
 * @brief How the owner is shown when the handler stops a call (level 1).
 *
 * @param self The handler; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_os_module_hook_handler_get_tag(const lh_os_module_hook_handler_t *self);

/**
 * @brief Store how the owner is shown when the handler stops a call (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The handler; checked by ::lh_assert_runtime_ref.
 * @param tag  The new value; borrowed, it must outlive the handler.
 */
lh_void
lh_os_module_hook_handler_set_tag(lh_os_module_hook_handler_t *self, const lh_char_t *tag);

/**
 * @brief Whether @p self is the handler @p fn with @p context that @p owner added
 *        (level 2).
 *
 * @param self    The handler; checked by ::lh_assert_runtime_ref.
 * @param owner   Who added it.
 * @param fn      The handler.
 * @param context The value it is given.
 */
lh_bool_t
lh_os_module_hook_handler_is(const lh_os_module_hook_handler_t *self, lh_ptr owner,
                             lh_os_module_hook_cb fn, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_HOOK_HANDLER_H */
