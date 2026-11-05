/**
 * @file fields.h
 * @brief Member fields of ::lh_rail_thread_t.
 */

#ifndef LH_RAIL_THREAD_FIELDS_H
#define LH_RAIL_THREAD_FIELDS_H

/**
 * @def lh_rail_thread_fields(executor_type, queue_type, context_type, kind_type, flag_type,
 *                            handle_type)
 * @brief The executor record, the tasks waiting, the thread's state and the base
 *        objects it runs on.
 *
 * @param executor_type ::lh_rail_executor_t: `executor`, the record submitters
 *                      use; its context is this thread executor.
 * @param queue_type    ::lh_rail_queue_t: `pending`, the accepted tasks not yet
 *                      started; pushed under `lock`, popped by the thread.
 * @param context_type  ::lh_rail_context_t: `context`, a copy of what the thread
 *                      opens and closes; `local` (an ::lh_ptr) is what it opened.
 * @param kind_type     ::lh_rail_stop_kind_t: `stop`, whether and how the thread
 *                      stops; under `lock`.
 * @param flag_type     ::lh_bool_t: `ended`, set by the thread when it closed its
 *                      context and runs nothing more; under `lock`.
 * @param handle_type   ::lh_ptr, handles of the rail base (lh/rail/base.h):
 *                      `lock` guards `pending` pushes, `stop` and `ended`;
 *                      `wake` is the signal the thread sleeps on for a task or a
 *                      stop; `end` the one a stop sleeps on for `ended`;
 *                      `thread` the one thread. ::lh_null while not made.
 */
#define lh_rail_thread_fields(executor_type, queue_type, context_type, kind_type, flag_type,       \
                              handle_type)                                                         \
    executor_type executor;                                                                        \
    queue_type pending;                                                                            \
    context_type context;                                                                          \
    lh_ptr local;                                                                                  \
    kind_type stop;                                                                                \
    flag_type ended;                                                                               \
    handle_type lock;                                                                              \
    handle_type wake;                                                                              \
    handle_type end;                                                                               \
    handle_type thread

#endif /* LH_RAIL_THREAD_FIELDS_H */
