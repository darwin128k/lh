/**
 * @file input.h
 * @brief OS layer: how the system is set up for input - ::lh_os_input_get_wheel_lines.
 *
 * The events, the queue and the recognizer (`lh/core/input/`) need no OS. This is
 * the brick that fills their contracts from the system: ::lh_os_input_get_wheel_lines
 * is an ::lh_input_wheel_lines_fn over the platform's kernel
 * (`lh/os/system/input.h`). Nothing installs it by default; a host hands it to
 * whoever consumes wheel notches:
 *
 * @code{.c}
 * lh_gl_view_install_wheel_lines(lh_addr_of(view), lh_os_input_get_wheel_lines, lh_null);
 * @endcode
 *
 * Built where the system has a kernel: Windows with ::LH_LIBRARY_OPTION_OS_WINDOW
 * (user32), Linux and macOS. Elsewhere (Windows without the window backend, a
 * board) there is nothing to ask, and a host installs nothing or its own.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_INPUT_H
#define LH_OS_INPUT_H

#include <lh/config.h>
#include <lh/core/input/wheel/lines/fn.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/input.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An ::lh_input_wheel_lines_fn: the lines one wheel notch
 *        (::LH_INPUT_WHEEL_NOTCH) scrolls, as the system is set up now.
 *
 * ::lh_os_system_input_get_wheel_lines, asked again on each call, so a change in
 * the Mouse control panel applies from the next notch. On Windows a call that
 * fails reads as ::LH_INPUT_WHEEL_LINES_DEFAULT, "one screen at a time" as
 * ::LH_INPUT_WHEEL_LINES_PAGE and `0` (wheel scrolling turned off) as `0`; Linux
 * and macOS answer ::LH_INPUT_WHEEL_LINES_DEFAULT.
 *
 * @param context Not used; install it with ::lh_null.
 * @return Lines a notch.
 */
lh_u32_t
lh_os_input_get_wheel_lines(lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_H */
