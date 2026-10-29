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

/**
 * @brief The pixels of the surface (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_byte_ptr
lh_os_system_posix_surface_get_bits(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store the pixels of the surface (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param bits The new value.
 */
lh_void
lh_os_system_posix_surface_set_bits(lh_os_system_posix_surface_t *self, lh_byte_ptr bits);

/**
 * @brief The width of the surface in pixels (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_surface_get_width(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store the width of the surface in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param width The new value.
 */
lh_void
lh_os_system_posix_surface_set_width(lh_os_system_posix_surface_t *self, int width);

/**
 * @brief The height of the surface in pixels (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_surface_get_height(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store the height of the surface in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param height The new value.
 */
lh_void
lh_os_system_posix_surface_set_height(lh_os_system_posix_surface_t *self, int height);

/**
 * @brief How many bits one pixel takes (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_surface_get_pixel_bits(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store how many bits one pixel takes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param pixel_bits The new value.
 */
lh_void
lh_os_system_posix_surface_set_pixel_bits(lh_os_system_posix_surface_t *self, int pixel_bits);

/**
 * @brief The buffer pixels are widened into for a 24-bit server; ::lh_null until needed (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t *
lh_os_system_posix_surface_get_staging(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store the buffer pixels are widened into for a 24-bit server; ::lh_null until needed
 * (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param staging The new value.
 */
lh_void
lh_os_system_posix_surface_set_staging(lh_os_system_posix_surface_t *self, lh_u32_t *staging);

/**
 * @brief How many words the staging buffer holds (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_os_system_posix_surface_get_staging_words(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store how many words the staging buffer holds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param staging_words The new value.
 */
lh_void
lh_os_system_posix_surface_set_staging_words(lh_os_system_posix_surface_t *self,
                                             lh_usize_t staging_words);

/**
 * @brief The shared-memory segment the pixels live in (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
const lh_os_system_posix_xshm_segment_info_t *
lh_os_system_posix_surface_get_segment(const lh_os_system_posix_surface_t *self);

/**
 * @brief The shared-memory segment the pixels live in, to change in place (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_system_posix_xshm_segment_info_t *
lh_os_system_posix_surface_get_segment_mut(lh_os_system_posix_surface_t *self);

/**
 * @brief The X display the shared segment is attached to (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_display_t *
lh_os_system_posix_surface_get_display(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store the X display the shared segment is attached to (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param display The new value.
 */
lh_void
lh_os_system_posix_surface_set_display(lh_os_system_posix_surface_t *self,
                                       lh_os_system_posix_display_t *display);

/**
 * @brief Whether the pixels are in shared memory (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_system_posix_surface_is_shared(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store whether the pixels are in shared memory (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param shared The new value.
 */
lh_void
lh_os_system_posix_surface_set_shared(lh_os_system_posix_surface_t *self, lh_bool_t shared);

/**
 * @brief Whether the server has not yet read the last present (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_system_posix_surface_is_pending(const lh_os_system_posix_surface_t *self);

/**
 * @brief Store whether the server has not yet read the last present (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param pending The new value.
 */
lh_void
lh_os_system_posix_surface_set_pending(lh_os_system_posix_surface_t *self, lh_bool_t pending);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_SURFACE_H */
