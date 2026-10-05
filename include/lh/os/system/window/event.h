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
 * @def lh_os_system_window_event_key
 * @brief A key was pressed. `button` is the code: a character, or one of
 *        the `LH_ENTITY_KEY_*` values.
 */
#define lh_os_system_window_event_key 7U

/**
 * @def lh_os_system_window_event_tick
 * @brief A timer asked for by ::lh_os_system_window_set_tick fired.
 */
#define lh_os_system_window_event_tick 8U

/**
 * @def lh_os_system_window_event_wheel
 * @brief The wheel turned over `x, y`.
 *
 * ::lh_os_system_window_event_get_delta says how far, in
 * ::LH_OS_SYSTEM_WHEEL_NOTCH units. It is a separate event from the pointer
 * because a wheel has no button and no position of its own: it belongs to
 * whatever is under the pointer, which is what `x, y` is for.
 */
#define lh_os_system_window_event_wheel 9U

/**
 * @def lh_os_system_window_event_wheel_horizontal
 * @brief The wheel was tilted over `x, y`, the other way round from
 *        ::lh_os_system_window_event_wheel.
 *
 * A platform with one wheel that does both reports this one or that one, and a
 * caller that wants to scroll sideways has to be able to tell which it got.
 */
#define lh_os_system_window_event_wheel_horizontal 10U

/**
 * @def LH_OS_SYSTEM_WHEEL_NOTCH
 * @brief What one notch of a wheel is worth, in the units a platform reports.
 *
 * A wheel does not report pixels, it reports notches, and this is the size of
 * one of them. Windows sends 120 for a notch and a smaller number for the
 * fine movement a touchpad sends, which is why it is named here rather than
 * left to the reader: a caller that wants pixels divides by this.
 */
#define LH_OS_SYSTEM_WHEEL_NOTCH 120

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

/**
 * @brief How far the wheel turned: 0 for every event but
 *        ::lh_os_system_window_event_wheel, and in
 *        ::LH_OS_SYSTEM_WHEEL_NOTCH units when it is one.
 *
 * Positive is away from the user, so a positive delta scrolls back the way a
 * document does under a wheel pushed forward.
 */
lh_int_t
lh_os_system_window_event_get_delta(const lh_os_system_window_event_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_WINDOW_EVENT_H */
