/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_input_event_t.
 */

#ifndef LH_INPUT_EVENT_INITIALIZER_H
#define LH_INPUT_EVENT_INITIALIZER_H

#include <lh/core/initializer.h>
#include <lh/core/input/axis.h>
#include <lh/core/input/button.h>
#include <lh/core/input/device/kind.h>
#include <lh/core/input/flags.h>
#include <lh/core/input/kind.h>
#include <lh/core/input/mods.h>
#include <lh/core/key.h>

/**
 * @def lh_input_event_initializer
 * @brief Produces a brace-enclosed initializer for ::lh_input_event_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_input_event_fields.
 *
 * Example usage:
 * @code{.c}
 * static const lh_input_event_t down =
 *     lh_input_event_initializer(lh_input_kind_pointer_down,    // kind
 *                                   lh_input_device_kind_mouse,    // device_kind
 *                                   0U,                               // device
 *                                   0U,                               // pointer
 *                                   lh_input_button_primary,       // button
 *                                   0x2U,                             // buttons
 *                                   lh_input_coord_from_int(10),   // x
 *                                   lh_input_coord_from_int(20),   // y
 *                                   0,                                // dx
 *                                   0,                                // dy
 *                                   lh_input_axis_none,            // axis
 *                                   0,                                // value
 *                                   lh_key_other,                  // key
 *                                   0U,                               // scancode
 *                                   0U,                               // code
 *                                   LH_INPUT_MODS_NONE,            // mods
 *                                   1000U,                            // time
 *                                   0U,                               // count
 *                                   LH_INPUT_FLAG_PRIMARY);        // flags
 * @endcode
 *
 * @see lh_initializer
 * @see lh_input_event_t
 */
#define lh_input_event_initializer(kind, device_kind, device, pointer, button, buttons, x, y, dx,  \
                                   dy, axis, value, key, scancode, code, mods, time, count, flags) \
    lh_initializer(kind, device_kind, device, pointer, button, buttons, x, y, dx, dy, axis, value, \
                   key, scancode, code, mods, time, count, flags)

/**
 * @def lh_input_event_empty_initializer()
 * @brief Initializer of an event of kind ::lh_input_kind_none, every field zero.
 *
 * Example usage:
 * @code{.c}
 * lh_input_event_t event = lh_input_event_empty_initializer();
 * @endcode
 *
 * @see lh_input_event_initializer
 */
#define lh_input_event_empty_initializer()                                                         \
    lh_input_event_initializer(lh_input_kind_none, lh_input_device_kind_none, 0U, 0U,              \
                               lh_input_button_none, 0U, 0, 0, 0, 0, lh_input_axis_none, 0,        \
                               lh_key_other, 0U, 0U, LH_INPUT_MODS_NONE, 0U, 0U,                   \
                               LH_INPUT_FLAG_NONE)

#endif /* LH_INPUT_EVENT_INITIALIZER_H */
