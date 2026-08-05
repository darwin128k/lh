/**
 * @file fn.h
 * @brief Callable signature for the library’s “fatal stop” hook.
 *
 * When error handling cannot continue (for example nothing catches a throw),
 * `lh` invokes the **runtime terminate** handler.
 * This type describes that handler: a function taking no arguments,
 * returning nothing (`void`),
 *
 * and expected **not** to return to the caller (same idea as `std::terminate`
 * in C++ — typically `abort`, logging then `_Exit`, or an embedded reset).
 *
 * Store a pointer to such a function as ::lh_runtime_terminate_cb;
 * install it with ::lh_runtime_terminate_set and run it via ::lh_runtime_terminate.
 */

#ifndef LH_RUNTIME_TERMINATE_FN_H
#define LH_RUNTIME_TERMINATE_FN_H

/**
 * @typedef lh_runtime_terminate_fn
 * @brief Function type `void (void)` — the terminate handler signature.
 *
 * Not a pointer type by itself;
 * use ::lh_runtime_terminate_cb for a pointer.
 *
 * @see lh_runtime_terminate_cb
 * @see lh_runtime_terminate_set
 * @see lh_runtime_terminate
 */
typedef void(lh_runtime_terminate_fn)(void);

#endif /* LH_RUNTIME_TERMINATE_FN_H */