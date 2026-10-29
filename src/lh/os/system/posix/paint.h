/**
 * @file paint.h
 * @brief Backend-private: what the X window backend hands a paint as its "DC".
 *
 * Windows has a device context to paint into and a surface blits into it. X has
 * a window, a graphics context and a connection, and a present needs all three,
 * so this is what `paint_dc` points to for the length of a paint on Linux —
 * owned by the window, read by the surface.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_PAINT_H
#define LH_SRC_OS_SYSTEM_POSIX_PAINT_H

#include <lh/numeric/fixed/types.h>
#include <lh/os/system/posix/x11.h>
#include <lh/os/system/posix/paint/fields.h>

/** @brief A paint in progress on one window. */
struct lh_os_system_posix_paint
{
    /** Members: ::lh_os_system_posix_paint_fields. */
    lh_os_system_posix_paint_fields;
};
/** @copybrief lh_os_system_posix_paint */
typedef struct lh_os_system_posix_paint lh_os_system_posix_paint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The Xlib function table the paint calls through (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
const lh_os_system_posix_x11_t *
lh_os_system_posix_paint_get_x11(const lh_os_system_posix_paint_t *self);

/**
 * @brief Store the Xlib function table the paint calls through (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param x11 The new value.
 */
lh_void
lh_os_system_posix_paint_set_x11(lh_os_system_posix_paint_t *self,
                                 const lh_os_system_posix_x11_t *x11);

/**
 * @brief The X display the paint is on (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_display_t *
lh_os_system_posix_paint_get_display(const lh_os_system_posix_paint_t *self);

/**
 * @brief Store the X display the paint is on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param display The new value.
 */
lh_void
lh_os_system_posix_paint_set_display(lh_os_system_posix_paint_t *self,
                                     lh_os_system_posix_display_t *display);

/**
 * @brief The drawable the paint goes into: the backing pixmap, or the window (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_xid_t
lh_os_system_posix_paint_get_window(const lh_os_system_posix_paint_t *self);

/**
 * @brief Store the drawable the paint goes into: the backing pixmap, or the window (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param window The new value.
 */
lh_void
lh_os_system_posix_paint_set_window(lh_os_system_posix_paint_t *self,
                                    lh_os_system_posix_xid_t window);

/**
 * @brief The graphics context the paint draws with (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_os_system_posix_gc_t *
lh_os_system_posix_paint_get_gc(const lh_os_system_posix_paint_t *self);

/**
 * @brief Store the graphics context the paint draws with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param gc The new value.
 */
lh_void
lh_os_system_posix_paint_set_gc(lh_os_system_posix_paint_t *self, lh_os_system_posix_gc_t *gc);

/**
 * @brief The depth of the drawable in bits (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
int
lh_os_system_posix_paint_get_depth(const lh_os_system_posix_paint_t *self);

/**
 * @brief Store the depth of the drawable in bits (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param depth The new value.
 */
lh_void
lh_os_system_posix_paint_set_depth(lh_os_system_posix_paint_t *self, int depth);

/**
 * @brief The area of the region being painted, in pixels (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_os_system_posix_paint_get_area(const lh_os_system_posix_paint_t *self);

/**
 * @brief Store the area of the region being painted, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param area The new value.
 */
lh_void
lh_os_system_posix_paint_set_area(lh_os_system_posix_paint_t *self, lh_u64_t area);

/**
 * @brief How many rectangles the region being painted has (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_os_system_posix_paint_get_rects(const lh_os_system_posix_paint_t *self);

/**
 * @brief Store how many rectangles the region being painted has (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param rects The new value.
 */
lh_void
lh_os_system_posix_paint_set_rects(lh_os_system_posix_paint_t *self, lh_u32_t rects);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_PAINT_H */
