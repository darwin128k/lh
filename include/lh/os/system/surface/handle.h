/**
 * @file handle.h
 * @brief Opaque OS off-screen surface handle.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_SURFACE_HANDLE_H
#define LH_OS_SYSTEM_SURFACE_HANDLE_H

#include <lh/null.h>
#include <lh/ptr.h>

/**
 * @typedef lh_os_system_surface_handle_t
 * @brief Native surface object, or ::LH_OS_SYSTEM_SURFACE_HANDLE_INVALID.
 */
typedef lh_ptr lh_os_system_surface_handle_t;

/**
 * @def LH_OS_SYSTEM_SURFACE_HANDLE_INVALID
 * @brief No surface.
 */
#define LH_OS_SYSTEM_SURFACE_HANDLE_INVALID lh_null

#endif /* LH_OS_SYSTEM_SURFACE_HANDLE_H */
