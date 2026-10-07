/**
 * @file gdi32.h
 * @brief Backend-private: the part of gdi32.dll the off-screen surface uses
 *        (a 32-bit DIB section, a memory DC, `GdiFlush`, `BitBlt`), declared
 *        by us instead of `<windows.h>`.
 *
 * Same rules as user32.h. Everything here exists since Windows 95 / NT 3.1.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_GDI32_H
#define LH_SRC_OS_SYSTEM_WIN_GDI32_H

#include <lh/numeric/types.h>
#include <lh/os/system/win/gdi32/bitmapinfoheader/fields.h>
#include <lh/os/system/win/types.h>
#include <lh/os/system/win/user32.h>
#include <lh/ptr.h>

/* `BITMAPINFOHEADER`. A 32-bit `BI_RGB` bitmap has no color table, so this
   header alone is what `CreateDIBSection` reads as its `BITMAPINFO`. */
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

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
DeleteObject(lh_os_system_win_handle_t hObject);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
SelectObject(lh_os_system_win_hdc_t hdc, lh_os_system_win_handle_t h);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hdc_t LH_OS_SYSTEM_WIN_CALL
CreateCompatibleDC(lh_os_system_win_hdc_t hdc);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
DeleteDC(lh_os_system_win_hdc_t hdc);

/* A 32-bit top-down DIB when `biHeight` is negative. `ppvBits` receives the
   pixels. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreateDIBSection(lh_os_system_win_hdc_t hdc, const lh_os_system_win_bitmapinfoheader_t *pbmi,
                 lh_os_system_win_uint_t usage, lh_ptr *ppvBits, lh_os_system_win_handle_t hSection,
                 lh_os_system_win_dword_t offset);

/* Finish queued GDI drawing of this thread, so DIB bits can be touched
   directly. Present since Windows 95 / NT 3.1. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
GdiFlush(lh_void);

/* `BitBlt` raster op: copy source to destination. */
#define LH_OS_SYSTEM_WIN_SRCCOPY 0x00CC0020UL

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
BitBlt(lh_os_system_win_hdc_t hdc, lh_int_t x, lh_int_t y, lh_int_t cx, lh_int_t cy,
       lh_os_system_win_hdc_t hdcSrc, lh_int_t x1, lh_int_t y1, lh_os_system_win_dword_t rop);

#endif /* LH_SRC_OS_SYSTEM_WIN_GDI32_H */
