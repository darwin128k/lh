/**
 * @file hdc.c
 * @brief Win32 fill and clip for an opaque paint / memory DC.
 */

#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/system/hdc.h>
#include <lh/os/system/win/gdi32.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_os_system_hdc_fill_rect(lh_ptr hdc, int left, int top, int right, int bottom, lh_byte_t r,
                           lh_byte_t g, lh_byte_t b)
{
    lh_os_system_win_hdc_t dc;
    lh_os_system_win_rect_t area;
    lh_os_system_win_handle_t brush;

    lh_return_if(lh_null_eq(hdc));
    dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, hdc);
    area.left = left;
    area.top = top;
    area.right = right;
    area.bottom = bottom;
    brush = CreateSolidBrush(LH_OS_SYSTEM_WIN_RGB(r, g, b));
    lh_return_if(lh_null_eq(brush));
    FillRect(dc, lh_addr_of(area), brush);
    DeleteObject(brush);
}

lh_void
lh_os_system_hdc_set_clip(lh_ptr hdc, int left, int top, int right, int bottom)
{
    lh_os_system_win_hdc_t dc;
    lh_os_system_win_rect_t area;
    lh_os_system_win_handle_t region;

    lh_return_if(lh_null_eq(hdc));
    dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, hdc);
    area.left = left;
    area.top = top;
    area.right = right;
    area.bottom = bottom;
    region = CreateRectRgnIndirect(lh_addr_of(area));
    lh_return_if(lh_null_eq(region));
    SelectClipRgn(dc, region);
    DeleteObject(region);
}

lh_void
lh_os_system_hdc_clear_clip(lh_ptr hdc)
{
    lh_os_system_win_hdc_t dc;

    lh_return_if(lh_null_eq(hdc));
    dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, hdc);
    SelectClipRgn(dc, lh_null);
}
