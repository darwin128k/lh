/**
 * @file schedule.h
 * @brief ::lh_mb_schedule_t — which request of a plan is due next.
 *
 * A device card says how often each value wants refreshing: a room temperature every
 * second, a filter setting every minute. A plan (::lh_mb_plan_reads) folds the wants
 * into requests; the schedule says when each request is due again. A request's period
 * is the shortest period of the wants it carries, so a value is never polled less
 * often than it asks.
 *
 * What is on a screen is watched (::lh_mb_schedule_watch): a watched want is polled at
 * the fast period, and becoming watched makes its request due at once, so a screen
 * that opens shows fresh values instead of waiting out a ten-second period. Watches
 * are counted: two screens showing a value, and one closing, leave it watched.
 *
 * A request is rescheduled after every attempt, answered or not
 * (::lh_mb_schedule_mark_done): a device that has stopped answering is asked at the
 * request's period, not as fast as the loop can go. The values of a failed request
 * stay in the image, stale (::lh_mb_image_fail).
 *
 * The schedule says *what* to ask; it sends nothing. Whoever owns the bus asks it
 * (::lh_mb_schedule_find_due) when the bus is free.
 *
 * Ownership: the schedule owns its arrays (runtime allocator), filled by
 * ::lh_mb_schedule_try_assign and freed by ::lh_mb_schedule_deinit.
 *
 * @see lh_mb_plan_reads, lh_mb_image_t
 */

#ifndef LH_NET_MODBUS_SCHEDULE_H
#define LH_NET_MODBUS_SCHEDULE_H

#include <lh/core/array.h>
#include <lh/core/net/modbus.h>

/** @brief The period a want has until it is given one, in microseconds: 10 s. */
#define LH_MB_SCHEDULE_PERIOD_US (lh_cast_static(lh_u32_t, 10000000U))

/** @brief The period of a watched want, by default, in microseconds: 0.5 s. */
#define LH_MB_SCHEDULE_FAST_US (lh_cast_static(lh_u32_t, 500000U))

/** @brief Which request of a plan is due next. */
typedef struct lh_mb_schedule
{
    lh_array_t reads;   /**< ::lh_mb_read_t: the plan's requests */
    lh_array_t due;     /**< lh_u64_t per read: when it is due, master's clock, µs */
    lh_array_t period;  /**< lh_u32_t per want: how often it wants refreshing, µs */
    lh_array_t watched; /**< lh_u16_t per want: how many watch it */
    lh_u32_t fast;      /**< the period of a watched want, µs */
} lh_mb_schedule_t;     /**< Which request of a plan is due next. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The requests, an array of ::lh_mb_read_t (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_schedule_get_reads(const lh_mb_schedule_t *self);

/**
 * @brief The requests, to change (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_schedule_get_reads_mut(lh_mb_schedule_t *self);

/**
 * @brief When each request is due, lh_u64_t µs (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_schedule_get_due(const lh_mb_schedule_t *self);

/**
 * @brief When each request is due, to change (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_schedule_get_due_mut(lh_mb_schedule_t *self);

/**
 * @brief The period of each want, lh_u32_t µs (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_schedule_get_period(const lh_mb_schedule_t *self);

/**
 * @brief The period of each want, to change (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_schedule_get_period_mut(lh_mb_schedule_t *self);

/**
 * @brief How many watch each want, lh_u16_t (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
const lh_array_t *
lh_mb_schedule_get_watched(const lh_mb_schedule_t *self);

/**
 * @brief How many watch each want, to change (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
lh_array_t *
lh_mb_schedule_get_watched_mut(lh_mb_schedule_t *self);

/**
 * @brief The period of a watched want, µs (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_schedule_get_fast(const lh_mb_schedule_t *self);

/**
 * @brief Set the period of a watched want, µs (level 1).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 * @param fast The new value.
 */
lh_void
lh_mb_schedule_set_fast(lh_mb_schedule_t *self, lh_u32_t fast);

/**
 * @brief How many requests the schedule holds (level 2).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_schedule_get_count(const lh_mb_schedule_t *self);

/**
 * @brief Request @p index (level 2).
 *
 * @param self  The schedule; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_schedule_get_count; not checked.
 */
const lh_mb_read_t *
lh_mb_schedule_get_read(const lh_mb_schedule_t *self, lh_u16_t index);

/**
 * @brief When request @p index is due, µs (level 2).
 *
 * @param self  The schedule; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_schedule_get_count; not checked.
 */
lh_u64_t
lh_mb_schedule_get_due_us(const lh_mb_schedule_t *self, lh_u16_t index);

/**
 * @brief The period want @p want is polled at: the fast period when watched, its own
 *        otherwise (level 2).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 * @param want A want of the plan; not checked.
 */
lh_u32_t
lh_mb_schedule_get_want_period_us(const lh_mb_schedule_t *self, lh_u16_t want);

/**
 * @brief Set how often want @p want asks to be refreshed (level 2).
 *
 * A period of 0 keeps the one it has. Takes effect from the next
 * ::lh_mb_schedule_mark_done of its request.
 *
 * @param self      The schedule; checked by ::lh_assert_runtime_ref.
 * @param want      A want of the plan; not checked.
 * @param period_us The period, µs.
 */
lh_void
lh_mb_schedule_set_want_period_us(lh_mb_schedule_t *self, lh_u16_t want, lh_u32_t period_us);

/**
 * @brief One watch of want @p want ends; with the last one it is back to its own period
 *        from its request's next attempt.
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 * @param want A want of the plan; not checked.
 */
lh_void
lh_mb_schedule_unwatch(lh_mb_schedule_t *self, lh_u16_t want);

/**
 * @brief Start empty, with the fast period ::LH_MB_SCHEDULE_FAST_US.
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_schedule_init(lh_mb_schedule_t *self);

/**
 * @brief Free what the schedule holds; it is empty and may be assigned again.
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_schedule_deinit(lh_mb_schedule_t *self);

/**
 * @brief The period request @p index is polled at: the shortest of its wants' (level 3).
 *
 * @param self  The schedule; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_schedule_get_count; not checked.
 */
lh_u32_t
lh_mb_schedule_get_read_period_us(const lh_mb_schedule_t *self, lh_u16_t index);

/**
 * @brief The request whose wants include want @p want (level 3: binary search).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 * @param want A want of the plan.
 *
 * @return Its index, or ::lh_mb_schedule_get_count when no request carries it.
 */
lh_u16_t
lh_mb_schedule_find_read(const lh_mb_schedule_t *self, lh_u16_t want);

/**
 * @brief The most overdue request at @p now_us (level 3).
 *
 * @param self   The schedule; checked by ::lh_assert_runtime_ref.
 * @param now_us The master's clock.
 *
 * @return Its index, or ::lh_mb_schedule_get_count when none is due yet.
 */
lh_u16_t
lh_mb_schedule_find_due(const lh_mb_schedule_t *self, lh_u64_t now_us);

/**
 * @brief When the next request falls due, µs: how long a master may sleep (level 3).
 *
 * @param self The schedule; checked by ::lh_assert_runtime_ref.
 *
 * @return The earliest due time; `UINT64_MAX`-like (all ones) when there are no requests.
 */
lh_u64_t
lh_mb_schedule_get_next_due_us(const lh_mb_schedule_t *self);

/**
 * @brief Make @p self the schedule of @p read_count requests over @p want_count wants:
 *        every request due at @p now_us, every want at ::LH_MB_SCHEDULE_PERIOD_US,
 *        nothing watched.
 *
 * @param self       The schedule, initialized; checked by ::lh_assert_runtime_ref.
 * @param reads      The requests ::lh_mb_plan_reads made.
 * @param read_count How many.
 * @param want_count How many wants the plan was made of.
 * @param now_us     The master's clock.
 *
 * @return ::lh_bool_false when memory runs out; @p self is then empty.
 */
lh_bool_t
lh_mb_schedule_try_assign(lh_mb_schedule_t *self, const lh_mb_read_t *reads, lh_u16_t read_count,
                          lh_u16_t want_count, lh_u64_t now_us);

/**
 * @brief Request @p index was attempted at @p now_us, answered or not: it is due again
 *        one period later.
 *
 * @param self   The schedule; checked by ::lh_assert_runtime_ref.
 * @param index  Below ::lh_mb_schedule_get_count; not checked.
 * @param now_us The master's clock.
 */
lh_void
lh_mb_schedule_mark_done(lh_mb_schedule_t *self, lh_u16_t index, lh_u64_t now_us);

/**
 * @brief Want @p want is on a screen: poll it at the fast period, starting now.
 *
 * Watches are counted; each ::lh_mb_schedule_watch is undone by one
 * ::lh_mb_schedule_unwatch.
 *
 * @param self   The schedule; checked by ::lh_assert_runtime_ref.
 * @param want   A want of the plan; not checked.
 * @param now_us The master's clock: its request becomes due at once.
 */
lh_void
lh_mb_schedule_watch(lh_mb_schedule_t *self, lh_u16_t want, lh_u64_t now_us);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_SCHEDULE_H */
