/**
 * @file group.h
 * @brief A set of logical timers: ::lh_timer_group_t.
 *
 * OS-independent, LVGL-style: the host calls ::lh_timer_group_handler with
 * the current ::lh_tick_t (e.g. from ::lh_os_tick_ms). No threads.
 */

#ifndef LH_TIMER_GROUP_H
#define LH_TIMER_GROUP_H

#include <lh/compiler/extern/c.h>
#include <lh/timer/group/fields.h>
#include <lh/timer/tick.h>
#include <lh/void.h>

/**
 * @struct lh_timer_group
 * @typedef lh_timer_group_t
 * @brief List head for ::lh_timer_t nodes.
 */
struct lh_timer_group
{
    lh_timer_group_fields(lh_list_t);
};
typedef struct lh_timer_group lh_timer_group_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Empty group.
 */
lh_void
lh_timer_group_init(lh_timer_group_t *self);

/**
 * @brief Unlink every timer (does not free them), then clear @p self.
 */
lh_void
lh_timer_group_deinit(lh_timer_group_t *self);

/**
 * @brief Fire every due timer for @p now.
 *
 * Safe to stop or restart a timer from its callback. Walks with a saved next.
 */
lh_void
lh_timer_group_handler(lh_timer_group_t *self, lh_tick_t now);

/**
 * @brief Milliseconds until the next timer is due, or ::LH_TICK_T_MAX if none.
 *
 * Returns 0 when at least one timer is already due.
 */
lh_tick_t
lh_timer_group_until_next(const lh_timer_group_t *self, lh_tick_t now);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIMER_GROUP_H */
