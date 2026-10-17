/**
 * @file input.h
 * @brief OS layer: how the system is set up for input - ::lh_os_input_get_wheel_step.
 *
 * The events, the queue and the recognizer (`lh/os/input/`) need no OS; this is the
 * part that asks the system, through the kernel of the platform it is built for,
 * picked by `#if` (as `lh/os/tick.h` is):
 *
 * - Windows with ::LH_LIBRARY_OPTION_OS_WINDOW: `src/lh/os/system/win/input.c`,
 *   `SystemParametersInfoA(SPI_GETWHEELSCROLLLINES)`.
 * - Linux, macOS: `src/lh/os/system/unix/input.c`, which reads nothing yet - the
 *   driver's increment needs libinput or evdev - so ::LH_OS_INPUT_WHEEL_LINES_DEFAULT.
 * - Windows without the window backend (user32 is not linked), anything else (STM32,
 *   ...): no kernel, ::LH_OS_INPUT_WHEEL_STEP_DEFAULT.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_INPUT_H
#define LH_OS_INPUT_H

#include <lh/config.h>
#include <lh/os/input/wheel.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/input.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How far one wheel notch (::LH_OS_INPUT_WHEEL_NOTCH) scrolls, in content
 *        units, as the system is set up now.
 *
 * The kernel's lines a notch (see the table above) through ::lh_os_input_wheel_step_of,
 * so the default 3 lines are ::LH_OS_INPUT_WHEEL_STEP_DEFAULT (40). On Windows a call
 * that fails reads as the default, "one screen at a time" as
 * ::LH_OS_INPUT_WHEEL_LINES_PAGE lines, and `0` (wheel scrolling turned off) as `0`.
 *
 * Not cached: each call asks again, so a change in the Mouse control panel applies
 * from the next notch. ::lh_ui_view_input calls it once a wheel event.
 *
 * @return Content units a notch, from `0` to
 *         `lh_os_input_wheel_step_of(LH_OS_INPUT_WHEEL_LINES_MAX)`.
 */
lh_s32_t lh_os_input_get_wheel_step(lh_void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_H */
