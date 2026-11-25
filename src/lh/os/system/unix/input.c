/**
 * @file input.c
 * @brief The Unix kernel of `lh/os/system/input.h`: the input driver is not read,
 *        so a notch is the default lines (see the header).
 */

#include <lh/os/system/input.h>
#include <lh/core/input/wheel.h>
#include <lh/core/util/type.h>

lh_u32_t
lh_os_system_input_get_wheel_lines(lh_void)
{
    return lh_type_cast(lh_u32_t, LH_INPUT_WHEEL_LINES_DEFAULT);
}
