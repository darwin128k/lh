/**
 * @file tick.h
 * @brief Millisecond tick type for ::lh_timer_t (::lh_tick_t).
 */

#ifndef LH_TIMER_TICK_H
#define LH_TIMER_TICK_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/limits.h>

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

/**
 * @brief Milliseconds from @p since to @p now, across a wrap of the counter.
 *
 * `now - since` modulo 2^32: a tick that wrapped from `0xFFFFFFF0` to `0x10`
 * is `0x20` later, not four billion earlier. Valid while the real distance is
 * below 2^32 ms (about 49.7 days).
 *
 * Example usage:
 * @code{.c}
 * lh_tick_elapsed(0xFFFFFFF0U, 0x10U);  // 0x20
 * lh_tick_elapsed(100U, 150U);          // 50
 * @endcode
 */
lh_tick_t
lh_tick_elapsed(lh_tick_t since, lh_tick_t now);

/**
 * @brief True if @p self comes strictly after @p other, across a wrap of the
 *        counter.
 *
 * Wrap-aware, unlike ::lh_tick_is_greater: the distance `self - other`, read
 * as a signed 32-bit number, is positive. Valid while the two are less than
 * 2^31 ms (about 24.8 days) apart.
 *
 * Example usage:
 * @code{.c}
 * lh_tick_is_after(0x10U, 0xFFFFFFF0U);  // true: 0x20 ms after the wrap
 * lh_tick_is_after(5U, 5U);              // false
 * @endcode
 */
lh_bool_t
lh_tick_is_after(lh_tick_t self, lh_tick_t other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIMER_TICK_H */
