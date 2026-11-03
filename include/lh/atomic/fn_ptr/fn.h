/**
 * @file fn.h
 * @brief Function type stored by ::lh_atomic_fn_ptr_t.
 *
 * C converts a pointer to any function type into a pointer to any other and
 * back without loss, but not into a data pointer; so the atomic function
 * pointer keeps one function type, `void (void)`, and a caller with another
 * signature casts on the way in and back to its own type before the call.
 */

#ifndef LH_ATOMIC_FN_PTR_FN_H
#define LH_ATOMIC_FN_PTR_FN_H

#include <lh/void.h>

/**
 * @typedef lh_atomic_fn_ptr_fn
 * @brief Function type `void (void)` — what ::lh_atomic_fn_ptr_t points to.
 *
 * Not a pointer type by itself; use ::lh_atomic_fn_ptr_cb for a pointer.
 */
typedef lh_void(lh_atomic_fn_ptr_fn)(void);

#endif /* LH_ATOMIC_FN_PTR_FN_H */
