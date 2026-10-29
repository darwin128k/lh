/**
 * @file worker.h
 * @brief Bounded serial background worker with caller-dispatched completions.
 *
 * Submit/take/stop/destroy require one externally coordinated caller thread.
 * Work runs on the worker; complete is never invoked by LH. A successful submit
 * transfers payload cleanup to the queue. Rejection leaves ownership with caller.
 * Capacity counts pending, running and completed jobs until taken. Taking a job
 * transfers it to the caller, who may call complete and must call dispose once.
 * Stop rejects submissions, joins running work and cancels pending work; destroy
 * disposes all remaining payloads on its caller without invoking complete.
 * Keep a stopped worker alive while destructors may attempt resubmission.
 * Synchronization invariant failures terminate rather than free live state.
 */

#ifndef LH_OS_WORKER_H
#define LH_OS_WORKER_H

#include <lh/os/worker/job.h>
#include <lh/os/condition.h>
#include <lh/os/thread.h>

/** @brief Opaque owner of a bounded serial background worker. */
typedef struct lh_os_worker lh_os_worker_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create a worker with a nonzero outstanding-job capacity.
 *
 * Returns ::lh_null on allocation or native startup failure. Captures the caller runtime allocator;
 * its context must outlive the worker and taken jobs.
 */
lh_os_worker_t *
lh_os_worker_create(lh_uint_t capacity);

/**
 * @brief Transfer a job to the worker if capacity permits.
 *
 * Returns ::lh_bool_false without taking payload ownership on rejection. work must be non-null;
 * complete and destroy may be ::lh_null.
 */
lh_bool_t
lh_os_worker_submit(lh_os_worker_t *self, lh_os_worker_cb work, lh_os_worker_cb complete,
                    lh_os_worker_cb destroy, lh_ptr user);

/**
 * @brief Take the next completed job without waiting.
 *
 * Returns ::lh_null when no completion is ready. The caller owns a returned record and must dispose
 * it exactly once.
 */
lh_os_worker_job_t *
lh_os_worker_take(lh_os_worker_t *self);

/**
 * @brief Invoke the optional payload destructor and release a taken record.
 *
 * job must be non-null. Runs destroy on the caller thread using the allocator captured when the job
 * was submitted.
 */
lh_void
lh_os_worker_dispose(lh_os_worker_job_t *job);

/**
 * @brief Reject submissions and join running work.
 *
 * self must be non-null. May be repeated. Pending jobs never run after stop; they remain owned
 * until destroy. Do not call from the worker callback.
 */
lh_void
lh_os_worker_stop(lh_os_worker_t *self);

/**
 * @brief Stop the worker and dispose all remaining jobs.
 *
 * A null self is ignored. Does not invoke completions. Payload destructors run before the stopped
 * worker and its synchronization objects are released.
 */
lh_void
lh_os_worker_destroy(lh_os_worker_t *self);

/**
 * @brief the first job waiting to run; ::lh_null for none (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
lh_os_worker_job_t *
lh_os_worker_get_pending(const lh_os_worker_t *self);

/**
 * @brief Store the first job waiting to run; ::lh_null for none (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @param pending The new value.
 */
lh_void
lh_os_worker_set_pending(lh_os_worker_t *self, lh_os_worker_job_t *pending);

/**
 * @brief where the next job waiting to run is linked in (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
lh_os_worker_job_t **
lh_os_worker_get_pending_tail(const lh_os_worker_t *self);

/**
 * @brief Store where the next job waiting to run is linked in (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @param pending_tail The new value.
 */
lh_void
lh_os_worker_set_pending_tail(lh_os_worker_t *self, lh_os_worker_job_t **pending_tail);

/**
 * @brief the first job that ran and waits to be taken; ::lh_null for none (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
lh_os_worker_job_t *
lh_os_worker_get_done(const lh_os_worker_t *self);

/**
 * @brief Store the first job that ran and waits to be taken; ::lh_null for none (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @param done The new value.
 */
lh_void
lh_os_worker_set_done(lh_os_worker_t *self, lh_os_worker_job_t *done);

/**
 * @brief where the next job that ran is linked in (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
lh_os_worker_job_t **
lh_os_worker_get_done_tail(const lh_os_worker_t *self);

/**
 * @brief Store where the next job that ran is linked in (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @param done_tail The new value.
 */
lh_void
lh_os_worker_set_done_tail(lh_os_worker_t *self, lh_os_worker_job_t **done_tail);

/**
 * @brief how many accepted jobs are not taken yet (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_os_worker_get_count(const lh_os_worker_t *self);

/**
 * @brief Store how many accepted jobs are not taken yet (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @param count The new value.
 */
lh_void
lh_os_worker_set_count(lh_os_worker_t *self, lh_uint_t count);

/**
 * @brief how many jobs may be outstanding at once (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_os_worker_get_capacity(const lh_os_worker_t *self);

/**
 * @brief Store how many jobs may be outstanding at once (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @param capacity The new value.
 */
lh_void
lh_os_worker_set_capacity(lh_os_worker_t *self, lh_uint_t capacity);

/**
 * @brief Whether @p self was told to stop (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_os_worker_is_stop(const lh_os_worker_t *self);

/**
 * @brief Store whether @p self is told to stop (level 1).
 *
 * A level-1 setter: the flag is stored as given. ::lh_os_worker_stop sets it under the lock.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @param stop The new flag.
 */
lh_void
lh_os_worker_set_stop(lh_os_worker_t *self, lh_bool_t stop);

/**
 * @brief The allocator the worker and its jobs come from (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
const lh_memory_allocator_t *
lh_os_worker_get_allocator(const lh_os_worker_t *self);

/**
 * @brief The allocator the worker and its jobs come from, to change in place (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_memory_allocator_t *
lh_os_worker_get_allocator_mut(lh_os_worker_t *self);

/**
 * @brief The thread the work runs on (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
const lh_os_thread_t *
lh_os_worker_get_thread(const lh_os_worker_t *self);

/**
 * @brief The thread the work runs on, to change in place (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_thread_t *
lh_os_worker_get_thread_mut(lh_os_worker_t *self);

/**
 * @brief The lock over the queues and the stop flag (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
const lh_os_mutex_t *
lh_os_worker_get_mutex(const lh_os_worker_t *self);

/**
 * @brief The lock over the queues and the stop flag, to change in place (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_mutex_t *
lh_os_worker_get_mutex_mut(lh_os_worker_t *self);

/**
 * @brief What the worker thread sleeps on (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 */
const lh_os_condition_t *
lh_os_worker_get_wake(const lh_os_worker_t *self);

/**
 * @brief What the worker thread sleeps on, to change in place (level 1).
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_condition_t *
lh_os_worker_get_wake_mut(lh_os_worker_t *self);

/**
 * @brief Where the first job waiting to run is linked in, to change in place (level 1).
 *
 * What an empty queue keeps as its tail.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_worker_job_t **
lh_os_worker_get_pending_mut(lh_os_worker_t *self);

/**
 * @brief Where the first job that ran is linked in, to change in place (level 1).
 *
 * What an empty queue keeps as its tail.
 *
 * @param self The worker; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_worker_job_t **
lh_os_worker_get_done_mut(lh_os_worker_t *self);

/**
 * @brief Level 2: take the lock of @p self; a failure terminates.
 *
 * @param self The worker.
 */
lh_void
lh_os_worker_lock(lh_os_worker_t *self);

/**
 * @brief Level 2: release the lock of @p self; a failure terminates.
 *
 * @param self The worker.
 */
lh_void
lh_os_worker_unlock(lh_os_worker_t *self);

/**
 * @brief Level 2: wake the worker thread after its queue or stop flag changed.
 *
 * Call it with the lock held; a failure terminates.
 *
 * @param self The worker.
 */
lh_void
lh_os_worker_wake(lh_os_worker_t *self);

/**
 * @brief Level 3: queue @p job to run. Call it with the lock held.
 *
 * @param self The worker.
 * @param job  The job; its next must be ::lh_null.
 */
lh_void
lh_os_worker_push_pending(lh_os_worker_t *self, lh_os_worker_job_t *job);

/**
 * @brief Level 3: unlink the first job waiting to run. Call it with the lock held.
 *
 * @param self The worker.
 * @return The job, or ::lh_null when none waits.
 */
lh_os_worker_job_t *
lh_os_worker_pop_pending(lh_os_worker_t *self);

/**
 * @brief Level 3: queue @p job as run, for ::lh_os_worker_take. Call it with the lock held.
 *
 * @param self The worker.
 * @param job  The job; its next must be ::lh_null.
 */
lh_void
lh_os_worker_push_done(lh_os_worker_t *self, lh_os_worker_job_t *job);

/**
 * @brief Level 3: unlink the first job that ran. Call it with the lock held.
 *
 * @param self The worker.
 * @return The job, or ::lh_null when none ran.
 */
lh_os_worker_job_t *
lh_os_worker_pop_done(lh_os_worker_t *self);

/**
 * @brief Level 4: the body of the worker thread: run jobs until told to stop.
 *
 * An ::lh_os_thread_fn; ::lh_os_worker_create starts it.
 *
 * @param self The worker, as ::lh_ptr.
 */
lh_void
lh_os_worker_run(lh_ptr self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WORKER_H */
