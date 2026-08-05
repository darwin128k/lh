/**
 * @file cb.h
 * @brief Pointer type for the runtime terminate callback.
 *
 * Defines ::lh_runtime_terminate_cb as a pointer to ::lh_runtime_terminate_fn,
 * used as the callback type in ::lh_runtime_terminate_set.
 *
 * @see lh_runtime_terminate_fn
 * @see lh_runtime_terminate_set
 */

#ifndef LH_RUNTIME_TERMINATE_CB_H
#define LH_RUNTIME_TERMINATE_CB_H

#include <lh/runtime/terminate/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_runtime_terminate_cb
 * @brief Pointer-to-callback type for the runtime terminate handler.
 *
 * Expands to lh_ptr_of(::lh_runtime_terminate_fn).
 * Use when storing or passing the handler to ::lh_runtime_terminate_set.
 */
#define lh_runtime_terminate_cb lh_ptr_of(lh_runtime_terminate_fn)

#endif /* LH_RUNTIME_TERMINATE_CB_H */