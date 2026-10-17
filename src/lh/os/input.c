/**
 * @file input.c
 * @brief Implementation of `lh/os/input.h`: the platform's kernel, picked by `#if`.
 */

#include <lh/compiler/os.h>
#include <lh/os/input.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS && LH_LIBRARY_OPTION_OS_WINDOW
#    include <lh/os/system/win/input.h>
#elif LH_COMPILER_OS == LH_COMPILER_OS_LINUX || LH_COMPILER_OS == LH_COMPILER_OS_MAC
#    include <lh/os/system/unix/input.h>
#endif

lh_s32_t
lh_os_input_get_wheel_step(lh_void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS && LH_LIBRARY_OPTION_OS_WINDOW
    return lh_os_input_wheel_step_of(lh_os_system_win_input_get_wheel_lines());
#elif LH_COMPILER_OS == LH_COMPILER_OS_LINUX || LH_COMPILER_OS == LH_COMPILER_OS_MAC
    return lh_os_input_wheel_step_of(lh_os_system_unix_input_get_wheel_lines());
#else
    /* No kernel to ask: Windows without user32 linked (no window backend), or a
       platform with no system setting at all (STM32 and the like). */
    return LH_OS_INPUT_WHEEL_STEP_DEFAULT;
#endif
}
