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
#include <lh/wstr/ptr.h>

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

/* Rounded rectangle. Right and bottom are exclusive, as in `CreateRectRgn`.
   The ellipse width and height are the corner diameters. */

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreateRoundRectRgn(lh_int_t x1, lh_int_t y1, lh_int_t x2, lh_int_t y2, lh_int_t w, lh_int_t h);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
DeleteObject(lh_os_system_win_handle_t hObject);

/* `GetStockObject`: the default UI font. Do not delete what it returns. */
#define LH_OS_SYSTEM_WIN_DEFAULT_GUI_FONT 17

/* `SetBkMode`. */
#define LH_OS_SYSTEM_WIN_TRANSPARENT 1

/* `CreatePen` style. */
#define LH_OS_SYSTEM_WIN_PS_SOLID 0

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
GetStockObject(lh_int_t i);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
SelectObject(lh_os_system_win_hdc_t hdc, lh_os_system_win_handle_t h);

/* `COLORREF` pack: 0x00BBGGRR. */
#define LH_OS_SYSTEM_WIN_RGB(r, g, b)                                                              \
    ((lh_os_system_win_dword_t)(r) | ((lh_os_system_win_dword_t)(g) << 8) |                         \
     ((lh_os_system_win_dword_t)(b) << 16))

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreateSolidBrush(lh_os_system_win_dword_t color);

/* Stock brush that follows `SetDCBrushColor`. Present since Windows 95. */
#define LH_OS_SYSTEM_WIN_DC_BRUSH 18

/* Set the color of the DC_BRUSH stock object. Present since Windows 95
   (`SetDCBrushColor` / Win95+; documented with the stock DC brush). */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
SetDCBrushColor(lh_os_system_win_hdc_t hdc, lh_os_system_win_dword_t color);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
FillRect(lh_os_system_win_hdc_t hdc, const lh_os_system_win_rect_t *lprc,
         lh_os_system_win_handle_t hbr);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreateRectRgn(lh_int_t x1, lh_int_t y1, lh_int_t x2, lh_int_t y2);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreateRectRgnIndirect(const lh_os_system_win_rect_t *lprect);

/* Change the rectangle of an existing region. Present since Windows 95. */
LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
SetRectRgn(lh_os_system_win_handle_t hrgn, lh_int_t left, lh_int_t top, lh_int_t right,
           lh_int_t bottom);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
SelectClipRgn(lh_os_system_win_hdc_t hdc, lh_os_system_win_handle_t hrgn);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_handle_t LH_OS_SYSTEM_WIN_CALL
CreatePen(lh_int_t iStyle, lh_int_t cWidth, lh_os_system_win_dword_t color);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
MoveToEx(lh_os_system_win_hdc_t hdc, lh_int_t x, lh_int_t y, lh_os_system_win_point_t *lppt);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
LineTo(lh_os_system_win_hdc_t hdc, lh_int_t x, lh_int_t y);

LH_OS_SYSTEM_WIN_IMPORT lh_int_t LH_OS_SYSTEM_WIN_CALL
SetBkMode(lh_os_system_win_hdc_t hdc, lh_int_t mode);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_dword_t LH_OS_SYSTEM_WIN_CALL
SetTextColor(lh_os_system_win_hdc_t hdc, lh_os_system_win_dword_t color);

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
TextOutW(lh_os_system_win_hdc_t hdc, lh_int_t x, lh_int_t y, lh_wstr_cptr lpString, lh_int_t c);

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

/* `BitBlt` raster op: copy source to destination. */
#define LH_OS_SYSTEM_WIN_SRCCOPY 0x00CC0020UL

LH_OS_SYSTEM_WIN_IMPORT lh_os_system_win_bool_t LH_OS_SYSTEM_WIN_CALL
BitBlt(lh_os_system_win_hdc_t hdc, lh_int_t x, lh_int_t y, lh_int_t cx, lh_int_t cy,
       lh_os_system_win_hdc_t hdcSrc, lh_int_t x1, lh_int_t y1, lh_os_system_win_dword_t rop);

#endif /* LH_SRC_OS_SYSTEM_WIN_GDI32_H */
