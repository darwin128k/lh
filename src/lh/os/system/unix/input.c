/**
 * @file input.c
 * @brief Implementation of `src/lh/os/system/unix/input.h`.
 */

#include <lh/os/input/wheel.h>
#include <lh/os/system/unix/input.h>
#include <lh/util/type.h>

lh_u32_t
lh_os_system_unix_input_get_wheel_lines(lh_void)
{
    return lh_type_cast(lh_u32_t, LH_OS_INPUT_WHEEL_LINES_DEFAULT);
}
