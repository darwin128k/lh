/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_os_controller_t.
 */

#ifndef LH_OS_CONTROLLER_INITIALIZER_H
#define LH_OS_CONTROLLER_INITIALIZER_H

#include <lh/os/controller/flags.h>
#include <lh/os/controller/flags/value.h>
#include <lh/os/controller/handlers/cancel/cb.h>
#include <lh/os/controller/handlers/handle/cb.h>
#include <lh/os/controller/handlers/update/cb.h>
#include <lh/os/input/recognizer/initializer.h>

/**
 * @def lh_os_controller_initializer(handle_fn, update_fn, cancel_fn, context, recognizer, flags)
 * @brief Produces a brace-enclosed initializer for ::lh_os_controller_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_os_controller_fields. Each handler goes through ::lh_cast_static to its
 * pointer type, as in ::lh_memory_allocator_initializer, so a function of the
 * wrong shape is a compile error in C++; ::lh_null is allowed for each.
 * @p recognizer is an ::lh_os_input_recognizer_initializer.
 *
 * Example usage:
 * @code{.c}
 * static lh_os_controller_t controller = lh_os_controller_initializer(
 *     my_handle,                                                         // handle_cb
 *     lh_null,                                                           // update_cb
 *     lh_null,                                                           // cancel_cb
 *     lh_addr_of(my_state),                                              // context
 *     lh_os_input_recognizer_initializer(
 *         lh_os_input_recognizer_config_default_initializer(),
 *         {lh_os_input_recognizer_pointer_idle_initializer()}),          // recognizer
 *     lh_os_controller_flags_enabled | lh_os_controller_flags_gestures); // flags
 * @endcode
 *
 * @see lh_initializer
 * @see lh_os_controller_t
 * @see lh_os_controller_init
 */
#define lh_os_controller_initializer(handle_fn, update_fn, cancel_fn, context, recognizer, flags)  \
    lh_initializer(lh_cast_static(lh_os_controller_handlers_handle_cb, handle_fn),                 \
                   lh_cast_static(lh_os_controller_handlers_update_cb, update_fn),                 \
                   lh_cast_static(lh_os_controller_handlers_cancel_cb, cancel_fn), (context),      \
                   recognizer, lh_cast_static(lh_os_controller_flags_t, flags))

/**
 * @def lh_os_controller_default_initializer(handle_fn, context)
 * @brief The state ::lh_os_controller_init leaves with the default thresholds:
 *        @p handle_fn on @p context, no update or cancel handler, enabled, with
 *        gestures.
 *
 * Example usage:
 * @code{.c}
 * static lh_os_controller_t controller =
 *     lh_os_controller_default_initializer(lh_ui_view_controller_handle, lh_addr_of(view));
 * @endcode
 */
#define lh_os_controller_default_initializer(handle_fn, context)                                   \
    lh_os_controller_initializer(                                                                  \
        handle_fn, lh_null, lh_null, context,                                                      \
        lh_os_input_recognizer_initializer(lh_os_input_recognizer_config_default_initializer(),    \
                                           {lh_os_input_recognizer_pointer_idle_initializer()}),   \
        lh_os_controller_flags_enabled | lh_os_controller_flags_gestures)

/**
 * @def lh_os_controller_empty_initializer()
 * @brief A controller with no handler, no context and no flag: it drops every event.
 *
 * Example usage:
 * @code{.c}
 * static lh_os_controller_t controller = lh_os_controller_empty_initializer();
 * @endcode
 */
#define lh_os_controller_empty_initializer()                                                       \
    lh_os_controller_initializer(                                                                  \
        lh_null, lh_null, lh_null, lh_null,                                                        \
        lh_os_input_recognizer_initializer(lh_os_input_recognizer_config_default_initializer(),    \
                                           {lh_os_input_recognizer_pointer_idle_initializer()}),   \
        lh_os_controller_flags_none)

#endif /* LH_OS_CONTROLLER_INITIALIZER_H */
