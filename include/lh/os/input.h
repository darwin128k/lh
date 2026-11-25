/**
 * @file input.h
 * @brief OS layer: how the system is set up for input - ::lh_os_input_get_wheel_lines,
 *        ::lh_os_input_read_recognizer_config.
 *
 * The events, the queue and the recognizer (`lh/core/input/`) need no OS. These are
 * the bricks that fill their contracts and their settings from the system, over the
 * platform's kernels (`lh/os/system/input.h`):
 *
 * - ::lh_os_input_get_wheel_lines is an ::lh_input_wheel_lines_fn, asked on every
 *   notch;
 * - ::lh_os_input_read_recognizer_config writes the system's drag threshold and
 *   double click into an ::lh_input_recognizer_config_t, which is data and is
 *   read once.
 *
 * Nothing installs either by default; a host hands them to whoever consumes them:
 *
 * @code{.c}
 * lh_input_recognizer_config_t config;
 *
 * lh_gl_view_install_wheel_lines(lh_addr_of(view), lh_os_input_get_wheel_lines, lh_null);
 * lh_input_recognizer_config_init_default(lh_addr_of(config));
 * lh_os_input_read_recognizer_config(lh_addr_of(config));
 * lh_input_recognizer_init(lh_addr_of(recognizer), lh_addr_of(config));
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
#include <lh/core/input/recognizer/config.h>
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

/**
 * @brief Write the system's pointer settings into @p config: the drag threshold
 *        (`slop`, ::lh_os_system_input_get_drag_slop), the double-click distance
 *        (`double_click_slop`, ::lh_os_system_input_get_double_click_slop) and
 *        time (`double_click_ms`, ::lh_os_system_input_get_double_click_ms).
 *
 * Every other field of @p config is left as it was, so a host starts from the
 * defaults (::lh_input_recognizer_config_init_default) or its own choices and
 * lets the system decide only what the system has a setting for. An answer past
 * what the config holds (::LH_U16_T_MAX) is cut to it. The config is a copy:
 * a change in the Mouse control panel applies the next time this is called and
 * the config given to the recognizer (::lh_input_recognizer_set_config). On Linux
 * and macOS every answer is lh's default
 * (`lh/core/input/recognizer/config/defaults.h`).
 *
 * @param config The config to write into; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_os_input_read_recognizer_config(lh_input_recognizer_config_t *config);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_H */
