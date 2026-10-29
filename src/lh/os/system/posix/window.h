/**
 * @file window.h
 * @brief Backend-private: ::lh_os_system_posix_window_t, one X11 window of ours.
 *
 * What the X11 backend keeps per window that the server does not: the window it
 * stands for, its backing pixmap and pictures, its size and corner, whether it is
 * enabled and maximized, the area waiting to be painted, and its tick. Not a public
 * API; reached only through the level-1 accessors below.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_WINDOW_H
#define LH_SRC_OS_SYSTEM_POSIX_WINDOW_H

#include <lh/bool.h>
#include <lh/os/system/posix/window/fields.h>
#include <lh/os/system/posix/x11.h>

struct lh_os_window;

/** @brief One window of ours. */
struct lh_os_system_posix_window
{
    /** Members: ::lh_os_system_posix_window_fields. */
    lh_os_system_posix_window_fields;
};
/** @copybrief lh_os_system_posix_window */
typedef struct lh_os_system_posix_window lh_os_system_posix_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The next window in the list of our windows; ::lh_null at its end (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
struct lh_os_system_posix_window *
lh_os_system_posix_window_get_next(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the next window in the list of our windows; ::lh_null at its end (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param next The new value.
 */
lh_void
lh_os_system_posix_window_set_next(lh_os_system_posix_window_t *self,
                                   struct lh_os_system_posix_window *next);

/**
 * @brief The portable window this one stands for (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
struct lh_os_window *
lh_os_system_posix_window_get_user(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the portable window this one stands for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param user The new value.
 */
lh_void
lh_os_system_posix_window_set_user(lh_os_system_posix_window_t *self, struct lh_os_window *user);

/**
 * @brief The window this one is modal to; ::lh_null for none (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
struct lh_os_system_posix_window *
lh_os_system_posix_window_get_owner(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the window this one is modal to; ::lh_null for none (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param owner The new value.
 */
lh_void
lh_os_system_posix_window_set_owner(lh_os_system_posix_window_t *self,
                                    struct lh_os_system_posix_window *owner);

/**
 * @brief The X window (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_xid_t
lh_os_system_posix_window_get_window(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the X window (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param window The new value.
 */
lh_void
lh_os_system_posix_window_set_window(lh_os_system_posix_window_t *self,
                                     lh_os_system_posix_xid_t window);

/**
 * @brief The pixmap the window is painted into first (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_xid_t
lh_os_system_posix_window_get_backing(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the pixmap the window is painted into first (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param backing The new value.
 */
lh_void
lh_os_system_posix_window_set_backing(lh_os_system_posix_window_t *self,
                                      lh_os_system_posix_xid_t backing);

/**
 * @brief The Render picture over the backing pixmap (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_xid_t
lh_os_system_posix_window_get_backing_picture(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the Render picture over the backing pixmap (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param backing_picture The new value.
 */
lh_void
lh_os_system_posix_window_set_backing_picture(lh_os_system_posix_window_t *self,
                                              lh_os_system_posix_xid_t backing_picture);

/**
 * @brief The Render picture over the window (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_xid_t
lh_os_system_posix_window_get_window_picture(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the Render picture over the window (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param window_picture The new value.
 */
lh_void
lh_os_system_posix_window_set_window_picture(lh_os_system_posix_window_t *self,
                                             lh_os_system_posix_xid_t window_picture);

/**
 * @brief The graphics context the window is painted with (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_gc_t *
lh_os_system_posix_window_get_gc(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the graphics context the window is painted with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param gc The new value.
 */
lh_void
lh_os_system_posix_window_set_gc(lh_os_system_posix_window_t *self, lh_os_system_posix_gc_t *gc);

/**
 * @brief The width of the window in pixels (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_window_get_width(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the width of the window in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param width The new value.
 */
lh_void
lh_os_system_posix_window_set_width(lh_os_system_posix_window_t *self, int width);

/**
 * @brief The height of the window in pixels (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_window_get_height(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the height of the window in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param height The new value.
 */
lh_void
lh_os_system_posix_window_set_height(lh_os_system_posix_window_t *self, int height);

/**
 * @brief The corner radius in pixels; 0 for square corners (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_window_get_corner(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the corner radius in pixels; 0 for square corners (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param corner The new value.
 */
lh_void
lh_os_system_posix_window_set_corner(lh_os_system_posix_window_t *self, int corner);

/**
 * @brief The colormap of the window (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_xid_t
lh_os_system_posix_window_get_colormap(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the colormap of the window (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param colormap The new value.
 */
lh_void
lh_os_system_posix_window_set_colormap(lh_os_system_posix_window_t *self,
                                       lh_os_system_posix_xid_t colormap);

/**
 * @brief Whether the window takes input (false while a modal child is open) (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_system_posix_window_is_enabled(const lh_os_system_posix_window_t *self);

/**
 * @brief Store whether the window takes input (false while a modal child is open) (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param enabled The new value.
 */
lh_void
lh_os_system_posix_window_set_enabled(lh_os_system_posix_window_t *self, lh_bool_t enabled);

/**
 * @brief Whether the window is maximized (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_system_posix_window_is_maximized(const lh_os_system_posix_window_t *self);

/**
 * @brief Store whether the window is maximized (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param maximized The new value.
 */
lh_void
lh_os_system_posix_window_set_maximized(lh_os_system_posix_window_t *self, lh_bool_t maximized);

/**
 * @brief Whether part of the window waits to be painted (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_system_posix_window_is_dirty(const lh_os_system_posix_window_t *self);

/**
 * @brief Store whether part of the window waits to be painted (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param dirty The new value.
 */
lh_void
lh_os_system_posix_window_set_dirty(lh_os_system_posix_window_t *self, lh_bool_t dirty);

/**
 * @brief The left edge of the area waiting to be painted (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_window_get_dirty_left(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the left edge of the area waiting to be painted (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param dirty_left The new value.
 */
lh_void
lh_os_system_posix_window_set_dirty_left(lh_os_system_posix_window_t *self, int dirty_left);

/**
 * @brief The top edge of the area waiting to be painted (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_window_get_dirty_top(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the top edge of the area waiting to be painted (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param dirty_top The new value.
 */
lh_void
lh_os_system_posix_window_set_dirty_top(lh_os_system_posix_window_t *self, int dirty_top);

/**
 * @brief The right edge of the area waiting to be painted (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_window_get_dirty_right(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the right edge of the area waiting to be painted (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param dirty_right The new value.
 */
lh_void
lh_os_system_posix_window_set_dirty_right(lh_os_system_posix_window_t *self, int dirty_right);

/**
 * @brief The bottom edge of the area waiting to be painted (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_window_get_dirty_bottom(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the bottom edge of the area waiting to be painted (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param dirty_bottom The new value.
 */
lh_void
lh_os_system_posix_window_set_dirty_bottom(lh_os_system_posix_window_t *self, int dirty_bottom);

/**
 * @brief The tick period in milliseconds; 0 for no tick (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_os_system_posix_window_get_tick_ms(const lh_os_system_posix_window_t *self);

/**
 * @brief Store the tick period in milliseconds; 0 for no tick (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param tick_ms The new value.
 */
lh_void
lh_os_system_posix_window_set_tick_ms(lh_os_system_posix_window_t *self, lh_u32_t tick_ms);

/**
 * @brief When the next tick is due, in microseconds (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_os_system_posix_window_get_tick_due_us(const lh_os_system_posix_window_t *self);

/**
 * @brief Store when the next tick is due, in microseconds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @param tick_due_us The new value.
 */
lh_void
lh_os_system_posix_window_set_tick_due_us(lh_os_system_posix_window_t *self, lh_u64_t tick_due_us);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_WINDOW_H */
