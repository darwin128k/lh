/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_wndclassexw_t (`WNDCLASSEXW`).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WINDOW_WNDCLASSEXW_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WINDOW_WNDCLASSEXW_FIELDS_H

/**
 * @def lh_os_system_win_wndclassexw_fields(dword_type, int_type, wndproc_type)
 * @brief `WNDCLASSEXW`: the window class registration record.
 *
 * XP-clean: only the original NT4-era fields (no Vista `hIconSm`).
 *
 * @param dword_type  ::lh_os_system_win_dword_t.
 * @param int_type    signed integer (cbSize / cbClsExtra / cbWndExtra).
 * @param wndproc_type ::lh_os_system_win_wndproc_t.
 */
#define lh_os_system_win_wndclassexw_fields(dword_type, int_type, wndproc_type)                  \
    dword_type   cbSize;                                                                         \
    dword_type   style;                                                                          \
    wndproc_type lpfnWndProc;                                                                    \
    int_type     cbClsExtra;                                                                     \
    int_type     cbWndExtra;                                                                     \
    lh_ptr       hInstance;                                                                      \
    lh_ptr       hIcon;                                                                          \
    lh_ptr       hCursor;                                                                        \
    lh_ptr       hbrBackground;                                                                  \
    lh_ptr       lpszMenuName;                                                                    \
    lh_ptr       lpszClassName;                                                                  \
    lh_ptr       hIconSm

#endif /* LH_SRC_OS_SYSTEM_WIN_WINDOW_WNDCLASSEXW_FIELDS_H */