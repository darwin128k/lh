/**
 * @file input.c
 * @brief The Unix kernel of `lh/os/system/input.h`: the input driver is not read
 *        and X keeps no pointer settings, so every answer is lh's default (see the
 *        header).
 */

#include <lh/os/system/input.h>
#include <lh/core/input/recognizer/config/defaults.h>
#include <lh/core/input/wheel.h>
#include <lh/core/util/type.h>

lh_u32_t
lh_os_system_input_get_wheel_lines(lh_void)
{
    return lh_type_cast(lh_u32_t, LH_INPUT_WHEEL_LINES_DEFAULT);
}

lh_u32_t
lh_os_system_input_get_drag_slop(lh_void)
{
    return lh_type_cast(lh_u32_t, LH_INPUT_RECOGNIZER_SLOP_DEFAULT);
}

lh_u32_t
lh_os_system_input_get_double_click_slop(lh_void)
{
    return lh_type_cast(lh_u32_t, LH_INPUT_RECOGNIZER_DOUBLE_CLICK_SLOP_DEFAULT);
}

lh_u32_t
lh_os_system_input_get_double_click_ms(lh_void)
{
    return lh_type_cast(lh_u32_t, LH_INPUT_RECOGNIZER_DOUBLE_CLICK_MS_DEFAULT);
}
