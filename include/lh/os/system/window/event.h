/**
 * @file event.h
 * @brief What a native window reports to the application
 *        (::lh_os_system_window_set_handler).
 */

#ifndef LH_OS_SYSTEM_WINDOW_EVENT_H
#define LH_OS_SYSTEM_WINDOW_EVENT_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/os/system/window/event/fields.h>

/**
 * @def lh_os_system_window_event_close
 * @brief The user asked to close the window (its close button).
 */
#define lh_os_system_window_event_close 1U

/**
 * @def lh_os_system_window_event_paint
 * @brief The OS lost the pixels of `x, y, width, height` (the window was
 *        uncovered or restored) and they must be shown again.
 */
#define lh_os_system_window_event_paint 2U

/**
 * @def lh_os_system_window_event_resize
 * @brief The client area is now `width` x `height` pixels.
 */
#define lh_os_system_window_event_resize 3U

/**
 * @def lh_os_system_window_event_pointer_move
 * @brief The pointer moved to `x, y` over the window.
 */
#define lh_os_system_window_event_pointer_move 4U

/**
 * @def lh_os_system_window_event_pointer_down
 * @brief `button` was pressed at `x, y`.
 */
#define lh_os_system_window_event_pointer_down 5U

/**
 * @def lh_os_system_window_event_pointer_up
 * @brief `button` was released at `x, y`.
 */
#define lh_os_system_window_event_pointer_up 6U

/**
 * @struct lh_os_system_window_event
 * @brief Fields via ::lh_os_system_window_event_fields.
 */
struct lh_os_system_window_event
{
    lh_os_system_window_event_fields(lh_uint_t, lh_int_t);
};
typedef struct lh_os_system_window_event lh_os_system_window_event_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What happened: an `lh_os_system_window_event_*` value.
 */
lh_uint_t
lh_os_system_window_event_get_type(const lh_os_system_window_event_t *self);

/**
 * @brief Pointer x, or the paint area's left edge, in client pixels.
 */
lh_int_t
lh_os_system_window_event_get_x(const lh_os_system_window_event_t *self);

/**
 * @brief Pointer y, or the paint area's top edge, in client pixels.
 */
lh_int_t
lh_os_system_window_event_get_y(const lh_os_system_window_event_t *self);

/**
 * @brief New client width (resize) or paint area width.
 */
lh_int_t
lh_os_system_window_event_get_width(const lh_os_system_window_event_t *self);

/**
 * @brief New client height (resize) or paint area height.
 */
lh_int_t
lh_os_system_window_event_get_height(const lh_os_system_window_event_t *self);

/**
 * @brief The pointer button: 0 left, 1 right, 2 middle.
 */
lh_int_t
lh_os_system_window_event_get_button(const lh_os_system_window_event_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_WINDOW_EVENT_H */
