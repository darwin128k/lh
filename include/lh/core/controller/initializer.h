/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_controller_t.
 */

#ifndef LH_CONTROLLER_INITIALIZER_H
#define LH_CONTROLLER_INITIALIZER_H

#include <lh/core/controller/flags.h>
#include <lh/core/controller/flags/value.h>
#include <lh/core/controller/handlers/cancel/cb.h>
#include <lh/core/controller/handlers/handle/cb.h>
#include <lh/core/controller/handlers/update/cb.h>
#include <lh/core/input/recognizer/initializer.h>

/**
 * @def lh_controller_initializer(handle_fn, update_fn, cancel_fn, context, recognizer, flags)
 * @brief Produces a brace-enclosed initializer for ::lh_controller_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_controller_fields. Each handler goes through ::lh_cast_static to its
 * pointer type, as in ::lh_memory_allocator_initializer, so a function of the
 * wrong shape is a compile error in C++; ::lh_null is allowed for each.
 * @p recognizer is an ::lh_input_recognizer_initializer.
 *
 * Example usage:
 * @code{.c}
 * static lh_controller_t controller = lh_controller_initializer(
 *     my_handle,                                                         // handle_cb
 *     lh_null,                                                           // update_cb
 *     lh_null,                                                           // cancel_cb
 *     lh_addr_of(my_state),                                              // context
 *     lh_input_recognizer_initializer(
 *         lh_input_recognizer_config_default_initializer(),
 *         {lh_input_recognizer_pointer_idle_initializer()}),          // recognizer
 *     lh_controller_flags_enabled | lh_controller_flags_gestures); // flags
 * @endcode
 *
 * @see lh_initializer
 * @see lh_controller_t
 * @see lh_controller_init
 */
#define lh_controller_initializer(handle_fn, update_fn, cancel_fn, context, recognizer, flags)     \
    lh_initializer(lh_cast_static(lh_controller_handlers_handle_cb, handle_fn),                    \
                   lh_cast_static(lh_controller_handlers_update_cb, update_fn),                    \
                   lh_cast_static(lh_controller_handlers_cancel_cb, cancel_fn), (context),         \
                   recognizer, lh_cast_static(lh_controller_flags_t, flags))

/**
 * @def lh_controller_default_initializer(handle_fn, context)
 * @brief The state ::lh_controller_init leaves with the default thresholds:
 *        @p handle_fn on @p context, no update or cancel handler, enabled, with
 *        gestures.
 *
 * Example usage:
 * @code{.c}
 * static lh_controller_t controller =
 *     lh_controller_default_initializer(lh_gl_view_controller_handle_event_is_handled,
 * lh_addr_of(view));
 * @endcode
 */
#define lh_controller_default_initializer(handle_fn, context)                                      \
    lh_controller_initializer(                                                                     \
        handle_fn, lh_null, lh_null, context,                                                      \
        lh_input_recognizer_initializer(lh_input_recognizer_config_default_initializer(),          \
                                        {lh_input_recognizer_pointer_idle_initializer()}),         \
        lh_controller_flags_enabled | lh_controller_flags_gestures)

/**
 * @def lh_controller_empty_initializer()
 * @brief A controller with no handler, no context and no flag: it drops every event.
 *
 * Example usage:
 * @code{.c}
 * static lh_controller_t controller = lh_controller_empty_initializer();
 * @endcode
 */
#define lh_controller_empty_initializer()                                                          \
    lh_controller_initializer(                                                                     \
        lh_null, lh_null, lh_null, lh_null,                                                        \
        lh_input_recognizer_initializer(lh_input_recognizer_config_default_initializer(),          \
                                        {lh_input_recognizer_pointer_idle_initializer()}),         \
        lh_controller_flags_none)

#endif /* LH_CONTROLLER_INITIALIZER_H */
