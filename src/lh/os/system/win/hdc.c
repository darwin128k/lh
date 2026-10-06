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

    lh_return_if(lh_null_eq(hdc));
    dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, hdc);
    area.left = left;
    area.top = top;
    area.right = right;
    area.bottom = bottom;
    /* SetDCBrushColor + stock DC_BRUSH: no CreateSolidBrush / DeleteObject.
       Present since Windows 95. */
    (void)SetDCBrushColor(dc, LH_OS_SYSTEM_WIN_RGB(r, g, b));
    FillRect(dc, lh_addr_of(area), GetStockObject(LH_OS_SYSTEM_WIN_DC_BRUSH));
}

lh_ptr
lh_os_system_hdc_region_create(lh_void)
{
    return CreateRectRgn(0, 0, 0, 0);
}

lh_void
lh_os_system_hdc_region_destroy(lh_ptr region)
{
    lh_return_if(lh_null_eq(region));
    DeleteObject(region);
}

lh_void
lh_os_system_hdc_set_clip(lh_ptr hdc, lh_ptr region, int left, int top, int right, int bottom)
{
    lh_os_system_win_hdc_t dc;

    lh_return_if(lh_null_eq(hdc) || lh_null_eq(region));
    dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, hdc);
    lh_return_if(SetRectRgn(region, left, top, right, bottom) == 0);
    SelectClipRgn(dc, region);
}

lh_void
lh_os_system_hdc_clear_clip(lh_ptr hdc)
{
    lh_os_system_win_hdc_t dc;

    lh_return_if(lh_null_eq(hdc));
    dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, hdc);
    SelectClipRgn(dc, lh_null);
}
