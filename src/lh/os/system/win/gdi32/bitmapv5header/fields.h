/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_win_bitmapv5header_t
 *        (`BITMAPV5HEADER`).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_GDI32_BITMAPV5HEADER_FIELDS_H
#define LH_SRC_OS_SYSTEM_WIN_GDI32_BITMAPV5HEADER_FIELDS_H

/**
 * @def lh_os_system_win_bitmapv5header_fields(header_type, dword_type, long_type, word_type, byte_type)
 * @brief `BITMAPV5HEADER`: the 124-byte description of a device-independent bitmap
 *        that says where each channel *is*.
 *
 * @param header_type `BITMAPINFOHEADER`, reused whole as the first 40 bytes.
 * @param dword_type  `DWORD`.
 * @param long_type   `LONG` (32-bit on every Windows).
 * @param word_type   `WORD`.
 * @param byte_type   `BYTE`.
 *
 * The plain `BITMAPINFOHEADER` has nowhere to put a mask, so a 16-bit DIB created
 * with it is RGB555 whatever the caller meant. `BITMAPV5HEADER` is the same header
 * with `bV5RedMask` / `bV5GreenMask` / `bV5BlueMask` after it, and `CreateDIBSection`
 * has read those since Windows 2000. `bV5Endpoints` is a `CIEXYZTRIPLE` — three
 * `CIEXYZ`, each three `LONG`s, so **36 bytes and not 36 of anything wider**: getting
 * that wrong makes the header 232 bytes, `CreateDIBSection` refuses the `biSize` it is
 * handed, and every frame after it draws into nothing.
 */
#define lh_os_system_win_bitmapv5header_fields(header_type, dword_type, long_type, word_type, byte_type)  \
    header_type bV5Header;                                                                            \
    dword_type bV5RedMask;                                                                            \
    dword_type bV5GreenMask;                                                                          \
    dword_type bV5BlueMask;                                                                           \
    dword_type bV5AlphaMask;                                                                          \
    dword_type bV5CSType;                                                                             \
    byte_type bV5Endpoints[36];                                                                       \
    dword_type bV5GammaRed;                                                                           \
    dword_type bV5GammaGreen;                                                                         \
    dword_type bV5GammaBlue;                                                                          \
    dword_type bV5Intent;                                                                             \
    dword_type bV5ProfileData;                                                                        \
    dword_type bV5ProfileSize;                                                                        \
    dword_type bV5Reserved

#endif /* LH_SRC_OS_SYSTEM_WIN_GDI32_BITMAPV5HEADER_FIELDS_H */