/**
 * @file fn.h
 * @brief Timer callback function type.
 *
 * Not a pointer type by itself. ::lh_timer_cb is the pointer.
 */

#ifndef LH_TIMER_FN_H
#define LH_TIMER_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_timer;

/**
 * @typedef lh_timer_fn
 * @brief Called when a timer period elapses.
 */
typedef lh_void(lh_timer_fn)(struct lh_timer *self, lh_ptr context);

#endif /* LH_TIMER_FN_H */
