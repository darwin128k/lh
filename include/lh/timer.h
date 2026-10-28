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

#include <lh/timer/fields.h>
#include <lh/timer/group.h>

/**
 * @struct lh_timer
 * @brief One period callback linked into a group.
 */
struct lh_timer
{
    /** Members: ::lh_timer_fields. */
    lh_timer_fields(lh_tick_t, lh_timer_cb);
};
/** @copybrief lh_timer */
typedef struct lh_timer lh_timer_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Intrusive list link of @p self in its group.
 *
 * For ::lh_timer_group_t; a timer never walks the group through this.
 */
lh_list_node_t *
lh_timer_get_node_mut(lh_timer_t *self);

/**
 * @brief Const intrusive list link of @p self in its group.
 */
const lh_list_node_t *
lh_timer_get_node(const lh_timer_t *self);

/**
 * @brief The timer whose link is @p node, or ::lh_null if @p node is
 *        ::lh_null. The reverse of ::lh_timer_get_node_mut.
 */
lh_timer_t *
lh_timer_get_by_node_mut(lh_list_node_t *node);

/**
 * @brief Const timer whose link is @p node, or ::lh_null if @p node is
 *        ::lh_null. The reverse of ::lh_timer_get_node.
 */
const lh_timer_t *
lh_timer_get_by_node(const lh_list_node_t *node);

/**
 * @brief Current priority (higher runs earlier when due).
 */
lh_u8_t
lh_timer_get_priority(const lh_timer_t *self);

/** @brief Ticks between two firings. */
lh_tick_t
lh_timer_get_period(const lh_timer_t *self);

/** @brief Store the ticks between two firings. Level-1 setter: no validation. */
lh_void
lh_timer_set_period(lh_timer_t *self, lh_tick_t period);

/** @brief Tick of the last firing (or of the start); the next one is due at last + period. */
lh_tick_t
lh_timer_get_last(const lh_timer_t *self);

/** @brief Store the tick of the last firing. Level-1 setter: no validation. */
lh_void
lh_timer_set_last(lh_timer_t *self, lh_tick_t last);

/** @brief Whether the timer fires again after each period, or once. */
lh_bool_t
lh_timer_get_repeat(const lh_timer_t *self);

/** @brief Store whether the timer repeats. Level-1 setter: no validation. */
lh_void
lh_timer_set_repeat(lh_timer_t *self, lh_bool_t repeat);

/** @brief Whether the timer is paused: a paused timer keeps its place and does not fire. */
lh_bool_t
lh_timer_get_paused(const lh_timer_t *self);

/** @brief Store whether the timer is paused. Level-1 setter: no validation. */
lh_void
lh_timer_set_paused(lh_timer_t *self, lh_bool_t paused);

/** @brief Callback the group calls when the timer fires (::lh_null for none). */
lh_timer_cb
lh_timer_get_cb(const lh_timer_t *self);

/** @brief Store the callback the timer fires. Level-1 setter: no validation. */
lh_void
lh_timer_set_cb(lh_timer_t *self, lh_timer_cb cb);

/** @brief Context handed to the callback. */
lh_ptr
lh_timer_get_context(const lh_timer_t *self);

/** @brief Store the context handed to the callback. Level-1 setter: no validation. */
lh_void
lh_timer_set_context(lh_timer_t *self, lh_ptr context);

/**
 * @brief Store the priority of @p self. Level-1 setter: it does not re-link a
 *        running timer — ::lh_timer_group_set_priority does.
 */
lh_void
lh_timer_set_priority(lh_timer_t *self, lh_u8_t priority);

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
 * @brief True if @p self's priority is strictly less than @p other's.
 */
lh_bool_t
lh_timer_priority_is_less(const lh_timer_t *self, const lh_timer_t *other);

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIMER_H */
