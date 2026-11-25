/**
 * @file input.c
 * @brief Implementation of `lh/os/input.h`: the contract's slot over the platform's
 *        kernel, which the link picks.
 */

#include <lh/os/input.h>
#include <lh/os/system/input.h>
#include <lh/core/attribute/unused.h>

lh_u32_t
lh_os_input_get_wheel_lines(lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(context);
    return lh_os_system_input_get_wheel_lines();
}

lh_void
lh_os_input_read_recognizer_config(lh_input_recognizer_config_t *config)
{
    const lh_u32_t most = lh_type_cast(lh_u32_t, LH_U16_T_MAX);

    lh_input_recognizer_config_set_slop(
        config, lh_type_cast(lh_u16_t, lh_math_min(lh_os_system_input_get_drag_slop(), most)));
    lh_input_recognizer_config_set_double_click_slop(
        config,
        lh_type_cast(lh_u16_t, lh_math_min(lh_os_system_input_get_double_click_slop(), most)));
    lh_input_recognizer_config_set_double_click_ms(
        config,
        lh_type_cast(lh_u16_t, lh_math_min(lh_os_system_input_get_double_click_ms(), most)));
}
