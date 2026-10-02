/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_paintstruct_t (`PAINTSTRUCT`).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WINDOW_PAINTSTRUCT_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WINDOW_PAINTSTRUCT_FIELDS_H

/**
 * @def lh_os_system_win_paintstruct_fields(hdc_type, bool_type, rect_type, dword_type)
 * @brief `PAINTSTRUCT` — `BeginPaint` result handed to the window proc.
 *
 * @param hdc_type   `HDC` of the paint context.
 * @param bool_type  ::lh_bool_t or local wrapper.
 * @param rect_type  ::lh_os_system_win_rect_t (or X-macro fields).
 * @param dword_type ::lh_os_system_win_dword_t.
 */
#define lh_os_system_win_paintstruct_fields(hdc_type, bool_type, rect_type, dword_type)          \
    hdc_type   hdc;                                                                               \
    bool_type  fErase;                                                                           \
    rect_type rcPaint;                                                                          \
    rect_type rcReserved;                                                                       \
    dword_type dwReserved;                                                                      \
    dword_type fIncUpdate

#endif /* LH_SRC_OS_SYSTEM_WIN_WINDOW_PAINTSTRUCT_FIELDS_H */