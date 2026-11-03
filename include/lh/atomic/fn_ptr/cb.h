/**
 * @file cb.h
 * @brief Pointer type stored by ::lh_atomic_fn_ptr_t.
 */

#ifndef LH_ATOMIC_FN_PTR_CB_H
#define LH_ATOMIC_FN_PTR_CB_H

#include <lh/atomic/fn_ptr/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_atomic_fn_ptr_cb
 * @brief Pointer to ::lh_atomic_fn_ptr_fn, the value an ::lh_atomic_fn_ptr_t holds.
 *
 * Expands to lh_ptr_of(::lh_atomic_fn_ptr_fn).
 */
#define lh_atomic_fn_ptr_cb lh_ptr_of(lh_atomic_fn_ptr_fn)

#endif /* LH_ATOMIC_FN_PTR_CB_H */
