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
