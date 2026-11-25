/**
 * @file input.h
 * @brief Native input settings: ::lh_os_system_input_get_wheel_lines.
 *
 * What the system says about input apart from any one event, in the platform's
 * own terms. One name, one kernel per platform; the link picks which one exists:
 *
 * - Windows: `src/lh/os/system/win/input.c`,
 *   `SystemParametersInfoA(SPI_GETWHEELSCROLLLINES)`. Built with
 *   ::LH_LIBRARY_OPTION_OS_WINDOW, since user32 is linked only then.
 * - Linux, macOS: `src/lh/os/system/unix/input.c`. How far one detent goes is the
 *   input driver's to say: libinput hands it over with each event
 *   (`libinput_event_pointer_get_scroll_value_v120`), evdev as `REL_WHEEL_HI_RES`
 *   next to `REL_WHEEL`. Reading either means linking libinput or opening
 *   `/dev/input/event*` (the `input` group), and lh does neither: it builds against
 *   no system library and opens only Xlib, by `dlopen`, at run time. Neither Xlib
 *   nor the X server keeps a "lines a notch" setting (that belongs to each
 *   toolkit), so this kernel answers ::LH_INPUT_WHEEL_LINES_DEFAULT. How many
 *   notches one X event is belongs to the X11 kernel
 *   (::lh_os_system_posix_input_wheel_increment_of).
 * - Anything else (Windows without the window backend, a board): no kernel, and
 *   nothing to link against. The portable side does not need one: a consumer
 *   without an installed ::lh_input_wheel_lines_fn uses its default.
 *
 * The portable brick over it is ::lh_os_input_get_wheel_lines (`lh/os/input.h`).
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

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_INPUT_H */
