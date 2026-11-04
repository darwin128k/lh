/**
 * @file cb.h
 * @brief Pointer type for what an executor thread opens when it starts.
 */

#ifndef LH_EXEC_CONTEXT_OPEN_CB_H
#define LH_EXEC_CONTEXT_OPEN_CB_H

#include <lh/core/util/ptr.h>
#include <lh/exec/context/open/fn.h>

/**
 * @def lh_exec_context_open_cb
 * @brief Pointer to ::lh_exec_context_open_fn.
 */
#define lh_exec_context_open_cb lh_ptr_of(lh_exec_context_open_fn)

#endif /* LH_EXEC_CONTEXT_OPEN_CB_H */
