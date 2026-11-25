/**
 * @file input.h
 * @brief Native input settings: ::lh_os_system_input_get_wheel_lines,
 *        ::lh_os_system_input_get_drag_slop, ::lh_os_system_input_get_double_click_slop,
 *        ::lh_os_system_input_get_double_click_ms.
 *
 * What the system says about input apart from any one event, in the platform's
 * own terms. One name, one kernel per platform; the link picks which one exists:
 *
 * - Windows: `src/lh/os/system/win/input.c`,
 *   `SystemParametersInfoA(SPI_GETWHEELSCROLLLINES)`, `GetSystemMetrics`
 *   (`SM_CXDRAG`, `SM_CXDOUBLECLK` and their `CY` pairs) and `GetDoubleClickTime`.
 *   Built with ::LH_LIBRARY_OPTION_OS_WINDOW, since user32 is linked only then.
 * - Linux, macOS: `src/lh/os/system/unix/input.c`. How far one detent goes is the
 *   input driver's to say: libinput hands it over with each event
 *   (`libinput_event_pointer_get_scroll_value_v120`), evdev as `REL_WHEEL_HI_RES`
 *   next to `REL_WHEEL`. Reading either means linking libinput or opening
 *   `/dev/input/event*` (the `input` group), and lh does neither: it builds against
 *   no system library and opens only Xlib, by `dlopen`, at run time. Neither Xlib
 *   nor the X server keeps a "lines a notch" setting (that belongs to each
 *   toolkit), so this kernel answers ::LH_INPUT_WHEEL_LINES_DEFAULT. How many
 *   notches one X event is belongs to the X11 kernel
 *   (::lh_os_system_posix_input_wheel_increment_of). The same holds for the drag
 *   threshold and the double click: X keeps neither, each toolkit (GTK, Qt) its
 *   own, so the kernel answers lh's defaults
 *   (`lh/core/input/recognizer/config/defaults.h`).
 * - Anything else (Windows without the window backend, a board): no kernel, and
 *   nothing to link against. The portable side does not need one: a consumer
 *   without an installed ::lh_input_wheel_lines_fn uses its default.
 *
 * The portable bricks over it are ::lh_os_input_get_wheel_lines and
 * ::lh_os_input_read_recognizer_config (`lh/os/input.h`).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_INPUT_H
#define LH_OS_SYSTEM_INPUT_H

#include <lh/config.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>
#include <lh/core/void.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/input.h requires LH_LIBRARY_OPTION_OS"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Lines one wheel notch scrolls as the system is set up now.
 *
 * Asked again on every call, so a change in the system's settings applies from
 * the next call. See the file comment for what each platform reads.
 *
 * @return Lines a notch: on Windows `0` when wheel scrolling is turned off,
 *         ::LH_INPUT_WHEEL_LINES_PAGE for "one screen at a time" and
 *         ::LH_INPUT_WHEEL_LINES_DEFAULT when the call fails; on Linux and macOS
 *         always ::LH_INPUT_WHEEL_LINES_DEFAULT.
 */
lh_u32_t
lh_os_system_input_get_wheel_lines(lh_void);

/**
 * @brief Pixels a press may move and still click, before it is a drag, as the
 *        system is set up now.
 *
 * @return On Windows the larger of `SM_CXDRAG` and `SM_CYDRAG`
 *         (::LH_INPUT_RECOGNIZER_SLOP_DEFAULT when the call fails); on Linux and
 *         macOS always ::LH_INPUT_RECOGNIZER_SLOP_DEFAULT.
 */
lh_u32_t lh_os_system_input_get_drag_slop(lh_void);

/**
 * @brief Pixels a second click may land from the first and still be a double
 *        click, as the system is set up now.
 *
 * @return On Windows half the larger side of the `SM_CXDOUBLECLK` x
 *         `SM_CYDOUBLECLK` rectangle (::LH_INPUT_RECOGNIZER_DOUBLE_CLICK_SLOP_DEFAULT
 *         when the call fails); on Linux and macOS always
 *         ::LH_INPUT_RECOGNIZER_DOUBLE_CLICK_SLOP_DEFAULT.
 */
lh_u32_t lh_os_system_input_get_double_click_slop(lh_void);

/**
 * @brief Milliseconds within which a second click is a double click, as the
 *        system is set up now.
 *
 * @return On Windows `GetDoubleClickTime` (the Mouse control panel's speed); on
 *         Linux and macOS always ::LH_INPUT_RECOGNIZER_DOUBLE_CLICK_MS_DEFAULT.
 */
lh_u32_t lh_os_system_input_get_double_click_ms(lh_void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_INPUT_H */
