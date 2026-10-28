/**
 * @file button.h
 * @brief Pointer and controller buttons: ::lh_os_input_button_t.
 */

#ifndef LH_OS_INPUT_BUTTON_H
#define LH_OS_INPUT_BUTTON_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/util/type.h>

/**
 * @enum lh_os_input_button
 * @brief One button: of a pointer, or of a controller in the "standard
 *        gamepad" layout (W3C Gamepad, SDL GameController).
 *
 * The order is fixed; new buttons are only ever added at the end. Every value
 * is below 32, so a set of buttons fits an ::lh_u32_t mask
 * (::lh_os_input_button_to_mask).
 */
typedef enum lh_os_input_button
{
    lh_os_input_button_none = 0,       /**< No button changed. */
    lh_os_input_button_primary,        /**< Left mouse button, a touch, a pen tip. */
    lh_os_input_button_secondary,      /**< Right mouse button, pen barrel button. */
    lh_os_input_button_middle,         /**< Middle mouse button (the wheel). */
    lh_os_input_button_back,           /**< Mouse X1. */
    lh_os_input_button_forward,        /**< Mouse X2. */
    lh_os_input_button_south,          /**< Controller A / cross. */
    lh_os_input_button_east,           /**< Controller B / circle. */
    lh_os_input_button_west,           /**< Controller X / square. */
    lh_os_input_button_north,          /**< Controller Y / triangle. */
    lh_os_input_button_shoulder_left,  /**< Left bumper. */
    lh_os_input_button_shoulder_right, /**< Right bumper. */
    lh_os_input_button_stick_left,     /**< Left stick pressed. */
    lh_os_input_button_stick_right,    /**< Right stick pressed. */
    lh_os_input_button_start,          /**< Start / menu. */
    lh_os_input_button_select,         /**< Select / back / view. */
    lh_os_input_button_guide,          /**< Guide / home. */
    lh_os_input_button_dpad_up,        /**< D-pad up. */
    lh_os_input_button_dpad_down,      /**< D-pad down. */
    lh_os_input_button_dpad_left,      /**< D-pad left. */
    lh_os_input_button_dpad_right      /**< D-pad right. */
} lh_os_input_button_t;                /**< One button. */

/**
 * @def lh_os_input_button_to_mask(button)
 * @brief The bit of @p button in a buttons mask
 *        (::lh_os_input_event_get_buttons).
 *
 * Example usage:
 * @code{.c}
 * lh_os_input_button_to_mask(lh_os_input_button_primary);    // 0x2
 * lh_os_input_button_to_mask(lh_os_input_button_secondary);  // 0x4
 * @endcode
 */
#define lh_os_input_button_to_mask(button) lh_bit_shl(lh_type_cast(lh_u32_t, 1U), (button))

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief ::lh_os_input_button_to_mask(@p button) when @p held, else `0`.
 *
 * Shared by backends that fold a platform bit (X `state`, Win `MK_*`, …) into
 * an event's buttons mask.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_os_input_button_mask_if(lh_bool_t held, lh_os_input_button_t button)
{
    return held ? lh_os_input_button_to_mask(button) : lh_type_cast(lh_u32_t, 0U);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_BUTTON_H */
