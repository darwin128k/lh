/**
 * @file fields.h
 * @brief Member fields of ::lh_rail_queue_t.
 */

#ifndef LH_RAIL_QUEUE_FIELDS_H
#define LH_RAIL_QUEUE_FIELDS_H

/**
 * @def lh_rail_queue_fields(link_type, task_type)
 * @brief The producers' end, the consumer's end and the stub task.
 *
 * @param link_type ::lh_atomic_ptr_t: `back`, the task pushed last; every
 *                  producer exchanges it.
 * @param task_type ::lh_rail_task_t: `front` points at the task the consumer
 *                  takes next (the consumer alone touches it), and `stub` is the
 *                  queue's own placeholder task, so the queue is never without
 *                  a node.
 */
#define lh_rail_queue_fields(link_type, task_type)                                                 \
    link_type back;                                                                                \
    task_type *front;                                                                              \
    task_type stub

#endif /* LH_RAIL_QUEUE_FIELDS_H */
