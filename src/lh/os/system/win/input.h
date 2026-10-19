/**
 * @file input.h
 * @brief Backend-private: Win32 mouse, pointer (touch, pen) and keyboard messages
 *        read into the pieces of an ::lh_os_input_event_t.
 *
 * Pure functions of the message, `wParam`, `lParam` and what the system says of a
 * pointer, apart from ::lh_os_system_win_input_get_mods and
 * ::lh_os_system_win_input_get_wheel_lines, so they can be tested without a window
 * and without a touch screen.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_INPUT_H
#define LH_SRC_OS_SYSTEM_WIN_INPUT_H

#include <lh/os/input/event.h>
#include <lh/os/system/win/user32.h>

/**
 * @brief The button a `WM_*BUTTONDOWN` / `WM_*BUTTONUP` @p msg is about;
 *        ::lh_os_input_button_none for any other message.
 *
 * @p wparam tells the two X buttons apart.
 */
lh_os_input_button_t
lh_os_system_win_input_button_of(lh_u32_t msg, lh_os_system_win_wparam_t wparam);

/**
 * @brief True when @p msg is a button going down (`WM_LBUTTONDOWN`,
 *        `WM_RBUTTONDOWN`, `WM_MBUTTONDOWN`, `WM_XBUTTONDOWN`).
 */
lh_bool_t
lh_os_system_win_input_is_button_down(lh_u32_t msg);

/**
 * @brief The buttons held, from the `MK_*` bits of a mouse message's @p wparam
 *        (::lh_os_input_button_to_mask).
 */
lh_u32_t
lh_os_system_win_input_buttons_of(lh_os_system_win_wparam_t wparam);

/**
 * @brief True when the `WM_KEYDOWN` / `WM_SYSKEYDOWN` with @p lparam repeats a key
 *        already down (bit 30, the previous key state).
 */
lh_bool_t
lh_os_system_win_input_is_repeat(lh_os_system_win_lparam_t lparam);

/**
 * @brief A wheel @p msg (`WM_MOUSEWHEEL`, `WM_MOUSEHWHEEL`) with @p wparam as the
 *        `dx`, `dy` of a wheel event: 1/120 notch, positive left / up.
 */
lh_void
lh_os_system_win_input_wheel_of(lh_u32_t msg, lh_os_system_win_wparam_t wparam, lh_s32_t *dx,
                                lh_s32_t *dy);

/**
 * @brief Lines one wheel notch scrolls, from what
 *        `SystemParametersInfoA(SPI_GETWHEELSCROLLLINES)` answered: @p got is what it
 *        returned, @p lines what it wrote.
 *
 * - The call failed (@p got false): ::LH_OS_INPUT_WHEEL_LINES_DEFAULT, the Windows
 *   default.
 * - ::LH_OS_SYSTEM_WIN_WHEEL_PAGESCROLL, "one screen at a time": a page, not a count
 *   of lines, and the kernel has no page to measure, so ::LH_OS_INPUT_WHEEL_LINES_PAGE.
 * - Anything else as it is, `0` (wheel scrolling turned off) included; a count past
 *   ::LH_OS_INPUT_WHEEL_LINES_MAX is cut later, by ::lh_os_input_wheel_step_of.
 */
lh_u32_t
lh_os_system_win_input_wheel_lines_of(lh_os_system_win_bool_t got, lh_os_system_win_uint_t lines);

/**
 * @brief Lines one wheel notch scrolls as the system is set up now:
 *        `SystemParametersInfoA(SPI_GETWHEELSCROLLLINES)`, read by
 *        ::lh_os_system_win_input_wheel_lines_of.
 */
lh_u32_t lh_os_system_win_input_get_wheel_lines(lh_void);

/**
 * @brief The device a pointer of @p type is: ::LH_OS_SYSTEM_WIN_PT_TOUCH a touch
 *        contact, ::LH_OS_SYSTEM_WIN_PT_PEN a pen, ::LH_OS_SYSTEM_WIN_PT_MOUSE and
 *        ::LH_OS_SYSTEM_WIN_PT_TOUCHPAD the mouse; ::lh_os_input_device_kind_none for
 *        anything else (the generic ::LH_OS_SYSTEM_WIN_PT_POINTER included).
 */
lh_os_input_device_kind_t
lh_os_system_win_input_device_kind_of_pointer_type(lh_os_system_win_pointer_input_type_t type);

/**
 * @brief The pointer flags a pointer message carries in its @p wparam: the high word,
 *        which is the low half of `POINTER_FLAGS` (::LH_OS_SYSTEM_WIN_POINTER_FLAG_INCONTACT,
 *        ::LH_OS_SYSTEM_WIN_POINTER_FLAG_CANCELED).
 */
lh_os_system_win_pointer_flags_t
lh_os_system_win_input_pointer_flags_of(lh_os_system_win_wparam_t wparam);

/**
 * @brief The event a pointer @p msg with @p flags is: `WM_POINTERDOWN`
 *        ::lh_os_input_kind_pointer_down, `WM_POINTERUPDATE`
 *        ::lh_os_input_kind_pointer_move, `WM_POINTERUP` ::lh_os_input_kind_pointer_up,
 *        `WM_POINTERCAPTURECHANGED` ::lh_os_input_kind_pointer_cancel; any of them
 *        with ::LH_OS_SYSTEM_WIN_POINTER_FLAG_CANCELED a cancel too (the contact will
 *        not end in an up worth acting on). ::lh_os_input_kind_none for any other
 *        message.
 */
lh_os_input_kind_t
lh_os_system_win_input_pointer_kind_of(lh_u32_t msg, lh_os_system_win_pointer_flags_t flags);

/**
 * @brief True when @p extra, what `GetMessageExtraInfo` answers for a mouse message,
 *        says the system made the message out of a pen or a touch contact
 *        (`(extra & 0xFFFFFF00) == 0xFF515700`).
 */
lh_bool_t
lh_os_system_win_input_is_promoted_mouse(lh_os_system_win_lparam_t extra);

/**
 * @brief The device a mouse message with @p extra came from: a touch contact or a pen
 *        when the system made it out of one (::lh_os_system_win_input_is_promoted_mouse,
 *        bit ::LH_OS_SYSTEM_WIN_MI_WP_TOUCH telling the two apart), else the mouse.
 */
lh_os_input_device_kind_t
lh_os_system_win_input_device_kind_of_mouse(lh_os_system_win_lparam_t extra);

/**
 * @brief The modifiers held as the current message was posted (`GetKeyState`).
 */
lh_os_input_mods_t lh_os_system_win_input_get_mods(lh_void);

/**
 * @brief True when virtual key @p vk is down, or (with @p toggled) toggled on.
 */
lh_bool_t
lh_os_system_win_input_vk_held(lh_int_t vk, lh_bool_t toggled);

#endif /* LH_SRC_OS_SYSTEM_WIN_INPUT_H */
