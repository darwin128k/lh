/**
 * @file tick.h
 * @brief Millisecond tick type for ::lh_timer_t (::lh_tick_t).
 */

#ifndef LH_TIMER_TICK_H
#define LH_TIMER_TICK_H

#include <lh/numeric/fixed/limits.h>
#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_tick_t
 * @brief Milliseconds since an arbitrary origin (wraps as ::lh_u32_t).
 */
typedef lh_u32_t lh_tick_t;

/**
 * @def LH_TICK_T_MAX
 * @brief Largest ::lh_tick_t value (also "wait forever" for pumps).
 */
#define LH_TICK_T_MAX LH_U32_T_MAX

#endif /* LH_TIMER_TICK_H */
