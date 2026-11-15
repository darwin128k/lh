/**
 * @file surface.h
 * @brief Backend-private: an AppKit off-screen surface.
 *
 * Memory in the same `0xAARRGGBB` words the other backends present. Nothing
 * here is a window. A present draws those words into the paint it is given.
 */

#ifndef LH_SRC_OS_SYSTEM_APPLE_SURFACE_H
#define LH_SRC_OS_SYSTEM_APPLE_SURFACE_H

#include <lh/core/byte/ptr.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/index.h>
#include <lh/os/system/apple/surface/fields.h>

/** @brief One off-screen surface. */
struct lh_os_system_apple_surface
{
    /** Members: ::lh_os_system_apple_surface_fields. */
    lh_os_system_apple_surface_fields;
};
/** @copybrief lh_os_system_apple_surface */
typedef struct lh_os_system_apple_surface lh_os_system_apple_surface_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Pixel bytes (level 1). */
lh_byte_ptr
lh_os_system_apple_surface_get_bits(const lh_os_system_apple_surface_t *self);

/** @brief Store pixel bytes (level 1). The value is stored as given. */
lh_void
lh_os_system_apple_surface_set_bits(lh_os_system_apple_surface_t *self, lh_byte_ptr bits);

/** @brief Width in pixels (level 1). */
int
lh_os_system_apple_surface_get_width(const lh_os_system_apple_surface_t *self);

/** @brief Store the width (level 1). The value is stored as given. */
lh_void
lh_os_system_apple_surface_set_width(lh_os_system_apple_surface_t *self, int width);

/** @brief Height in pixels (level 1). */
int
lh_os_system_apple_surface_get_height(const lh_os_system_apple_surface_t *self);

/** @brief Store the height (level 1). The value is stored as given. */
lh_void
lh_os_system_apple_surface_set_height(lh_os_system_apple_surface_t *self, int height);

/** @brief 16 or 32 (level 1). */
int
lh_os_system_apple_surface_get_pixel_bits(const lh_os_system_apple_surface_t *self);

/** @brief Store 16 or 32 (level 1). The value is stored as given. */
lh_void
lh_os_system_apple_surface_set_pixel_bits(lh_os_system_apple_surface_t *self, int pixel_bits);

/** @brief Staging words for a widen or a partial present (level 1). */
lh_u32_t *
lh_os_system_apple_surface_get_staging(const lh_os_system_apple_surface_t *self);

/** @brief Store the staging words (level 1). The value is stored as given. */
lh_void
lh_os_system_apple_surface_set_staging(lh_os_system_apple_surface_t *self, lh_u32_t *staging);

/** @brief How many staging words are allocated (level 1). */
lh_usize_t
lh_os_system_apple_surface_get_staging_words(const lh_os_system_apple_surface_t *self);

/** @brief Store how many staging words are allocated (level 1). */
lh_void
lh_os_system_apple_surface_set_staging_words(lh_os_system_apple_surface_t *self,
                                             lh_usize_t staging_words);

/** @brief The cached letterbox surface, or null (level 1). */
lh_os_system_apple_surface_t *
lh_os_system_apple_surface_get_fit(const lh_os_system_apple_surface_t *self);

/** @brief Store the cached letterbox surface (level 1). */
lh_void
lh_os_system_apple_surface_set_fit(lh_os_system_apple_surface_t *self,
                                   lh_os_system_apple_surface_t *fit);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_APPLE_SURFACE_H */
