/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_exec_executor_t.
 */

#ifndef LH_EXEC_EXECUTOR_INITIALIZER_H
#define LH_EXEC_EXECUTOR_INITIALIZER_H

#include <lh/exec/executor/accept/cb.h>

/**
 * @def lh_exec_executor_initializer(accept_fn, context)
 * @brief Produces a brace-enclosed initializer for ::lh_exec_executor_t.
 *
 * @param accept_fn ::lh_exec_executor_accept_fn (must not be ::lh_null).
 * @param context   The state the call receives (may be ::lh_null).
 */
#define lh_exec_executor_initializer(accept_fn, context)                                           \
    lh_initializer(lh_cast_static(lh_exec_executor_accept_cb, accept_fn), context)

#endif /* LH_EXEC_EXECUTOR_INITIALIZER_H */
