/**
 * @file backend.h
 * @brief The contract under a window: ::lh_os_window_backend_t.
 *
 * What a window needs from below, as a table of calls. LH does not know who
 * fills it: Win32, X11 and AppKit through the system backend
 * (`lh/os/window/backend/system.h`), a board with a panel and a touch driver, a
 * test's fake, a host's own. The window calls the table; the backend calls
 * the window back through its up-calls (::lh_os_window_on_native_paint,
 * ::lh_os_window_on_native_resize, ::lh_os_window_on_native_input,
 * ::lh_os_window_on_native_tick, ::lh_os_window_on_native_destroy,
 * ::lh_os_window_zone_at).
 *
 * The backend keeps its own state per window behind one opaque pointer
 * (::lh_os_window_set_native); a window whose pointer is ::lh_null is not open.
 *
 * An application holds the table and its context
 * (::lh_os_app_install_window_backend); a window takes them when it is opened into
 * the application, and a modal child takes its parent's. Nothing is installed by
 * default.
 */

#ifndef LH_OS_WINDOW_BACKEND_H
#define LH_OS_WINDOW_BACKEND_H

#include <lh/core/timer/clock/fn.h>
#include <lh/os/window/backend/close/fn.h>
#include <lh/os/window/backend/fields.h>
#include <lh/os/window/backend/get_client_size/fn.h>
#include <lh/os/window/backend/get_paint_region/fn.h>
#include <lh/os/window/backend/get_position/fn.h>
#include <lh/os/window/backend/invalidate/fn.h>
#include <lh/os/window/backend/invalidate_rect/fn.h>
#include <lh/os/window/backend/minimize/fn.h>
#include <lh/os/window/backend/now_us/fn.h>
#include <lh/os/window/backend/open/fn.h>
#include <lh/os/window/backend/set_corner_radius/fn.h>
#include <lh/os/window/backend/set_enabled/fn.h>
#include <lh/os/window/backend/set_maximized/fn.h>
#include <lh/os/window/backend/set_timer/fn.h>

/**
 * @struct lh_os_window_backend
 * @brief The slots a window backend fills.
 */
struct lh_os_window_backend
{
    /** Members: ::lh_os_window_backend_fields. */
    lh_os_window_backend_fields(
        lh_os_window_backend_open_fn, lh_os_window_backend_close_fn,
        lh_os_window_backend_invalidate_fn, lh_os_window_backend_invalidate_rect_fn,
        lh_timer_clock_fn, lh_os_window_backend_set_timer_fn, lh_os_window_backend_set_enabled_fn,
        lh_os_window_backend_minimize_fn, lh_os_window_backend_set_maximized_fn,
        lh_os_window_backend_set_corner_radius_fn, lh_os_window_backend_get_client_size_fn,
        lh_os_window_backend_get_position_fn, lh_os_window_backend_get_paint_region_fn,
        lh_os_window_backend_now_us_fn);
};
/** @copybrief lh_os_window_backend */
typedef struct lh_os_window_backend lh_os_window_backend_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The `open` slot of @p self (level 1): create and show the backend's window.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_open_fn *
lh_os_window_backend_get_open(const lh_os_window_backend_t *self);

/**
 * @brief Store the `open` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_open(lh_os_window_backend_t *self, lh_os_window_backend_open_fn *fn);

/**
 * @brief The `close` slot of @p self (level 1): destroy it.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_close_fn *
lh_os_window_backend_get_close(const lh_os_window_backend_t *self);

/**
 * @brief Store the `close` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_close(lh_os_window_backend_t *self, lh_os_window_backend_close_fn *fn);

/**
 * @brief The `invalidate` slot of @p self (level 1): ask for a paint of the whole window.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_invalidate_fn *
lh_os_window_backend_get_invalidate(const lh_os_window_backend_t *self);

/**
 * @brief Store the `invalidate` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_invalidate(lh_os_window_backend_t *self,
                                    lh_os_window_backend_invalidate_fn *fn);

/**
 * @brief The `invalidate_rect` slot of @p self (level 1): ask for a paint of a rectangle.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_invalidate_rect_fn *
lh_os_window_backend_get_invalidate_rect(const lh_os_window_backend_t *self);

/**
 * @brief Store the `invalidate_rect` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_invalidate_rect(lh_os_window_backend_t *self,
                                         lh_os_window_backend_invalidate_rect_fn *fn);

/**
 * @brief The `now` slot of @p self (level 1): the clock the window's timers run by.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_timer_clock_fn *
lh_os_window_backend_get_now(const lh_os_window_backend_t *self);

/**
 * @brief Store the `now` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_now(lh_os_window_backend_t *self, lh_timer_clock_fn *fn);

/**
 * @brief The `set_timer` slot of @p self (level 1): run or stop the window's timer.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_set_timer_fn *
lh_os_window_backend_get_set_timer(const lh_os_window_backend_t *self);

/**
 * @brief Store the `set_timer` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_set_timer(lh_os_window_backend_t *self,
                                   lh_os_window_backend_set_timer_fn *fn);

/**
 * @brief The `set_enabled` slot of @p self (level 1): let the window take input, or not.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_set_enabled_fn *
lh_os_window_backend_get_set_enabled(const lh_os_window_backend_t *self);

/**
 * @brief Store the `set_enabled` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_set_enabled(lh_os_window_backend_t *self,
                                     lh_os_window_backend_set_enabled_fn *fn);

/**
 * @brief The `minimize` slot of @p self (level 1): minimize the window.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_minimize_fn *
lh_os_window_backend_get_minimize(const lh_os_window_backend_t *self);

/**
 * @brief Store the `minimize` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_minimize(lh_os_window_backend_t *self,
                                  lh_os_window_backend_minimize_fn *fn);

/**
 * @brief The `set_maximized` slot of @p self (level 1): maximize or restore the window.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_set_maximized_fn *
lh_os_window_backend_get_set_maximized(const lh_os_window_backend_t *self);

/**
 * @brief Store the `set_maximized` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_set_maximized(lh_os_window_backend_t *self,
                                       lh_os_window_backend_set_maximized_fn *fn);

/**
 * @brief The `set_corner_radius` slot of @p self (level 1): cut the corners of an open window.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_set_corner_radius_fn *
lh_os_window_backend_get_set_corner_radius(const lh_os_window_backend_t *self);

/**
 * @brief Store the `set_corner_radius` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_set_corner_radius(lh_os_window_backend_t *self,
                                           lh_os_window_backend_set_corner_radius_fn *fn);

/**
 * @brief The `get_client_size` slot of @p self (level 1): read the client size.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_get_client_size_fn *
lh_os_window_backend_get_get_client_size(const lh_os_window_backend_t *self);

/**
 * @brief Store the `get_client_size` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_get_client_size(lh_os_window_backend_t *self,
                                         lh_os_window_backend_get_client_size_fn *fn);

/**
 * @brief The `get_position` slot of @p self (level 1): read the position on the screen.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_get_position_fn *
lh_os_window_backend_get_get_position(const lh_os_window_backend_t *self);

/**
 * @brief Store the `get_position` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_get_position(lh_os_window_backend_t *self,
                                      lh_os_window_backend_get_position_fn *fn);

/**
 * @brief The `get_paint_region` slot of @p self (level 1): read what a paint was asked for.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_get_paint_region_fn *
lh_os_window_backend_get_get_paint_region(const lh_os_window_backend_t *self);

/**
 * @brief Store the `get_paint_region` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_get_paint_region(lh_os_window_backend_t *self,
                                          lh_os_window_backend_get_paint_region_fn *fn);

/**
 * @brief The `now_us` slot of @p self (level 1): a clock in microseconds, for frame stats.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the backend does not have it.
 */
lh_os_window_backend_now_us_fn *
lh_os_window_backend_get_now_us(const lh_os_window_backend_t *self);

/**
 * @brief Store the `now_us` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the backend does not have it.
 */
lh_void
lh_os_window_backend_set_now_us(lh_os_window_backend_t *self, lh_os_window_backend_now_us_fn *fn);

/**
 * @brief Empty table: every slot ::lh_null.
 *
 * A backend then sets the slots it has. The table is the backend's and is
 * usually a constant it hands out; nothing here owns it.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_os_window_backend_init(lh_os_window_backend_t *self);

/**
 * @brief True when every required slot of @p self is set: `open`, `close`, `invalidate`,
 * `invalidate_rect`, `now`.
 *
 * The other slots are optional, and each one's type says what happens
 * without it.
 *
 * @param self The table; must not be null.
 * @return True when the table can be used.
 */
lh_bool_t
lh_os_window_backend_is_complete(const lh_os_window_backend_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_H */
