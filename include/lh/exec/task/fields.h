/**
 * @file fields.h
 * @brief Member fields of ::lh_exec_task_t.
 */

#ifndef LH_EXEC_TASK_FIELDS_H
#define LH_EXEC_TASK_FIELDS_H

/**
 * @def lh_exec_task_fields(work_fn_type, done_fn_type, user_type, link_type, mailbox_type,
 *                          flag_type)
 * @brief The task's calls, its pointer, its queue link, its owner's mailbox and
 *        whether it was cancelled.
 *
 * @param work_fn_type ::lh_exec_task_work_fn; the member is `work_cb`.
 * @param done_fn_type ::lh_exec_task_done_fn; the members are `complete_cb`
 *                     and `destroy_cb`.
 * @param user_type    ::lh_ptr, handed to every call.
 * @param link_type    ::lh_atomic_ptr_t: the next task in the queue that holds
 *                     this one, written and read only by `lh/exec`.
 * @param mailbox_type `struct lh_exec_mailbox`: `mailbox` points at the mailbox
 *                     that delivers `complete`; the executor sets it at submit.
 * @param flag_type    ::lh_bool_t: `cancelled`, set by an executor that stopped
 *                     before the work ran; the mailbox then runs `destroy` only.
 */
#define lh_exec_task_fields(work_fn_type, done_fn_type, user_type, link_type, mailbox_type,        \
                            flag_type)                                                             \
    work_fn_type *work_cb;                                                                         \
    done_fn_type *complete_cb;                                                                     \
    done_fn_type *destroy_cb;                                                                      \
    user_type user;                                                                                \
    link_type next;                                                                                \
    mailbox_type *mailbox;                                                                         \
    flag_type cancelled

#endif /* LH_EXEC_TASK_FIELDS_H */
