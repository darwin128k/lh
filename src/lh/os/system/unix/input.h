/**
 * @file input.h
 * @brief Backend-private: what the Unix input driver says about the wheel.
 *
 * How far one detent of a wheel goes is the driver's to say: libinput hands it over
 * with each event (`libinput_event_pointer_get_scroll_value_v120`), evdev as
 * `REL_WHEEL_HI_RES` next to `REL_WHEEL`. Reading either means linking libinput or
 * opening `/dev/input/event*` (the `input` group), and lh does neither: it builds
 * against no system library and opens only Xlib, by `dlopen`, at run time. Neither
 * Xlib nor the X server keeps a "lines a notch" setting (that belongs to each
 * toolkit), so this kernel answers the default, and says so. How many notches one X
 * event is belongs to the X11 kernel (::lh_os_system_posix_input_wheel_increment_of).
 */

#ifndef LH_SRC_OS_SYSTEM_UNIX_INPUT_H
#define LH_SRC_OS_SYSTEM_UNIX_INPUT_H

#include <lh/numeric/fixed/types.h>
#include <lh/void.h>

/**
 * @brief Lines one wheel notch scrolls by the input driver:
 *        ::LH_OS_INPUT_WHEEL_LINES_DEFAULT, because the driver is not read (see the
 *        file comment).
 */
lh_u32_t lh_os_system_unix_input_get_wheel_lines(lh_void);

#endif /* LH_SRC_OS_SYSTEM_UNIX_INPUT_H */
