/**
 * @file meter.h
 * @brief A paint that measures the paint it wraps: ::lh_gl_frame_meter_t.
 *
 * The meter stands where a window's paint callback stands
 * (::lh_os_window_set_on_paint) and calls the real paint inside. Around the call
 * it builds one ::lh_gl_frame_t — the rectangle the paint was given, what the
 * window's backend says was asked for, and how long the paint took — and records
 * it into an ::lh_gl_frame_stats_t.
 *
 * It is a composition over the window's public contract, and nothing in the
 * window knows about it: the rectangle is ::lh_os_window_get_paint_rect, the
 * request is the backend's optional `get_paint_region` slot, the time is its
 * optional `now_us` slot. A backend without them gives a frame whose request is
 * unknown and whose time is `0` — the same numbers a platform that cannot say
 * gives.
 *
 * The meter, the stats and the wrapped paint's context are the caller's; the meter
 * must outlive the window's paints, or be detached first (set another paint).
 *
 * Example usage:
 * @code{.c}
 * lh_gl_frame_stats_init(&stats);
 * lh_gl_frame_meter_init(&meter, &stats, on_paint, &self);
 * lh_gl_frame_meter_attach(&meter, &window);
 * @endcode
 */

#ifndef LH_GL_FRAME_METER_H
#define LH_GL_FRAME_METER_H

#include <lh/gl/frame/meter/fields.h>
#include <lh/gl/frame/stats.h>
#include <lh/os/window.h>

/**
 * @struct lh_gl_frame_meter
 * @brief The stats to record into and the paint to measure.
 */
struct lh_gl_frame_meter
{
    /** Members: ::lh_gl_frame_meter_fields. */
    lh_gl_frame_meter_fields(lh_gl_frame_stats_t, lh_os_window_on_paint_cb);
};
/** @copybrief lh_gl_frame_meter */
typedef struct lh_gl_frame_meter lh_gl_frame_meter_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The stats @p self records into (level 1).
 *
 * @param self The meter; checked by ::lh_assert_runtime_ref.
 */
lh_gl_frame_stats_t *
lh_gl_frame_meter_get_stats(const lh_gl_frame_meter_t *self);

/**
 * @brief Store the stats @p self records into (level 1).
 *
 * @param self  The meter; checked by ::lh_assert_runtime_ref.
 * @param stats The caller's stats; must outlive the meter's use.
 */
lh_void
lh_gl_frame_meter_set_stats(lh_gl_frame_meter_t *self, lh_gl_frame_stats_t *stats);

/**
 * @brief The paint @p self measures (level 1).
 *
 * @param self The meter; checked by ::lh_assert_runtime_ref.
 * @return The paint, or ::lh_null when there is none to call.
 */
lh_os_window_on_paint_cb
lh_gl_frame_meter_get_on_paint_fn(const lh_gl_frame_meter_t *self);

/**
 * @brief Store the paint @p self measures (level 1).
 *
 * @param self     The meter; checked by ::lh_assert_runtime_ref.
 * @param on_paint The paint; ::lh_null measures an empty paint.
 */
lh_void
lh_gl_frame_meter_set_on_paint_fn(lh_gl_frame_meter_t *self, lh_os_window_on_paint_cb on_paint);

/**
 * @brief The context the measured paint gets (level 1).
 *
 * @param self The meter; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gl_frame_meter_get_on_paint_context(const lh_gl_frame_meter_t *self);

/**
 * @brief Store the context the measured paint gets (level 1).
 *
 * @param self    The meter; checked by ::lh_assert_runtime_ref.
 * @param context The paint's own context.
 */
lh_void
lh_gl_frame_meter_set_on_paint_context(lh_gl_frame_meter_t *self, lh_ptr context);

/**
 * @brief Read the clock of @p window's backend in microseconds.
 *
 * @param window The window; must not be null.
 * @return The backend's `now_us`, or `0` when the window has no backend or the
 *         backend has no such clock.
 */
lh_u64_t
lh_gl_frame_meter_now_us(const lh_os_window_t *window);

/**
 * @brief Fill the request of @p frame from what @p window's backend says the
 *        current paint was asked for.
 *
 * Asks the backend's `get_paint_region` with the paint target
 * (::lh_os_window_get_paint_target). Without a backend or that slot the request is
 * recorded unknown, with `0` pixels and `0` rectangles.
 *
 * @param window The window, inside its paint; must not be null.
 * @param frame  The frame to fill; must not be null.
 */
lh_void
lh_gl_frame_meter_read_request(const lh_os_window_t *window, lh_gl_frame_t *frame);

/**
 * @brief Set the drawn rectangle of @p frame to the rectangle of @p window's
 *        current paint (::lh_os_window_get_paint_rect).
 *
 * @param window The window, inside its paint; must not be null.
 * @param frame  The frame to fill; must not be null.
 */
lh_void
lh_gl_frame_meter_read_drawn(const lh_os_window_t *window, lh_gl_frame_t *frame);

/**
 * @brief Set up @p self to measure @p on_paint into @p stats.
 *
 * @param self     The meter; must not be null.
 * @param stats    The stats to record into; the caller's.
 * @param on_paint The paint to measure; may be ::lh_null.
 * @param context  The paint's own context.
 */
lh_void
lh_gl_frame_meter_init(lh_gl_frame_meter_t *self, lh_gl_frame_stats_t *stats,
                       lh_os_window_on_paint_cb on_paint, lh_ptr context);

/**
 * @brief The paint callback of a measured window (::lh_os_window_on_paint_fn).
 *
 * Reads the drawn rectangle and the request, times the measured paint with the
 * backend's clock, calls it, and records the frame into the meter's stats.
 *
 * @param window  The window being painted.
 * @param context The meter (::lh_gl_frame_meter_t); must not be null.
 */
lh_void
lh_gl_frame_meter_on_paint(lh_os_window_t *window, lh_ptr context);

/**
 * @brief Make @p self the paint of @p window
 *        (::lh_os_window_set_on_paint with ::lh_gl_frame_meter_on_paint).
 *
 * @param self   The meter; must outlive the window's paints.
 * @param window The window; must not be null.
 */
lh_void
lh_gl_frame_meter_attach(lh_gl_frame_meter_t *self, lh_os_window_t *window);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_FRAME_METER_H */
