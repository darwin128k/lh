/**
 * @file paint.h
 * @brief Backend-private: what the AppKit window hands a paint as its "DC".
 *
 * Windows has a device context. AppKit has the current graphics context of the
 * view that is drawing. This is what `paint_dc` points to for the length of a
 * paint — owned by the window, read by the surface.
 */

#ifndef LH_SRC_OS_SYSTEM_APPLE_PAINT_H
#define LH_SRC_OS_SYSTEM_APPLE_PAINT_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/null.h>
#include <lh/os/system/apple/paint/fields.h>

/** @brief A paint in progress on one window. */
struct lh_os_system_apple_paint
{
    /** Members: ::lh_os_system_apple_paint_fields. */
    lh_os_system_apple_paint_fields;
};
/** @copybrief lh_os_system_apple_paint */
typedef struct lh_os_system_apple_paint lh_os_system_apple_paint_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The graphics context the paint draws into (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_system_apple_paint_get_context(const lh_os_system_apple_paint_t *self);

/**
 * @brief Store the graphics context the paint draws into (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param context The new value.
 */
lh_void
lh_os_system_apple_paint_set_context(lh_os_system_apple_paint_t *self, lh_ptr context);

/**
 * @brief The area of the region being painted, in pixels (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_os_system_apple_paint_get_area(const lh_os_system_apple_paint_t *self);

/**
 * @brief Store the area of the region being painted, in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param area The new value.
 */
lh_void
lh_os_system_apple_paint_set_area(lh_os_system_apple_paint_t *self, lh_u64_t area);

/**
 * @brief How many rectangles the region being painted has (level 1).
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_os_system_apple_paint_get_rects(const lh_os_system_apple_paint_t *self);

/**
 * @brief Store how many rectangles the region being painted has (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The paint; checked by ::lh_assert_runtime_ref.
 * @param rects The new value.
 */
lh_void
lh_os_system_apple_paint_set_rects(lh_os_system_apple_paint_t *self, lh_u32_t rects);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_APPLE_PAINT_H */
