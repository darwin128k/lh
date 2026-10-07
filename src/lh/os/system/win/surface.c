/**
 * @file surface.c
 * @brief Win32 off-screen surface: DIB section + memory DC + BitBlt present.
 */

#include <lh/bool.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/os/alloc.h>
#include <lh/os/system/surface.h>
#include <lh/os/system/win/gdi32.h>
#include <lh/runtime/allocator.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

struct lh_os_system_win_surface
{
    lh_os_system_win_hdc_t dc;
    lh_os_system_win_handle_t bitmap;
    lh_os_system_win_handle_t previous;
    lh_ptr bits;
    int width;
    int height;
};
typedef struct lh_os_system_win_surface lh_os_system_win_surface_t;

lh_os_system_surface_handle_t
lh_os_system_surface_create(int width, int height)
{
    lh_os_system_win_surface_t *surface;
    lh_os_system_win_bitmapinfoheader_t info;
    lh_os_system_win_hdc_t dc;
    lh_os_system_win_handle_t bitmap;
    lh_ptr bits;

    lh_return_if(width <= 0 || height <= 0, LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);

    surface = lh_ptr_rcast(lh_os_system_win_surface_t, lh_os_alloc(sizeof(*surface)));
    lh_return_if(lh_null_eq(surface), LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);

    dc = CreateCompatibleDC(lh_null);
    if (lh_null_eq(dc))
    {
        lh_runtime_allocator_free(surface);
        return LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
    }

    lh_memory_set(lh_addr_of(info), sizeof(info), 0);
    info.biSize = lh_cast_static(lh_os_system_win_dword_t, sizeof(info));
    info.biWidth = width;
    info.biHeight = -height; /* top-down */
    info.biPlanes = 1;
    info.biBitCount = 32;
    info.biCompression = LH_OS_SYSTEM_WIN_BI_RGB;

    bits = lh_null;
    bitmap = CreateDIBSection(dc, lh_addr_of(info), LH_OS_SYSTEM_WIN_DIB_RGB_COLORS, lh_addr_of(bits),
                              lh_null, 0);
    if (lh_null_eq(bitmap) || lh_null_eq(bits))
    {
        DeleteDC(dc);
        lh_runtime_allocator_free(surface);
        return LH_OS_SYSTEM_SURFACE_HANDLE_INVALID;
    }

    surface->dc = dc;
    surface->bitmap = bitmap;
    surface->previous = SelectObject(dc, bitmap);
    surface->bits = bits;
    surface->width = width;
    surface->height = height;
    return lh_cast_reinterpret(lh_os_system_surface_handle_t, surface);
}

lh_void
lh_os_system_surface_destroy(lh_os_system_surface_handle_t handle)
{
    lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle));
    surface = lh_ptr_rcast(lh_os_system_win_surface_t, handle);
    if (lh_null_ne(surface->dc) && lh_null_ne(surface->previous))
    {
        SelectObject(surface->dc, surface->previous);
    }
    if (lh_null_ne(surface->bitmap))
    {
        DeleteObject(surface->bitmap);
    }
    if (lh_null_ne(surface->dc))
    {
        DeleteDC(surface->dc);
    }
    lh_runtime_allocator_free(surface);
}

int
lh_os_system_surface_get_width(lh_os_system_surface_handle_t handle)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle), 0);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    return surface->width;
}

int
lh_os_system_surface_get_height(lh_os_system_surface_handle_t handle)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle), 0);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    return surface->height;
}

lh_ptr
lh_os_system_surface_get_draw_target(lh_os_system_surface_handle_t handle)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle), lh_null);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    return surface->dc;
}

lh_ptr
lh_os_system_surface_get_pixels(lh_os_system_surface_handle_t handle)
{
    const lh_os_system_win_surface_t *surface;

    lh_return_if(lh_null_eq(handle), lh_null);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    (void)GdiFlush();
    return surface->bits;
}

lh_bool_t
lh_os_system_surface_present(lh_os_system_surface_handle_t handle, lh_ptr dest)
{
    const lh_os_system_win_surface_t *surface;
    lh_os_system_win_hdc_t dest_dc;

    lh_return_if(lh_null_eq(handle) || lh_null_eq(dest), lh_bool_false);
    surface = lh_ptr_rcast(const lh_os_system_win_surface_t, handle);
    dest_dc = lh_cast_reinterpret(lh_os_system_win_hdc_t, dest);
    return lh_cast_static(lh_bool_t,
                          BitBlt(dest_dc, 0, 0, surface->width, surface->height, surface->dc, 0, 0,
                                 LH_OS_SYSTEM_WIN_SRCCOPY) != 0);
}
