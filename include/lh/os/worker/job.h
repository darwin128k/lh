/**
 * @file job.h
 * @brief Completed work record transferred to the caller.
 */

#ifndef LH_OS_WORKER_JOB_H
#define LH_OS_WORKER_JOB_H

#include <lh/os/worker/cb.h>
#include <lh/os/worker/job/fields.h>
#include <lh/memory/allocator.h>

/**
 * @brief One accepted job; release with ::lh_os_worker_dispose after taking it.
 *
 * The caller may invoke complete(user). The next and allocator members are
 * reserved for LH; do not modify them or copy ownership of the record.
 */
struct lh_os_worker_job
{
    /** Members: ::lh_os_worker_job_fields. */
    lh_os_worker_job_fields(struct lh_os_worker_job, lh_os_worker_cb, lh_ptr,
                            lh_memory_allocator_t);
};
/** @copybrief lh_os_worker_job */
typedef struct lh_os_worker_job lh_os_worker_job_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief the next job in the queue the job is on; ::lh_null at its end (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_os_worker_job_t *
lh_os_worker_job_get_next(const lh_os_worker_job_t *self);

/**
 * @brief Store the next job in the queue the job is on; ::lh_null at its end (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @param next The new value.
 */
lh_void
lh_os_worker_job_set_next(lh_os_worker_job_t *self, lh_os_worker_job_t *next);

/**
 * @brief the work, run on the worker thread (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_os_worker_cb
lh_os_worker_job_get_work_fn(const lh_os_worker_job_t *self);

/**
 * @brief Store the work, run on the worker thread (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @param work The new value.
 */
lh_void
lh_os_worker_job_set_work_fn(lh_os_worker_job_t *self, lh_os_worker_cb work);

/**
 * @brief the completion, run by the caller after ::lh_os_worker_take (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_os_worker_cb
lh_os_worker_job_get_complete_fn(const lh_os_worker_job_t *self);

/**
 * @brief Store the completion, run by the caller after ::lh_os_worker_take (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @param complete The new value.
 */
lh_void
lh_os_worker_job_set_complete_fn(lh_os_worker_job_t *self, lh_os_worker_cb complete);

/**
 * @brief the destructor of the payload, run by ::lh_os_worker_dispose (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_os_worker_cb
lh_os_worker_job_get_destroy_fn(const lh_os_worker_job_t *self);

/**
 * @brief Store the destructor of the payload, run by ::lh_os_worker_dispose (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @param destroy The new value.
 */
lh_void
lh_os_worker_job_set_destroy_fn(lh_os_worker_job_t *self, lh_os_worker_cb destroy);

/**
 * @brief the payload every callback is given (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_worker_job_get_user(const lh_os_worker_job_t *self);

/**
 * @brief Store the payload every callback is given (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @param user The new value.
 */
lh_void
lh_os_worker_job_set_user(lh_os_worker_job_t *self, lh_ptr user);

/**
 * @brief The allocator the job record came from and goes back to (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 */
const lh_memory_allocator_t *
lh_os_worker_job_get_allocator(const lh_os_worker_job_t *self);

/**
 * @brief The allocator the job record came from and goes back to, to change in place (level 1).
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_memory_allocator_t *
lh_os_worker_job_get_allocator_mut(lh_os_worker_job_t *self);

/**
 * @brief Where the next job is linked in, to change in place (level 1).
 *
 * What a queue keeps as its tail.
 *
 * @param self The job; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_worker_job_t **
lh_os_worker_job_get_next_mut(lh_os_worker_job_t *self);

/**
 * @brief Level 2: run the work of @p self with its payload.
 *
 * @param self The job; its work must be set (not checked).
 */
lh_void
lh_os_worker_job_run(lh_os_worker_job_t *self);

/**
 * @brief Level 2: run the completion of @p self with its payload, if it has one.
 *
 * The caller does this on its own thread after ::lh_os_worker_take.
 *
 * @param self The job.
 */
lh_void
lh_os_worker_job_complete(lh_os_worker_job_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WORKER_JOB_H */
