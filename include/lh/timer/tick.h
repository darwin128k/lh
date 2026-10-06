/**
 * @file tick.h
 * @brief Millisecond tick type for ::lh_timer_t (::lh_tick_t).
 */

#ifndef LH_TIMER_TICK_H
#define LH_TIMER_TICK_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
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

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True if @p self and @p other are the same tick value.
 */
lh_bool_t
lh_tick_equals(lh_tick_t self, lh_tick_t other);

/**
 * @brief True if @p self is not less than @p minimum.
 *
 * Plain unsigned ::lh_u32_t order — not wrap-aware distance across overflow.
 */
lh_bool_t
lh_tick_is_at_least(lh_tick_t self, lh_tick_t minimum);

/**
 * @brief True if @p self is strictly less than @p other.
 *
 * Same unsigned order as ::lh_tick_is_at_least (not wrap-aware).
 */
lh_bool_t
lh_tick_is_less(lh_tick_t self, lh_tick_t other);

/**
 * @brief True if @p self is strictly greater than @p other.
 *
 * Same unsigned order as ::lh_tick_is_at_least (not wrap-aware).
 */
lh_bool_t
lh_tick_is_greater(lh_tick_t self, lh_tick_t other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIMER_TICK_H */
