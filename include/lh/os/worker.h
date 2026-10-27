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

#include <lh/os/system/concurrency.h>
#include <lh/os/worker/job.h>

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WORKER_H */
