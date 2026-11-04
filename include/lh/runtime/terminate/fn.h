/**
 * @file fn.h
 * @brief Callable signature for the library's "fatal stop" hook.
 *
 * When error handling cannot continue (a failed check, a broken invariant),
 * `lh` invokes the **runtime terminate** handler (::lh_runtime_terminate).
 * This type describes the function of that handler: it takes the handler's
 * context, returns nothing and is expected **not** to return to the caller
 * (same idea as `std::terminate` in C++ — typically `abort`, logging then
 * `_Exit`, or an embedded reset).
 *
 * The function and its context together are an
 * ::lh_runtime_terminate_handler_t; a pointer to the function alone is
 * ::lh_runtime_terminate_cb.
 */

#ifndef LH_RUNTIME_TERMINATE_FN_H
#define LH_RUNTIME_TERMINATE_FN_H

#include <lh/self.h>

/**
 * @typedef lh_runtime_terminate_fn
 * @brief Function type `void (lh_self_ptr self)` — the terminate handler's call.
 *
 * @p self is the handler's context (::lh_runtime_terminate_handler_get_context),
 * which may be ::lh_null. Not a pointer type by itself; use
 * ::lh_runtime_terminate_cb for a pointer.
 *
 * @see lh_runtime_terminate_cb
 * @see lh_runtime_terminate_handler_t
 * @see lh_runtime_terminate
 */
typedef lh_void(lh_runtime_terminate_fn)(lh_self_ptr self);

#endif /* LH_RUNTIME_TERMINATE_FN_H */
