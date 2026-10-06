/**
 * @file surface.h
 * @brief Kernel off-screen surface: create, size, draw target, present.
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
 * @return Handle, or ::LH_OS_SYSTEM_SURFACE_HANDLE_INVALID on failure.
 */
lh_os_system_surface_handle_t
lh_os_system_surface_create(int width, int height);

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
 * @brief Platform draw target for @p handle (Win32: memory `HDC`), or
 *        ::lh_null when invalid.
 */
lh_ptr
lh_os_system_surface_get_draw_target(lh_os_system_surface_handle_t handle);

/**
 * @brief Copy the whole surface of @p handle into @p dest at `(0, 0)`.
 *
 * @p dest is a platform paint target (Win32: window/`BeginPaint` `HDC`).
 * @return True when the blit ran.
 */
lh_bool_t
lh_os_system_surface_present(lh_os_system_surface_handle_t handle, lh_ptr dest);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_SURFACE_H */
