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
#include <lh/list/node.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>
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
 * @brief Intrusive list link of @p self in its group.
 *
 * For ::lh_timer_group_t; a timer never walks the group through this.
 */
lh_list_node_t *
lh_timer_get_node(lh_timer_t *self);

/**
 * @brief Const intrusive list link of @p self in its group.
 */
const lh_list_node_t *
lh_timer_get_node_as_const(const lh_timer_t *self);

/**
 * @brief The timer whose link is @p node, or ::lh_null if @p node is
 *        ::lh_null. The reverse of ::lh_timer_get_node.
 */
lh_timer_t *
lh_timer_get_by_node(lh_list_node_t *node);

/**
 * @brief Const timer whose link is @p node, or ::lh_null if @p node is
 *        ::lh_null. The reverse of ::lh_timer_get_node_as_const.
 */
const lh_timer_t *
lh_timer_get_by_node_as_const(const lh_list_node_t *node);

/**
 * @brief Arm @p self on @p group.
 *
 * @p period must be > 0. @p now is the current tick (first fire after
 * @p period). Linked ahead of lower-priority timers (stable among equals).
 * Replaces a previous start. @p cb must be non-null.
 */
lh_void
lh_timer_start(lh_timer_group_t *group, lh_timer_t *self, lh_tick_t period, lh_bool_t repeat,
               lh_u8_t priority, lh_timer_cb cb, lh_ptr context, lh_tick_t now);

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
 * @brief Current priority (higher runs earlier when due).
 */
lh_u8_t
lh_timer_get_priority(const lh_timer_t *self);

/**
 * @brief Set priority; re-links in @p group when running.
 */
lh_void
lh_timer_set_priority(lh_timer_group_t *group, lh_timer_t *self, lh_u8_t priority);

/**
 * @brief True if @p self and @p other have the same priority.
 */
lh_bool_t
lh_timer_priority_equals(const lh_timer_t *self, const lh_timer_t *other);

/**
 * @brief True if @p self's priority is not less than @p minimum's.
 */
lh_bool_t
lh_timer_priority_is_at_least(const lh_timer_t *self, const lh_timer_t *minimum);

/**
 * @brief True if @p self's priority is strictly less than @p other's.
 */
lh_bool_t
lh_timer_priority_is_less(const lh_timer_t *self, const lh_timer_t *other);

/**
 * @brief True if @p self's priority is strictly greater than @p other's.
 */
lh_bool_t
lh_timer_priority_is_greater(const lh_timer_t *self, const lh_timer_t *other);

/**
 * @brief ::lh_list_cmp_cb: higher priority first; equals stay stable.
 *
 * @p context is unused.
 */
lh_int_t
lh_timer_cmp_priority(const lh_list_node_t *a, const lh_list_node_t *b, lh_ptr context);

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
