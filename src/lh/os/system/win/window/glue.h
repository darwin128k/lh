/**
 * @file glue.h
 * @brief Backend-private: Win32 WndProc, class registration, and message → event.
 *
 * The remaining event-path guts of `win/window.c` — still not a public API,
 * but no longer file-local `static`.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WINDOW_GLUE_H
#define LH_SRC_OS_SYSTEM_WIN_WINDOW_GLUE_H

#include <lh/os/system/win/user32.h>
#include <lh/os/window.h>

lh_os_system_win_lresult_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_window_proc(lh_os_system_win_hwnd_t hwnd, lh_os_system_win_uint_t msg,
                             lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam);

lh_os_system_win_atom_t
lh_os_system_win_window_register_class(lh_os_system_win_hinstance_t instance);

lh_void
lh_os_system_win_window_send_pointer(lh_os_window_t *window, lh_os_input_kind_t kind,
                                     lh_os_input_button_t button, lh_os_system_win_wparam_t wparam,
                                     lh_os_system_win_lparam_t lparam);

lh_void
lh_os_system_win_window_send_button(lh_os_window_t *window, lh_os_system_win_hwnd_t hwnd,
                                    lh_os_system_win_uint_t msg, lh_os_system_win_wparam_t wparam,
                                    lh_os_system_win_lparam_t lparam);

lh_void
lh_os_system_win_window_send_wheel(lh_os_window_t *window, lh_os_system_win_hwnd_t hwnd,
                                   lh_os_system_win_uint_t msg, lh_os_system_win_wparam_t wparam,
                                   lh_os_system_win_lparam_t lparam);

lh_bool_t
lh_os_system_win_window_send_contact(lh_os_window_t *window, lh_os_system_win_hwnd_t hwnd,
                                     lh_os_system_win_uint_t msg, lh_os_system_win_wparam_t wparam,
                                     lh_os_system_win_lparam_t lparam);

lh_void
lh_os_system_win_window_send_key(lh_os_window_t *window, lh_os_system_win_uint_t msg,
                                 lh_os_system_win_wparam_t wparam, lh_os_system_win_lparam_t lparam);

#endif /* LH_SRC_OS_SYSTEM_WIN_WINDOW_GLUE_H */
