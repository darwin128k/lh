/**
 * @file gdi32.h
 * @brief Backend-private: the part of gdi32.dll the Windows window backend
 *        uses, declared by us instead of `<windows.h>`.
 *
 * Same rules as user32.h. `SetDIBitsToDevice` with a 32-bit `BI_RGB` bitmap
 * exists since Windows 95 / NT 3.1.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_GDI32_H
#define LH_SRC_OS_SYSTEM_WIN_GDI32_H

#include <lh/numeric/types.h>
#include <lh/os/system/win/gdi32/bitmapinfoheader/fields.h>
#include <lh/os/system/win/types.h>
#include <lh/os/system/win/user32.h>
#include <lh/ptr.h>

/* `BITMAPINFOHEADER`. A 32-bit `BI_RGB` bitmap has no color table, so this
   header alone is what `SetDIBitsToDevice` reads as its `BITMAPINFO`. */
struct lh_os_system_win_bitmapinfoheader
{
    lh_os_system_win_bitmapinfoheader_fields(lh_os_system_win_dword_t, lh_int_t,
                                             lh_os_system_win_word_t);
};
typedef struct lh_os_system_win_bitmapinfoheader lh_os_system_win_bitmapinfoheader_t;

/* `biCompression`: uncompressed; 32-bit pixels are bytes b, g, r, unused. */
#define LH_OS_SYSTEM_WIN_BI_RGB 0

/* `ColorUse`: the bitmap holds colors, not palette indices. */
#define LH_OS_SYSTEM_WIN_DIB_RGB_COLORS 0

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
SetDIBitsToDevice(lh_os_system_win_hdc_t hdc, lh_int_t xDest, lh_int_t yDest,
                  lh_os_system_win_dword_t w, lh_os_system_win_dword_t h, lh_int_t xSrc,
                  lh_int_t ySrc, lh_os_system_win_uint_t StartScan,
                  lh_os_system_win_uint_t cLines, const lh_ptr lpvBits,
                  const lh_os_system_win_bitmapinfoheader_t *lpbmi,
                  lh_os_system_win_uint_t ColorUse);

#endif /* LH_SRC_OS_SYSTEM_WIN_GDI32_H */
