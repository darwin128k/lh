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

#endif /* LH_OS_WORKER_JOB_H */
