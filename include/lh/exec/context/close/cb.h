/**
 * @file cb.h
 * @brief Pointer type for what an executor thread closes when it stops.
 */

#ifndef LH_EXEC_CONTEXT_CLOSE_CB_H
#define LH_EXEC_CONTEXT_CLOSE_CB_H

#include <lh/core/util/ptr.h>
#include <lh/exec/context/close/fn.h>

/**
 * @def lh_exec_context_close_cb
 * @brief Pointer to ::lh_exec_context_close_fn.
 */
#define lh_exec_context_close_cb lh_ptr_of(lh_exec_context_close_fn)

#endif /* LH_EXEC_CONTEXT_CLOSE_CB_H */
