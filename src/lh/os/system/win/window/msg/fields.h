/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_msg_t (`MSG`).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WINDOW_MSG_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WINDOW_MSG_FIELDS_H

/**
 * @def lh_os_system_win_msg_fields(hwnd_type, dword_type, wparam_type, lparam_type)
 * @brief `MSG`: a posted thread message.
 *
 * @param hwnd_type  `HWND` (or null if the message is thread-level).
 * @param dword_type  `UINT` (`DWORD` is fine on LLP64 too) — message id.
 * @param wparam_type `WPARAM`.
 * @param lparam_type `LPARAM`.
 */
#define lh_os_system_win_msg_fields(hwnd_type, dword_type, wparam_type, lparam_type)              \
    hwnd_type   hwnd;                                                                             \
    dword_type  message;                                                                          \
    wparam_type wParam;                                                                           \
    lparam_type lParam;                                                                           \
    dword_type  pt_x;                                                                             \
    dword_type  pt_y;                                                                             \
    dword_type  time

#endif /* LH_SRC_OS_SYSTEM_WIN_WINDOW_MSG_FIELDS_H */