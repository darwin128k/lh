/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_os_input_event_t.
 */

#ifndef LH_OS_INPUT_EVENT_INITIALIZER_H
#define LH_OS_INPUT_EVENT_INITIALIZER_H

#include <lh/initializer.h>
#include <lh/os/input/axis.h>
#include <lh/os/input/button.h>
#include <lh/os/input/device/kind.h>
#include <lh/os/input/flags.h>
#include <lh/os/input/kind.h>
#include <lh/os/input/mods.h>
#include <lh/os/key.h>

/**
 * @def lh_os_input_event_initializer
 * @brief Produces a brace-enclosed initializer for ::lh_os_input_event_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_os_input_event_fields.
 *
 * Example usage:
 * @code{.c}
 * static const lh_os_input_event_t down =
 *     lh_os_input_event_initializer(lh_os_input_kind_pointer_down,    // kind
 *                                   lh_os_input_device_kind_mouse,    // device_kind
 *                                   0U,                               // device
 *                                   0U,                               // pointer
 *                                   lh_os_input_button_primary,       // button
 *                                   0x2U,                             // buttons
 *                                   lh_os_input_coord_from_int(10),   // x
 *                                   lh_os_input_coord_from_int(20),   // y
 *                                   0,                                // dx
 *                                   0,                                // dy
 *                                   lh_os_input_axis_none,            // axis
 *                                   0,                                // value
 *                                   lh_os_key_other,                  // key
 *                                   0U,                               // scancode
 *                                   0U,                               // code
 *                                   LH_OS_INPUT_MODS_NONE,            // mods
 *                                   1000U,                            // time
 *                                   0U,                               // count
 *                                   LH_OS_INPUT_FLAG_PRIMARY);        // flags
 * @endcode
 *
 * @see lh_initializer
 * @see lh_os_input_event_t
 */
#define lh_os_input_event_initializer(kind, device_kind, device, pointer, button, buttons, x, y,   \
                                      dx, dy, axis, value, key, scancode, code, mods, time, count, \
                                      flags)                                                       \
    lh_initializer(kind, device_kind, device, pointer, button, buttons, x, y, dx, dy, axis, value, \
                   key, scancode, code, mods, time, count, flags)

/**
 * @def lh_os_input_event_empty_initializer()
 * @brief Initializer of an event of kind ::lh_os_input_kind_none, every field zero.
 *
 * Example usage:
 * @code{.c}
 * lh_os_input_event_t event = lh_os_input_event_empty_initializer();
 * @endcode
 *
 * @see lh_os_input_event_initializer
 */
#define lh_os_input_event_empty_initializer()                                                      \
    lh_os_input_event_initializer(lh_os_input_kind_none, lh_os_input_device_kind_none, 0U, 0U,     \
                                  lh_os_input_button_none, 0U, 0, 0, 0, 0, lh_os_input_axis_none,  \
                                  0, lh_os_key_other, 0U, 0U, LH_OS_INPUT_MODS_NONE, 0U, 0U,       \
                                  LH_OS_INPUT_FLAG_NONE)

#endif /* LH_OS_INPUT_EVENT_INITIALIZER_H */
