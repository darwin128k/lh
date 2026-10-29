/**
 * @file surface.c
 * @brief Win32 off-screen surface: DIB section + memory DC + BitBlt present.
 */

#include <lh/assert/runtime.h>
#include <lh/memory.h>
#include <lh/os/alloc.h>
#include <lh/os/system/surface.h>
#include <lh/os/system/win/gdi32.h>
#include <lh/os/system/win/surface.h>
#include <lh/runtime/allocator.h>
#include <lh/util/return.h>

lh_os_system_surface_handle_t
lh_os_system_surface_create(int width, int height, int bits)
{
    lh_os_system_win_surface_t *surface;
    lh_os_system_win_bitmapv5header_t info;
    lh_os_system_win_hdc_t dc;
    lh_os_system_win_handle_t bitmap;
    lh_ptr bits_pointer;

    lh_return_if(width <= 0 || height <= 0, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);

    surface = lh_ptr_rcast(lh_os_system_win_surface_t, lh_os_alloc(sizeof(lh_ptr_deref(surface))));
    lh_return_if(lh_null_eq(surface), LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);

    dc = CreateCompatibleDC(lh_null);
    if (lh_null_eq(dc))
    {
        lh_runtime_allocator_free(surface);
        return LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
    }

    lh_memory_set(lh_addr_of(info), sizeof(info), 0);
    info.bV5Header.biSize = lh_cast_static(lh_os_system_win_dword_t, sizeof(info));
    info.bV5Header.biWidth = width;
    info.bV5Header.biHeight = lh_math_neg(height); /* top-down */
    info.bV5Header.biPlanes = 1;
    info.bV5Header.biBitCount = lh_cast_static(lh_os_system_win_word_t, bits);
    info.bV5Header.biCompression = LH_OS_SYSTEM_WIN_BI_RGB;
    if (bits == 16)
    {
        /* 16 bits with nowhere to say where a channel is are RGB555, which loses a
           bit of green — the channel the eye reads a UI by. `BITMAPV5HEADER` has the
           room and `CreateDIBSection` has read it since Windows 2000, so say it. */
        info.bV5Header.biCompression = LH_OS_SYSTEM_WIN_BI_BITFIELDS;
        info.bV5RedMask = 0x0000F800U;
        info.bV5GreenMask = 0x000007E0U;
        info.bV5BlueMask = 0x0000001FU;
    }

    bits_pointer = lh_null;
    bitmap = CreateDIBSection(
        dc, lh_cast_reinterpret(const lh_os_system_win_bitmapinfo_t *, lh_addr_of(info)),
        LH_OS_SYSTEM_WIN_DIB_RGB_COLORS, lh_addr_of(bits_pointer), lh_null, 0);
    if (lh_null_eq(bitmap) || lh_null_eq(bits_pointer))
    {
        DeleteDC(dc);
        lh_runtime_allocator_free(surface);
        return LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
    }

    lh_os_system_win_surface_set_dc(surface, dc);
    lh_os_system_win_surface_set_bitmap(surface, bitmap);
    lh_os_system_win_surface_set_previous(surface, SelectObject(dc, bitmap));
    lh_os_system_win_surface_set_bits(surface, bits_pointer);
    lh_os_system_win_surface_set_width(surface, width);
    lh_os_system_win_surface_set_height(surface, height);
    return lh_cast_reinterpret(lh_os_system_surface_handle_t, surface);
}

lh_void
lh_os_system_surface_destroy(lh_os_system_surface_handle_t handle)
{
    lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle));
    surface = lh_ptr_rcast(lh_os_system_win_surface_t, handle);
    if (lh_null_ne(lh_os_system_win_surface_get_dc(surface)) &&
        lh_null_ne(lh_os_system_win_surface_get_previous(surface)))
    {
        SelectObject(lh_os_system_win_surface_get_dc(surface),
                     lh_os_system_win_surface_get_previous(surface));
    }
    if (lh_null_ne(lh_os_system_win_surface_get_bitmap(surface)))
    {
        DeleteObject(lh_os_system_win_surface_get_bitmap(surface));
    }
    if (lh_null_ne(lh_os_system_win_surface_get_dc(surface)))
    {
        DeleteDC(lh_os_system_win_surface_get_dc(surface));
    }
    lh_runtime_allocator_free(surface);
}

int
lh_os_system_surface_get_width(lh_os_system_surface_handle_t handle)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle), 0);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    return lh_os_system_win_surface_get_width(surface);
}

int
lh_os_system_surface_get_height(lh_os_system_surface_handle_t handle)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle), 0);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    return lh_os_system_win_surface_get_height(surface);
}

lh_ptr
lh_os_system_surface_get_pixels(lh_os_system_surface_handle_t handle)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle), lh_null);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    (void)GdiFlush();
    return lh_os_system_win_surface_get_bits(surface);
}

lh_ptr
lh_os_system_surface_get_dc(lh_os_system_surface_handle_t handle)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle), lh_null);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    return lh_cast_reinterpret(lh_ptr, lh_os_system_win_surface_get_dc(surface));
}

lh_bool_t
lh_os_system_surface_present_part(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y,
                                  int src_x, int src_y, int width, int height)
{
    const lh_os_system_win_surface_t *surface;
    lh_os_system_win_hdc_t dest_dc;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    lh_return_if(width <= 0 || height <= 0, lh_bool_false);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    dest_dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, dest);
    return lh_cast_static(lh_bool_t, BitBlt(dest_dc, x, y, width, height,
                                            lh_os_system_win_surface_get_dc(surface), src_x, src_y,
                                            LH_OS_SYSTEM_WIN_SRCCOPY) != 0);
}

lh_bool_t
lh_os_system_surface_present_at(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    return lh_os_system_surface_present_part(handle, dest, x, y, 0, 0,
                                             lh_os_system_win_surface_get_width(surface),
                                             lh_os_system_win_surface_get_height(surface));
}

lh_bool_t
lh_os_system_surface_present_fit(lh_os_system_surface_handle_t handle, lh_ptr dest, int dest_w,
                                 int dest_h, lh_u32_t fill)
{
    const lh_os_system_win_surface_t *surface;
    lh_os_system_win_hdc_t dest_dc;
    lh_os_system_win_handle_t brush;
    lh_os_system_win_handle_t previous;
    int dw;
    int dh;
    int ox;
    int oy;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest) || dest_w <= 0 || dest_h <= 0,
                 lh_bool_false);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    dest_dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, dest);
    if (dest_w * lh_os_system_win_surface_get_height(surface) >
        dest_h * lh_os_system_win_surface_get_width(surface))
    {
        dh = dest_h;
        dw = dest_h * lh_os_system_win_surface_get_width(surface) /
             lh_os_system_win_surface_get_height(surface);
    }
    else
    {
        dw = dest_w;
        dh = dest_w * lh_os_system_win_surface_get_height(surface) /
             lh_os_system_win_surface_get_width(surface);
    }
    if (dw < 1)
    {
        dw = 1;
    }
    if (dh < 1)
    {
        dh = 1;
    }
    ox = (dest_w - dw) / 2;
    oy = (dest_h - dh) / 2;
    brush = CreateSolidBrush(fill);
    previous = SelectObject(dest_dc, brush);
    (void)PatBlt(dest_dc, 0, 0, dest_w, dest_h, LH_OS_SYSTEM_WIN_PATCOPY);
    (void)SelectObject(dest_dc, previous);
    if (lh_null_ne(brush))
    {
        (void)DeleteObject(brush);
    }
    (void)SetStretchBltMode(dest_dc, LH_OS_SYSTEM_WIN_COLORONCOLOR);
    return lh_cast_static(lh_bool_t, StretchBlt(dest_dc, ox, oy, dw, dh,
                                                lh_os_system_win_surface_get_dc(surface), 0, 0,
                                                lh_os_system_win_surface_get_width(surface),
                                                lh_os_system_win_surface_get_height(surface),
                                                LH_OS_SYSTEM_WIN_SRCCOPY) != 0);
}

lh_os_system_win_hdc_t
lh_os_system_win_surface_get_dc(const lh_os_system_win_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dc;
}

lh_void
lh_os_system_win_surface_set_dc(lh_os_system_win_surface_t *self, lh_os_system_win_hdc_t dc)
{
    lh_assert_runtime_ref(self);
    self->dc = dc;
}

lh_os_system_win_handle_t
lh_os_system_win_surface_get_bitmap(const lh_os_system_win_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bitmap;
}

lh_void
lh_os_system_win_surface_set_bitmap(lh_os_system_win_surface_t *self,
                                    lh_os_system_win_handle_t bitmap)
{
    lh_assert_runtime_ref(self);
    self->bitmap = bitmap;
}

lh_os_system_win_handle_t
lh_os_system_win_surface_get_previous(const lh_os_system_win_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->previous;
}

lh_void
lh_os_system_win_surface_set_previous(lh_os_system_win_surface_t *self,
                                      lh_os_system_win_handle_t previous)
{
    lh_assert_runtime_ref(self);
    self->previous = previous;
}

lh_ptr
lh_os_system_win_surface_get_bits(const lh_os_system_win_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bits;
}

lh_void
lh_os_system_win_surface_set_bits(lh_os_system_win_surface_t *self, lh_ptr bits)
{
    lh_assert_runtime_ref(self);
    self->bits = bits;
}

int
lh_os_system_win_surface_get_width(const lh_os_system_win_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_void
lh_os_system_win_surface_set_width(lh_os_system_win_surface_t *self, int width)
{
    lh_assert_runtime_ref(self);
    self->width = width;
}

int
lh_os_system_win_surface_get_height(const lh_os_system_win_surface_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_void
lh_os_system_win_surface_set_height(lh_os_system_win_surface_t *self, int height)
{
    lh_assert_runtime_ref(self);
    self->height = height;
}
