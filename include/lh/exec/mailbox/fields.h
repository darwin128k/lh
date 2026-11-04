/**
 * @file fields.h
 * @brief Member fields of ::lh_exec_mailbox_t.
 */

#ifndef LH_EXEC_MAILBOX_FIELDS_H
#define LH_EXEC_MAILBOX_FIELDS_H

/**
 * @def lh_exec_mailbox_fields(capacity_type, count_type, queue_type)
 * @brief The capacity, the places taken and the finished tasks.
 *
 * @param capacity_type ::lh_uint_t: `capacity`, how many tasks may be accepted
 *                      for this owner and not yet dispatched.
 * @param count_type    ::lh_atomic_uint_t: `used`, how many of those places are
 *                      taken; submitters on any thread take one, dispatch gives
 *                      it back.
 * @param queue_type    ::lh_exec_queue_t: `finished`, the tasks whose work is
 *                      done, waiting for the owner; executors push, the owner
 *                      pops.
 */
#define lh_exec_mailbox_fields(capacity_type, count_type, queue_type)                              \
    capacity_type capacity;                                                                        \
    count_type used;                                                                               \
    queue_type finished

#endif /* LH_EXEC_MAILBOX_FIELDS_H */
