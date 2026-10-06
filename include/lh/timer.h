/**
 * @file timer.h
 * @brief One logical timer: ::lh_timer_t.
 *
 * Period in milliseconds, optional repeat, callback + context. Lives in a
 * ::lh_timer_group_t; the group does not own the timer. Independent of the
 * OS — the host supplies ticks to ::lh_timer_group_handler.
 */

#ifndef LH_TIMER_H
#define LH_TIMER_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/timer/cb.h>
#include <lh/timer/fields.h>
#include <lh/timer/group.h>
#include <lh/timer/tick.h>
#include <lh/void.h>

/**
 * @struct lh_timer
 * @typedef lh_timer_t
 * @brief One period callback linked into a group.
 */
struct lh_timer
{
    lh_timer_fields(lh_tick_t, lh_timer_cb);
};
typedef struct lh_timer lh_timer_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Cleared timer, not linked.
 */
lh_void
lh_timer_init(lh_timer_t *self);

/**
 * @brief Stop if running, then clear callback state.
 */
lh_void
lh_timer_deinit(lh_timer_t *self);

/**
 * @brief Arm @p self on @p group.
 *
 * @p period must be > 0. @p now is the current tick (first fire after
 * @p period). Replaces a previous start. @p cb must be non-null.
 */
lh_void
lh_timer_start(lh_timer_group_t *group, lh_timer_t *self, lh_tick_t period, lh_bool_t repeat,
               lh_timer_cb cb, lh_ptr context, lh_tick_t now);

/**
 * @brief Unlink @p self from its group. No-op when not running.
 */
lh_void
lh_timer_stop(lh_timer_t *self);

/**
 * @brief True when @p self is linked in a group.
 */
lh_bool_t
lh_timer_is_running(const lh_timer_t *self);

/**
 * @brief Pause due checks without unlinking. No-op when not running.
 */
lh_void
lh_timer_pause(lh_timer_t *self);

/**
 * @brief Resume after ::lh_timer_pause. @p now becomes the new last fire.
 */
lh_void
lh_timer_resume(lh_timer_t *self, lh_tick_t now);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIMER_H */
