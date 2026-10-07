/**
 * @file surface.h
 * @brief Off-screen draw target: ::lh_ui_surface_t.
 *
 * A surface is a standalone pixel buffer the canvas backends can paint into,
 * then present to a real target in one blit. Size and life cycle are ours;
 * the pixels live in ::lh_os_system_surface_t behind a handle.
 *
 * Requires ::LH_LIBRARY_OPTION_UI and ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_UI_SURFACE_H
#define LH_UI_SURFACE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/system/surface/handle.h>
#include <lh/ptr.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/size.h>
#include <lh/ui/surface/fields.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/surface.h requires LH_LIBRARY_OPTION_UI"
#endif

#if !LH_LIBRARY_OPTION_OS
#    error "lh/ui/surface.h requires LH_LIBRARY_OPTION_OS"
#endif

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/ui/surface.h requires LH_LIBRARY_OPTION_OS_WINDOW"
#endif

/**
 * @struct lh_ui_surface
 * @typedef lh_ui_surface_t
 * @brief Off-screen target: size plus OS handle.
 */
struct lh_ui_surface
{
    lh_ui_surface_fields(lh_ui_size_t, lh_os_system_surface_handle_t);
};
typedef struct lh_ui_surface lh_ui_surface_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty surface: size zero, no handle.
 */
lh_void
lh_ui_surface_init(lh_ui_surface_t *self);

/**
 * @brief Destroy the OS buffer of @p self and clear it.
 */
lh_void
lh_ui_surface_deinit(lh_ui_surface_t *self);

/**
 * @brief True when @p self holds a live OS buffer.
 */
lh_bool_t
lh_ui_surface_is_valid(const lh_ui_surface_t *self);

/**
 * @brief Pixel size of @p self (zero when empty).
 */
lh_ui_size_t
lh_ui_surface_get_size(const lh_ui_surface_t *self);

/**
 * @brief Recreate @p self for @p size. No-op when the size already matches
 *        and the handle is live. Drops the old buffer first when changing.
 *
 * @return True when @p self is valid at @p size afterwards.
 */
lh_bool_t
lh_ui_surface_set_size(lh_ui_surface_t *self, lh_ui_size_t size);

/**
 * @brief Platform draw target for @p self (Win32 memory DC as ::lh_ptr), or
 *        ::lh_null when invalid.
 */
lh_ptr
lh_ui_surface_get_draw_target(const lh_ui_surface_t *self);

/**
 * @brief Set @p pixmap over the pixels of @p self (::lh_os_system_surface_get_pixels),
 *        rows `width` apart. False, @p pixmap left alone, when @p self is empty.
 */
lh_bool_t
lh_ui_surface_get_pixmap(const lh_ui_surface_t *self, lh_ui_pixmap_t *pixmap);

/**
 * @brief Present @p self into @p dest (Win32 paint DC as ::lh_ptr).
 *
 * @return True when the blit ran.
 */
lh_bool_t
lh_ui_surface_present(const lh_ui_surface_t *self, lh_ptr dest);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_SURFACE_H */
