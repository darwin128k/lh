/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_exec_task_t.
 */

#ifndef LH_EXEC_TASK_INITIALIZER_H
#define LH_EXEC_TASK_INITIALIZER_H

#include <lh/core/atomic/ptr/initializer.h>
#include <lh/exec/task/done/cb.h>
#include <lh/exec/task/work/cb.h>

/**
 * @def lh_exec_task_initializer(work_fn, complete_fn, destroy_fn, user)
 * @brief Produces a brace-enclosed initializer for ::lh_exec_task_t, with an
 *        empty queue link, no mailbox, not cancelled.
 *
 * @param work_fn     ::lh_exec_task_work_fn (must not be ::lh_null).
 * @param complete_fn ::lh_exec_task_done_fn, or ::lh_null.
 * @param destroy_fn  ::lh_exec_task_done_fn, or ::lh_null.
 * @param user        The pointer every call receives.
 */
#define lh_exec_task_initializer(work_fn, complete_fn, destroy_fn, user)                           \
    lh_initializer(lh_cast_static(lh_exec_task_work_cb, work_fn),                                  \
                   lh_cast_static(lh_exec_task_done_cb, complete_fn),                              \
                   lh_cast_static(lh_exec_task_done_cb, destroy_fn), user,                         \
                   lh_atomic_ptr_initializer(lh_null), lh_null, lh_bool_false)

#endif /* LH_EXEC_TASK_INITIALIZER_H */
