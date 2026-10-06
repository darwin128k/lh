/**
 * @file cb.h
 * @brief Pointer to ::lh_timer_fn.
 */

#ifndef LH_TIMER_CB_H
#define LH_TIMER_CB_H

#include <lh/timer/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_timer_cb
 * @brief Pointer to ::lh_timer_fn.
 */
#define lh_timer_cb lh_ptr_of(lh_timer_fn)

#endif /* LH_TIMER_CB_H */
