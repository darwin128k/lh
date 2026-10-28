/**
 * @file surface.h
 * @brief Backend-private: MIT-SHM share / settle / 16→32 widen for an X surface.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_SURFACE_H
#define LH_SRC_OS_SYSTEM_POSIX_SURFACE_H

#include <lh/bool.h>
#include <lh/os/system/posix/surface/fields.h>
#include <lh/size.h>
#include <lh/compiler/extern/c.h>

/** @brief A surface: its pixels and what they look like. */
struct lh_os_system_posix_surface
{
    /** Members: ::lh_os_system_posix_surface_fields. */
    lh_os_system_posix_surface_fields;
};
/** @copybrief lh_os_system_posix_surface */
typedef struct lh_os_system_posix_surface lh_os_system_posix_surface_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Put @p surface's pixels in a segment the server is attached to.
 * @return False when SHM is unavailable or the attach was refused.
 */
lh_bool_t
lh_os_system_posix_surface_share(lh_os_system_posix_surface_t *surface, lh_usize_t bytes);

/** Wait until the server has read what the last present gave it. */
lh_void
lh_os_system_posix_surface_settle(lh_os_system_posix_surface_t *surface);

/**
 * @brief Widen the present rectangle of a 16-bit surface into its staging buffer.
 * @return Staging words, or ::lh_null on alloc failure.
 */
lh_u32_t *
lh_os_system_posix_surface_widen(lh_os_system_posix_surface_t *surface, int src_x, int src_y,
                                 int width, int height);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_SURFACE_H */
