/**
 * @file wheel.h
 * @brief How far one wheel notch scrolls: the system's lines as content units
 *        (::lh_os_input_wheel_step_of), and the defaults for a system that says nothing.
 *
 * A system counts a notch in lines (Windows: 3 out of the box); a view scrolls by
 * content units. One line is ::LH_OS_INPUT_WHEEL_STEP_DEFAULT /
 * ::LH_OS_INPUT_WHEEL_LINES_DEFAULT content units, 40 / 3. The line height is kept as
 * that ratio, not rounded to 13 or 14, so a system at its default scrolls exactly
 * ::LH_OS_INPUT_WHEEL_STEP_DEFAULT a notch - what a view scrolled before it asked the
 * system. Asking it is ::lh_os_input_get_wheel_step (`lh/os/input.h`).
 *
 * Needs no ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_INPUT_WHEEL_H
#define LH_OS_INPUT_WHEEL_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

/**
 * @def LH_OS_INPUT_WHEEL_STEP_DEFAULT
 * @brief Content units one notch (::LH_OS_INPUT_WHEEL_NOTCH) scrolls when the system
 *        is at its default (::LH_OS_INPUT_WHEEL_LINES_DEFAULT lines) or cannot be asked.
 */
#ifndef LH_OS_INPUT_WHEEL_STEP_DEFAULT
#    define LH_OS_INPUT_WHEEL_STEP_DEFAULT 40
#endif /* LH_OS_INPUT_WHEEL_STEP_DEFAULT */

/**
 * @def LH_OS_INPUT_WHEEL_LINES_DEFAULT
 * @brief Lines one notch scrolls when the system cannot be asked: the Windows default,
 *        which X11 toolkits and macOS keep too. Makes ::LH_OS_INPUT_WHEEL_STEP_DEFAULT
 *        content units.
 */
#ifndef LH_OS_INPUT_WHEEL_LINES_DEFAULT
#    define LH_OS_INPUT_WHEEL_LINES_DEFAULT 3
#endif /* LH_OS_INPUT_WHEEL_LINES_DEFAULT */

/**
 * @def LH_OS_INPUT_WHEEL_LINES_PAGE
 * @brief Lines a notch counts as when the system asks for a page a notch (Windows
 *        `WHEEL_PAGESCROLL`, "one screen at a time").
 *
 * The OS layer does not know how tall the view under the wheel is, so a page is this
 * many lines: about one screen of a small window.
 */
#ifndef LH_OS_INPUT_WHEEL_LINES_PAGE
#    define LH_OS_INPUT_WHEEL_LINES_PAGE 20
#endif /* LH_OS_INPUT_WHEEL_LINES_PAGE */

/**
 * @def LH_OS_INPUT_WHEEL_LINES_MAX
 * @brief The most lines a notch is taken to scroll: the top of the Windows Mouse
 *        control panel's range. A larger count, from the registry or a driver, is cut
 *        to it, so the step cannot overflow.
 */
#ifndef LH_OS_INPUT_WHEEL_LINES_MAX
#    define LH_OS_INPUT_WHEEL_LINES_MAX 100
#endif /* LH_OS_INPUT_WHEEL_LINES_MAX */

#if LH_OS_INPUT_WHEEL_LINES_DEFAULT <= 0
#    error "LH_OS_INPUT_WHEEL_LINES_DEFAULT must be positive: it divides the step into lines"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p lines a notch as content units a notch: @p lines lines of
 *        ::LH_OS_INPUT_WHEEL_STEP_DEFAULT / ::LH_OS_INPUT_WHEEL_LINES_DEFAULT units
 *        each, cut towards zero.
 *
 * @p lines past ::LH_OS_INPUT_WHEEL_LINES_MAX count as that many. `0` stays `0`: a
 * system set not to scroll on the wheel.
 *
 * Example usage:
 * @code{.c}
 * lh_os_input_wheel_step_of(3);  // 40, the default
 * lh_os_input_wheel_step_of(1);  // 13
 * lh_os_input_wheel_step_of(5);  // 66
 * lh_os_input_wheel_step_of(0);  // 0
 * @endcode
 */
lh_s32_t
lh_os_input_wheel_step_of(lh_u32_t lines);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_WHEEL_H */
