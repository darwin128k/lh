/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_os_input_recognizer_pointer_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_POINTER_INITIALIZER_H
#define LH_OS_INPUT_RECOGNIZER_POINTER_INITIALIZER_H

#include <lh/initializer.h>
#include <lh/os/input/button.h>
#include <lh/os/input/device/kind.h>
#include <lh/os/input/mods.h>
#include <lh/os/input/recognizer/phase.h>

/**
 * @def lh_os_input_recognizer_pointer_initializer
 * @brief Produces a brace-enclosed initializer for
 *        ::lh_os_input_recognizer_pointer_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_os_input_recognizer_pointer_fields.
 *
 * Example usage:
 * @code{.c}
 * static const lh_os_input_recognizer_pointer_t pending =
 *     lh_os_input_recognizer_pointer_initializer(lh_os_input_recognizer_phase_pending,  // phase
 *                                                lh_os_input_device_kind_mouse,         //
 * device_kind lh_os_input_button_primary,            // button LH_OS_INPUT_MODS_NONE, // mods 0, //
 * down_x 0,                                     // down_y 0U,                                    //
 * down_time 0,                                     // last_x 0, // last_y lh_os_input_button_none,
 * // click_button 0,                                     // click_x 0, // click_y 0U, // click_time
 *                                                0U);                                   //
 * click_count
 * @endcode
 *
 * @see lh_initializer
 * @see lh_os_input_recognizer_pointer_t
 */
#define lh_os_input_recognizer_pointer_initializer(                                                \
    phase, device_kind, button, mods, down_x, down_y, down_time, last_x, last_y, click_button,     \
    click_x, click_y, click_time, click_count)                                                     \
    lh_initializer(phase, device_kind, button, mods, down_x, down_y, down_time, last_x, last_y,    \
                   click_button, click_x, click_y, click_time, click_count)

/**
 * @def lh_os_input_recognizer_pointer_idle_initializer()
 * @brief Initializer of a pointer with nothing held and no click to continue.
 *
 * Example usage:
 * @code{.c}
 * static const lh_os_input_recognizer_pointer_t idle =
 *     lh_os_input_recognizer_pointer_idle_initializer();
 * @endcode
 *
 * @see lh_os_input_recognizer_pointer_initializer
 */
#define lh_os_input_recognizer_pointer_idle_initializer()                                          \
    lh_os_input_recognizer_pointer_initializer(                                                    \
        lh_os_input_recognizer_phase_idle, lh_os_input_device_kind_none, lh_os_input_button_none,  \
        LH_OS_INPUT_MODS_NONE, 0, 0, 0U, 0, 0, lh_os_input_button_none, 0, 0, 0U, 0U)

#endif /* LH_OS_INPUT_RECOGNIZER_POINTER_INITIALIZER_H */
