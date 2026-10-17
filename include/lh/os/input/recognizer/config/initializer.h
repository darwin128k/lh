/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_os_input_recognizer_config_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_CONFIG_INITIALIZER_H
#define LH_OS_INPUT_RECOGNIZER_CONFIG_INITIALIZER_H

#include <lh/bool.h>
#include <lh/initializer.h>

/**
 * @def lh_os_input_recognizer_config_initializer
 * @brief Produces a brace-enclosed initializer for
 *        ::lh_os_input_recognizer_config_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_os_input_recognizer_config_fields.
 *
 * Example usage:
 * @code{.c}
 * static const lh_os_input_recognizer_config_t config =
 *     lh_os_input_recognizer_config_initializer(8U,             // slop
 *                                               400U,           // double_click_ms
 *                                               8U,             // double_click_slop
 *                                               600U,           // long_press_ms
 *                                               400U,           // repeat_delay_ms
 *                                               50U,            // repeat_rate_ms
 *                                               8000U,          // axis_deadzone
 *                                               2000U,          // axis_hysteresis
 *                                               lh_bool_true);  // wheel_shift_horizontal
 * @endcode
 *
 * @see lh_initializer
 * @see lh_os_input_recognizer_config_t
 */
#define lh_os_input_recognizer_config_initializer(                                                 \
    slop, double_click_ms, double_click_slop, long_press_ms, repeat_delay_ms, repeat_rate_ms,      \
    axis_deadzone, axis_hysteresis, wheel_shift_horizontal)                                        \
    lh_initializer(slop, double_click_ms, double_click_slop, long_press_ms, repeat_delay_ms,       \
                   repeat_rate_ms, axis_deadzone, axis_hysteresis, wheel_shift_horizontal)

/**
 * @def lh_os_input_recognizer_config_default_initializer()
 * @brief The defaults: slop 4 px, double click 500 ms within 4 px, long press
 *        500 ms, repeat 400 / 50 ms, dead zone 8000 with 2000 hysteresis, Shift
 *        turns the wheel sideways.
 *
 * Example usage:
 * @code{.c}
 * static const lh_os_input_recognizer_config_t config =
 *     lh_os_input_recognizer_config_default_initializer();
 * @endcode
 *
 * @see lh_os_input_recognizer_config_initializer
 */
#define lh_os_input_recognizer_config_default_initializer()                                        \
    lh_os_input_recognizer_config_initializer(4U, 500U, 4U, 500U, 400U, 50U, 8000U, 2000U,         \
                                              lh_bool_true)

#endif /* LH_OS_INPUT_RECOGNIZER_CONFIG_INITIALIZER_H */
