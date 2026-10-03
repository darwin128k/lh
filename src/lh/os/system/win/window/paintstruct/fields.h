/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_paintstruct_t (`PAINTSTRUCT`).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WINDOW_PAINTSTRUCT_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_WINDOW_PAINTSTRUCT_FIELDS_H

/**
 * @def lh_os_system_win_paintstruct_fields(hdc_type, bool_type, rect_type, byte_type)
 * @brief `PAINTSTRUCT` — `BeginPaint` result handed to the window proc, in
 *        Win32's member order: 72 bytes on Win64, 64 on Win32.
 *
 * `BeginPaint` writes all of it; a shorter struct gets the stack behind it
 * overwritten.
 *
 * @param hdc_type  `HDC` of the paint context.
 * @param bool_type `BOOL` (4 bytes: ::lh_os_system_win_bool_t, not ::lh_bool_t).
 * @param rect_type ::lh_os_system_win_rect_t.
 * @param byte_type `BYTE`, for the 32 reserved bytes.
 */
#define lh_os_system_win_paintstruct_fields(hdc_type, bool_type, rect_type, byte_type)            \
    hdc_type hdc;                                                                                 \
    bool_type fErase;                                                                             \
    rect_type rcPaint;                                                                            \
    bool_type fRestore;                                                                           \
    bool_type fIncUpdate;                                                                         \
    byte_type rgbReserved[32]

#endif /* LH_SRC_OS_SYSTEM_WIN_WINDOW_PAINTSTRUCT_FIELDS_H */