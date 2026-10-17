/**
 * @file wheel.c
 * @brief Implementation of `lh/os/input/wheel.h`.
 */

#include <lh/math/ops.h>
#include <lh/os/input/wheel.h>
#include <lh/util/type.h>

lh_s32_t
lh_os_input_wheel_step_of(lh_u32_t lines)
{
    const lh_u32_t kept = lh_math_min(lines, lh_type_cast(lh_u32_t, LH_OS_INPUT_WHEEL_LINES_MAX));

    return lh_type_cast(lh_s32_t, kept) * LH_OS_INPUT_WHEEL_STEP_DEFAULT /
           LH_OS_INPUT_WHEEL_LINES_DEFAULT;
}
