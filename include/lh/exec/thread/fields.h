/**
 * @file fields.h
 * @brief Member fields of ::lh_exec_thread_t.
 */

#ifndef LH_EXEC_THREAD_FIELDS_H
#define LH_EXEC_THREAD_FIELDS_H

/**
 * @def lh_exec_thread_fields(executor_type, queue_type, context_type, kind_type, flag_type,
 *                            mutex_type, condition_type, os_thread_type)
 * @brief The executor record, the tasks waiting, the thread's state and the OS
 *        objects it runs on.
 *
 * @param executor_type  ::lh_exec_executor_t: `executor`, the record submitters
 *                       use; its context is this thread executor.
 * @param queue_type     ::lh_exec_queue_t: `pending`, the accepted tasks not yet
 *                       started; pushed under `mutex`, popped by the thread.
 * @param context_type   ::lh_exec_context_t: `context`, a copy of what the thread
 *                       opens and closes; `local` (an ::lh_ptr) is what it opened.
 * @param kind_type      ::lh_exec_stop_kind_t: `stop`, whether and how the thread
 *                       stops; under `mutex`.
 * @param flag_type      ::lh_bool_t: `ended`, set by the thread when it closed its
 *                       context and runs nothing more; under `mutex`.
 * @param mutex_type     ::lh_os_mutex_t: `mutex`, guards `pending` pushes, `stop`
 *                       and `ended`.
 * @param condition_type ::lh_os_condition_t: `wake` (the thread waits for a task
 *                       or a stop) and `end` (a stop waits for `ended`).
 * @param os_thread_type ::lh_os_thread_t: `thread`, the one OS thread.
 */
#define lh_exec_thread_fields(executor_type, queue_type, context_type, kind_type, flag_type,       \
                              mutex_type, condition_type, os_thread_type)                          \
    executor_type executor;                                                                        \
    queue_type pending;                                                                            \
    context_type context;                                                                          \
    lh_ptr local;                                                                                  \
    kind_type stop;                                                                                \
    flag_type ended;                                                                               \
    mutex_type mutex;                                                                              \
    condition_type wake;                                                                           \
    condition_type end;                                                                            \
    os_thread_type thread

#endif /* LH_EXEC_THREAD_FIELDS_H */
