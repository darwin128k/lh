/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_ui_view_pointer_t.
 */

#ifndef LH_UI_VIEW_POINTER_INITIALIZER_H
#define LH_UI_VIEW_POINTER_INITIALIZER_H

#include <lh/bool.h>
#include <lh/initializer.h>
#include <lh/null.h>
#include <lh/ui/scalar.h>

/**
 * @def lh_ui_view_pointer_initializer
 * @brief Produces a brace-enclosed initializer for ::lh_ui_view_pointer_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_view_pointer_fields. Each point is a brace initializer of its own,
 * `lh_initializer(x, y)`.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_view_pointer_t held = lh_ui_view_pointer_initializer(
 *     lh_addr_of(button_entity),                          // target
 *     lh_null,                                            // drag
 *     lh_null,                                            // grab
 *     lh_ui_scalar(0),                                    // grab_offset
 *     lh_initializer(lh_ui_scalar(10), lh_ui_scalar(20)), // press_point
 *     lh_initializer(lh_ui_scalar(10), lh_ui_scalar(20)), // last_point
 *     lh_initializer(lh_ui_scalar(0), lh_ui_scalar(0)),   // velocity
 *     lh_bool_true,                                       // pressed
 *     lh_bool_false);                                     // dragged
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_view_pointer_t
 */
#define lh_ui_view_pointer_initializer(target, drag, grab, grab_offset, press_point, last_point,   \
                                       velocity, pressed, dragged)                                 \
    lh_initializer(target, drag, grab, grab_offset, press_point, last_point, velocity, pressed,    \
                   dragged)

/**
 * @def lh_ui_view_pointer_idle_initializer()
 * @brief A pointer with no press, no grab and no drag, at the origin, still: what
 *        ::lh_ui_view_pointer_init leaves.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_view_pointer_t idle = lh_ui_view_pointer_idle_initializer();
 * @endcode
 *
 * @see lh_ui_view_pointer_initializer
 */
#define lh_ui_view_pointer_idle_initializer()                                                      \
    lh_ui_view_pointer_initializer(lh_null, lh_null, lh_null, lh_ui_scalar(0),                     \
                                   lh_initializer(lh_ui_scalar(0), lh_ui_scalar(0)),               \
                                   lh_initializer(lh_ui_scalar(0), lh_ui_scalar(0)),               \
                                   lh_initializer(lh_ui_scalar(0), lh_ui_scalar(0)),               \
                                   lh_bool_false, lh_bool_false)

#endif /* LH_UI_VIEW_POINTER_INITIALIZER_H */
