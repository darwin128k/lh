/**
 * @file fields.h
 * @brief Member fields of ::lh_os_worker_job_t.
 */

#ifndef LH_OS_WORKER_JOB_FIELDS_H
#define LH_OS_WORKER_JOB_FIELDS_H

/**
 * @def lh_os_worker_job_fields(job_type, callback_type, payload_type, allocator_type)
 * @brief Queue link, callbacks, payload and the allocator retained for disposal.
 * @param job_type Job type used for the private queue link.
 * @param callback_type Pointer type for work, complete and destroy.
 * @param payload_type Borrowed payload pointer type.
 * @param allocator_type Allocator snapshot used to release the job record.
 */
#define lh_os_worker_job_fields(job_type, callback_type, payload_type, allocator_type)             \
    job_type *next;                                                                                \
    callback_type work;                                                                            \
    callback_type complete;                                                                        \
    callback_type destroy;                                                                         \
    payload_type user;                                                                             \
    allocator_type allocator

#endif /* LH_OS_WORKER_JOB_FIELDS_H */
