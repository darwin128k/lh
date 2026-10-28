/**
 * @file config.h
 * @brief Thresholds of the gesture recognizer: ::lh_os_input_recognizer_config_t.
 */

#ifndef LH_OS_INPUT_RECOGNIZER_CONFIG_H
#define LH_OS_INPUT_RECOGNIZER_CONFIG_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/os/input/recognizer/config/fields.h>
#include <lh/os/input/recognizer/config/ptr.h>

/**
 * @struct lh_os_input_recognizer_config
 * @brief How far, how soon and how long, for ::lh_os_input_recognizer_t.
 */
struct lh_os_input_recognizer_config
{
    /** Members: ::lh_os_input_recognizer_config_fields. */
    lh_os_input_recognizer_config_fields(lh_u16_t, lh_bool_t);
};
/** @copybrief lh_os_input_recognizer_config */
typedef struct lh_os_input_recognizer_config lh_os_input_recognizer_config_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Set @p self to the defaults
 *        (::lh_os_input_recognizer_config_default_initializer).
 */
lh_void
lh_os_input_recognizer_config_init_default(lh_os_input_recognizer_config_t *self);

/** @brief Pixels a pointer may move while down and still click. */
lh_u16_t
lh_os_input_recognizer_config_get_slop(const lh_os_input_recognizer_config_ptr self);

/** @brief Milliseconds within which a click continues a series. */
lh_u16_t
lh_os_input_recognizer_config_get_double_click_ms(const lh_os_input_recognizer_config_ptr self);

/** @brief Pixels within which a click continues a series. */
lh_u16_t
lh_os_input_recognizer_config_get_double_click_slop(const lh_os_input_recognizer_config_ptr self);

/** @brief Milliseconds a still press waits for a long press; `0` is never. */
lh_u16_t
lh_os_input_recognizer_config_get_long_press_ms(const lh_os_input_recognizer_config_ptr self);

/** @brief Milliseconds before a held controller button repeats (reserved). */
lh_u16_t
lh_os_input_recognizer_config_get_repeat_delay_ms(const lh_os_input_recognizer_config_ptr self);

/** @brief Milliseconds between repeats of a held controller button (reserved). */
lh_u16_t
lh_os_input_recognizer_config_get_repeat_rate_ms(const lh_os_input_recognizer_config_ptr self);

/** @brief Stick dead zone (reserved). */
lh_u16_t
lh_os_input_recognizer_config_get_axis_deadzone(const lh_os_input_recognizer_config_ptr self);

/** @brief Stick hysteresis (reserved). */
lh_u16_t
lh_os_input_recognizer_config_get_axis_hysteresis(const lh_os_input_recognizer_config_ptr self);

/** @brief True when Shift turns a vertical wheel sideways. */
lh_bool_t
lh_os_input_recognizer_config_is_wheel_shift_horizontal(
    const lh_os_input_recognizer_config_ptr self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_RECOGNIZER_CONFIG_H */
