/**
 * @file queue.h
 * @brief ::lh_mb_job_queue_t — jobs waiting for a line, first come first served.
 *
 * A ring over storage the caller lends: no allocation, a fixed bound, and a full queue
 * says so (::lh_mb_job_queue_try_push fails) instead of growing. Besides push and pop
 * it can drop every job of one owner (::lh_mb_job_queue_discard_owner): a client of a
 * gateway that went away with requests still waiting should not cost the line their
 * round trips, and the answers would have nobody to go to.
 */

#ifndef LH_NET_MODBUS_JOB_QUEUE_H
#define LH_NET_MODBUS_JOB_QUEUE_H

#include <lh/core/net/modbus/job.h>

/** @brief Jobs waiting for a line, oldest first. */
typedef struct lh_mb_job_queue
{
    lh_mb_job_t *jobs; /**< the lent storage, ::capacity jobs */
    lh_u32_t capacity; /**< how many jobs the storage holds */
    lh_u32_t head;     /**< index of the oldest job */
    lh_u32_t count;    /**< how many jobs wait */
} lh_mb_job_queue_t;   /**< Jobs waiting for a line, oldest first. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The lent storage (level 1).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 */
const lh_mb_job_t *
lh_mb_job_queue_get_jobs(const lh_mb_job_queue_t *self);

/**
 * @brief The lent storage, to change (level 1).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 */
lh_mb_job_t *
lh_mb_job_queue_get_jobs_mut(lh_mb_job_queue_t *self);

/**
 * @brief Point the queue at @p jobs (level 1).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 * @param jobs The storage.
 */
lh_void
lh_mb_job_queue_set_jobs(lh_mb_job_queue_t *self, lh_mb_job_t *jobs);

/**
 * @brief How many jobs the storage holds (level 1).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_job_queue_get_capacity(const lh_mb_job_queue_t *self);

/**
 * @brief Store how many jobs the storage holds (level 1).
 *
 * @param self     The queue; checked by ::lh_assert_runtime_ref.
 * @param capacity The new value.
 */
lh_void
lh_mb_job_queue_set_capacity(lh_mb_job_queue_t *self, lh_u32_t capacity);

/**
 * @brief Index of the oldest job in the storage (level 1).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_job_queue_get_head(const lh_mb_job_queue_t *self);

/**
 * @brief Store the index of the oldest job (level 1).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 * @param head The new value.
 */
lh_void
lh_mb_job_queue_set_head(lh_mb_job_queue_t *self, lh_u32_t head);

/**
 * @brief How many jobs wait (level 1).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_job_queue_get_count(const lh_mb_job_queue_t *self);

/**
 * @brief Store how many jobs wait (level 1).
 *
 * @param self  The queue; checked by ::lh_assert_runtime_ref.
 * @param count The new value.
 */
lh_void
lh_mb_job_queue_set_count(lh_mb_job_queue_t *self, lh_u32_t count);

/**
 * @brief Start empty over @p capacity jobs of @p jobs (level 2).
 *
 * @param self     The queue; checked by ::lh_assert_runtime_ref.
 * @param jobs     The storage; it must outlive the queue. Checked by
 *                 ::lh_assert_runtime_ref.
 * @param capacity How many jobs it holds; 0 makes a queue that is always full.
 */
lh_void
lh_mb_job_queue_init(lh_mb_job_queue_t *self, lh_mb_job_t *jobs, lh_u32_t capacity);

/**
 * @brief Whether no job waits (level 2).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_mb_job_queue_is_empty(const lh_mb_job_queue_t *self);

/**
 * @brief Whether no job fits any more (level 2).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_mb_job_queue_is_full(const lh_mb_job_queue_t *self);

/**
 * @brief The job @p index places behind the oldest one (level 2).
 *
 * @param self  The queue; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_job_queue_get_count; not checked.
 */
const lh_mb_job_t *
lh_mb_job_queue_get_job_at(const lh_mb_job_queue_t *self, lh_u32_t index);

/**
 * @brief The job @p index places behind the oldest one, to change (level 2).
 *
 * @param self  The queue; checked by ::lh_assert_runtime_ref.
 * @param index Below ::lh_mb_job_queue_get_count; not checked.
 */
lh_mb_job_t *
lh_mb_job_queue_get_job_at_mut(lh_mb_job_queue_t *self, lh_u32_t index);

/**
 * @brief Put a copy of @p job behind the others (level 3).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 * @param job  The job; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_bool_false when the queue is full; nothing changes then.
 */
lh_bool_t
lh_mb_job_queue_try_push(lh_mb_job_queue_t *self, const lh_mb_job_t *job);

/**
 * @brief Take the oldest job out into @p job (level 3).
 *
 * @param self The queue; checked by ::lh_assert_runtime_ref.
 * @param job  Where it goes; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_bool_false when the queue is empty; @p job is untouched then.
 */
lh_bool_t
lh_mb_job_queue_try_pop(lh_mb_job_queue_t *self, lh_mb_job_t *job);

/**
 * @brief Drop every job of @p owner; the others keep their order (level 3).
 *
 * @param self  The queue; checked by ::lh_assert_runtime_ref.
 * @param owner The owner whose jobs go.
 *
 * @return How many jobs were dropped.
 */
lh_u32_t
lh_mb_job_queue_discard_owner(lh_mb_job_queue_t *self, lh_u32_t owner);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_JOB_QUEUE_H */
