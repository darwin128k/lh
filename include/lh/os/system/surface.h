/**
 * @file surface.h
 * @brief Kernel off-screen surface: create, size, pixels, present.
 *
 * The only place that talks to `CreateCompatibleDC` / `CreateDIBSection` /
 * `BitBlt` / `DeleteDC` (Windows). One contract on every platform; the
 * backend is picked by CMake. Holds no portable state — a handle in, a
 * result out.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_SURFACE_H
#define LH_OS_SYSTEM_SURFACE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/system/surface/handle.h>
#include <lh/ptr.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/surface.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create a surface of @p width by @p height pixels.
 *
 * @param bits 16 or 32 bits per pixel. 16 is RGB565 (five bits of red, six of
 *             green, five of blue) where the platform can be asked for it by name.
 *
 * @return Handle, or ::LH_OS_SYSTEM_SURFACE_HANDLE_INVALID on failure.
 */
lh_os_system_surface_handle_t
lh_os_system_surface_create(int width, int height, int bits);

/**
 * @brief Release @p handle. Safe with ::LH_OS_SYSTEM_SURFACE_HANDLE_INVALID.
 */
lh_void
lh_os_system_surface_destroy(lh_os_system_surface_handle_t handle);

/**
 * @brief Pixel width of @p handle, or `0` when invalid.
 */
int
lh_os_system_surface_get_width(lh_os_system_surface_handle_t handle);

/**
 * @brief Pixel height of @p handle, or `0` when invalid.
 */
int
lh_os_system_surface_get_height(lh_os_system_surface_handle_t handle);

/**
 * @brief The pixels of @p handle: `height` top-down rows of `width` 32-bit
 *        words `0xAARRGGBB` (Win32: the DIB section bits, alpha ignored by
 *        the blit), or ::lh_null when invalid.
 *
 * Finishes queued drawing on the surface first (Win32 `GdiFlush`), so the
 * words are current and safe to write.
 */
lh_ptr
lh_os_system_surface_get_pixels(lh_os_system_surface_handle_t handle);

/**
 * @brief Copy the whole surface of @p handle into @p dest with its top-left
 *        pixel at `(x, y)` — the partial case, where the surface is one strip
 *        of a larger target. `(0, 0)` presents a whole-target frame.
 *
 * @return True when the blit ran.
 */
lh_bool_t
lh_os_system_surface_present_at(lh_os_system_surface_handle_t handle, lh_ptr dest, int x, int y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_SURFACE_H */
