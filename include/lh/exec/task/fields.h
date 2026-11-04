/**
 * @file fields.h
 * @brief Member fields of ::lh_exec_task_t.
 */

#ifndef LH_EXEC_TASK_FIELDS_H
#define LH_EXEC_TASK_FIELDS_H

/**
 * @def lh_exec_task_fields(work_fn_type, done_fn_type, user_type, link_type)
 * @brief The task's calls, its pointer and its queue link.
 *
 * @param work_fn_type ::lh_exec_task_work_fn; the member is `work_cb`.
 * @param done_fn_type ::lh_exec_task_done_fn; the members are `complete_cb`
 *                     and `destroy_cb`.
 * @param user_type    ::lh_ptr, handed to every call.
 * @param link_type    ::lh_atomic_ptr_t: the next task in the queue that holds
 *                     this one, written and read only by `lh/exec`.
 */
#define lh_exec_task_fields(work_fn_type, done_fn_type, user_type, link_type)                      \
    work_fn_type *work_cb;                                                                         \
    done_fn_type *complete_cb;                                                                     \
    done_fn_type *destroy_cb;                                                                      \
    user_type user;                                                                                \
    link_type next

#endif /* LH_EXEC_TASK_FIELDS_H */
