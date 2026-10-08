/**
 * @file gdi32.h
 * @brief Backend-private: the part of gdi32.dll the window and the off-screen
 *        surface use (a 32-bit DIB section, a memory DC, `GdiFlush`, `BitBlt`, and
 *        the regions a window is cut to), declared by us instead of
 *        `<windows.h>`.
 *
 * Same rules as user32.h. Everything here exists since Windows 95 / NT 3.1.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_GDI32_H
#define LH_SRC_OS_SYSTEM_WIN_GDI32_H

#include <lh/numeric/types.h>
#include <lh/os/system/win/gdi32/bitmapinfoheader/fields.h>
#include <lh/os/system/win/gdi32/bitmapv5header/fields.h>
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

/* `BITMAPV5HEADER`: the same 40 bytes with room for the channel masks after them.
   `CreateDIBSection` reads it when `biSize` says so, so a 16-bit DIB can be the
   format the caller meant instead of RGB555. Present since Windows 2000. */
struct lh_os_system_win_bitmapv5header
{
    lh_os_system_win_bitmapv5header_fields(lh_os_system_win_bitmapinfoheader_t,
                                           lh_os_system_win_dword_t, lh_int_t, lh_os_system_win_word_t,
                                           lh_byte_t);
};
typedef struct lh_os_system_win_bitmapv5header lh_os_system_win_bitmapv5header_t;

/* `biCompression`: the masks above are meaningful, and 32-bit pixels are bytes b, g, r,
   unused. */
#define LH_OS_SYSTEM_WIN_BI_RGB 0
#define LH_OS_SYSTEM_WIN_BI_BITFIELDS 3

/* `ColorUse`: the bitmap holds colors, not palette indices. */
#define LH_OS_SYSTEM_WIN_DIB_RGB_COLORS 0

/* `RGBQUAD`, the one entry of a `BITMAPINFO`'s color table. */
struct lh_os_system_win_rgbquad
{
    lh_byte_t rgbBlue;
    lh_byte_t rgbGreen;
    lh_byte_t rgbRed;
    lh_byte_t rgbReserved;
};
typedef struct lh_os_system_win_rgbquad lh_os_system_win_rgbquad_t;

/* `BITMAPINFO`, which is what `CreateDIBSection` really takes: the header followed
   by room for one colour. A 32-bit `BI_RGB` DIB has no table and never reads it; a
   `BITMAPV5HEADER` is handed over through this pointer, which is what Win32 itself
   expects — the caller reads only as far as the header's own `biSize` says. */
struct lh_os_system_win_bitmapinfo
{
    lh_os_system_win_bitmapinfoheader_t bmiHeader;
    lh_os_system_win_rgbquad_t bmiColors[1];
};
typedef struct lh_os_system_win_bitmapinfo lh_os_system_win_bitmapinfo_t;

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
DeleteObject(lh_os_system_win_handle_t hObject);

/* A region of a rounded rectangle, the corner ellipses being `width` by `height`.
   Passed to ::SetWindowRgn to round a window's corners. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreateRoundRectRgn(lh_int_t left, lh_int_t top, lh_int_t right, lh_int_t bottom, lh_int_t width,
                   lh_int_t height);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
SelectObject(lh_os_system_win_hdc_t hdc, lh_os_system_win_handle_t h);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_hdc_t LH_OS_SYSTEM_WIN_CALL
CreateCompatibleDC(lh_os_system_win_hdc_t hdc);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
DeleteDC(lh_os_system_win_hdc_t hdc);

/* A top-down DIB when `biHeight` is negative, 16 or 32 bits per pixel: `biSize`
   and `biBitCount` in @p pbmi say which, and for 16 they are what makes it RGB565
   rather than RGB555. `ppvBits` receives the pixels. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreateDIBSection(lh_os_system_win_hdc_t hdc, const lh_os_system_win_bitmapinfo_t *pbmi,
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
