/**
 * @file fields.h
 * @brief Member fields of ::lh_os_input_recognizer_config_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_CONFIG_FIELDS_H
#define LH_OS_INPUT_RECOGNIZER_CONFIG_FIELDS_H

/**
 * @def lh_os_input_recognizer_config_fields(value_type, bool_type)
 * @brief The recognizer's thresholds, in this order:
 *
 * - `slop` — pixels a pointer may move while down and still click;
 * - `double_click_ms`, `double_click_slop` — how soon and how near (pixels) a
 *   click must follow the last one to count as the next of a series;
 * - `long_press_ms` — how long a still press waits for a long press (`0`
 *   turns long presses off);
 * - `repeat_delay_ms`, `repeat_rate_ms` — held controller buttons (reserved);
 * - `axis_deadzone`, `axis_hysteresis` — controller sticks (reserved);
 * - `wheel_shift_horizontal` — a vertical wheel with Shift held scrolls across.
 *
 * @param value_type ::lh_u16_t.
 * @param bool_type  ::lh_bool_t.
 */
#define lh_os_input_recognizer_config_fields(value_type, bool_type)                                \
    value_type slop;                                                                               \
    value_type double_click_ms;                                                                    \
    value_type double_click_slop;                                                                  \
    value_type long_press_ms;                                                                      \
    value_type repeat_delay_ms;                                                                    \
    value_type repeat_rate_ms;                                                                     \
    value_type axis_deadzone;                                                                      \
    value_type axis_hysteresis;                                                                    \
    bool_type wheel_shift_horizontal

#endif /* LH_OS_INPUT_RECOGNIZER_CONFIG_FIELDS_H */
