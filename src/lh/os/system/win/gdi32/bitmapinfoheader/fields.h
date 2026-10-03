/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_bitmapinfoheader_t
 *        (`BITMAPINFOHEADER`).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_GDI32_BITMAPINFOHEADER_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_GDI32_BITMAPINFOHEADER_FIELDS_H

/**
 * @def lh_os_system_win_bitmapinfoheader_fields(dword_type, long_type, word_type)
 * @brief `BITMAPINFOHEADER`: the 40-byte description of a device-independent
 *        bitmap, in Win32's member order.
 *
 * @param dword_type `DWORD`.
 * @param long_type  `LONG` (32-bit on every Windows).
 * @param word_type  `WORD`.
 */
#define lh_os_system_win_bitmapinfoheader_fields(dword_type, long_type, word_type)                 \
    dword_type biSize;                                                                             \
    long_type biWidth;                                                                             \
    long_type biHeight;                                                                            \
    word_type biPlanes;                                                                            \
    word_type biBitCount;                                                                          \
    dword_type biCompression;                                                                      \
    dword_type biSizeImage;                                                                        \
    long_type biXPelsPerMeter;                                                                     \
    long_type biYPelsPerMeter;                                                                     \
    dword_type biClrUsed;                                                                          \
    dword_type biClrImportant

#endif /* LH_SRC_OS_SYSTEM_WIN_GDI32_BITMAPINFOHEADER_FIELDS_H */
