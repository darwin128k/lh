/**
 * @file cb.h
 * @brief Pointer type for how an executor takes a task.
 */

#ifndef LH_EXEC_EXECUTOR_ACCEPT_CB_H
#define LH_EXEC_EXECUTOR_ACCEPT_CB_H

#include <lh/exec/executor/accept/fn.h>

/**
 * @def lh_exec_executor_accept_cb
 * @brief Pointer to ::lh_exec_executor_accept_fn.
 */
#define lh_exec_executor_accept_cb lh_ptr_of(lh_exec_executor_accept_fn)

#endif /* LH_EXEC_EXECUTOR_ACCEPT_CB_H */
