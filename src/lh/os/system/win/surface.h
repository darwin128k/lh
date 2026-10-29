/**
 * @file surface.h
 * @brief Backend-private: ::lh_os_system_win_surface_t, one Win32 off-screen surface.
 *
 * A DIB section selected into a memory DC: what ::lh_os_system_surface_handle_t
 * points to on Windows. Not a public API; reached only through the level-1
 * accessors below.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_SURFACE_H
#define LH_SRC_OS_SYSTEM_WIN_SURFACE_H

#include <lh/os/system/win/types.h>
#include <lh/compiler/extern/c.h>

/** An off-screen surface: a DIB section selected into a memory DC. */
struct lh_os_system_win_surface
{
    lh_os_system_win_hdc_t dc;          /**< memory DC the bitmap is selected into */
    lh_os_system_win_handle_t bitmap;   /**< the DIB section */
    lh_os_system_win_handle_t previous; /**< the DC's bitmap before ours, restored on free */
    lh_ptr bits;                        /**< the DIB's pixels */
    int width;                          /**< width in pixels */
    int height;                         /**< height in pixels */
};
/** @copybrief lh_os_system_win_surface */
typedef struct lh_os_system_win_surface lh_os_system_win_surface_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The memory DC the bitmap is selected into (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_win_hdc_t
lh_os_system_win_surface_get_dc(const lh_os_system_win_surface_t *self);

/**
 * @brief Store the memory DC the bitmap is selected into (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param dc The new value.
 */
lh_void
lh_os_system_win_surface_set_dc(lh_os_system_win_surface_t *self, lh_os_system_win_hdc_t dc);

/**
 * @brief The DIB section (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_win_handle_t
lh_os_system_win_surface_get_bitmap(const lh_os_system_win_surface_t *self);

/**
 * @brief Store the DIB section (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param bitmap The new value.
 */
lh_void
lh_os_system_win_surface_set_bitmap(lh_os_system_win_surface_t *self,
                                    lh_os_system_win_handle_t bitmap);

/**
 * @brief The bitmap the DC had before ours, restored on free (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_win_handle_t
lh_os_system_win_surface_get_previous(const lh_os_system_win_surface_t *self);

/**
 * @brief Store the bitmap the DC had before ours, restored on free (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param previous The new value.
 */
lh_void
lh_os_system_win_surface_set_previous(lh_os_system_win_surface_t *self,
                                      lh_os_system_win_handle_t previous);

/**
 * @brief The pixels of the DIB (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_system_win_surface_get_bits(const lh_os_system_win_surface_t *self);

/**
 * @brief Store the pixels of the DIB (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param bits The new value.
 */
lh_void
lh_os_system_win_surface_set_bits(lh_os_system_win_surface_t *self, lh_ptr bits);

/**
 * @brief The width of the surface in pixels (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_win_surface_get_width(const lh_os_system_win_surface_t *self);

/**
 * @brief Store the width of the surface in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param width The new value.
 */
lh_void
lh_os_system_win_surface_set_width(lh_os_system_win_surface_t *self, int width);

/**
 * @brief The height of the surface in pixels (level 1).
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_win_surface_get_height(const lh_os_system_win_surface_t *self);

/**
 * @brief Store the height of the surface in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The surface; checked by ::lh_assert_runtime_ref.
 * @param height The new value.
 */
lh_void
lh_os_system_win_surface_set_height(lh_os_system_win_surface_t *self, int height);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_WIN_SURFACE_H */
