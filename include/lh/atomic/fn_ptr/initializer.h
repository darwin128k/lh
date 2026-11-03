/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_atomic_fn_ptr_t.
 */

#ifndef LH_ATOMIC_FN_PTR_INITIALIZER_H
#define LH_ATOMIC_FN_PTR_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_atomic_fn_ptr_initializer(fn)
 * @brief Produces a brace-enclosed initializer for ::lh_atomic_fn_ptr_t
 *        holding @p fn.
 *
 * For a static object: it holds @p fn before any thread runs, so no
 * publication is needed. Example:
 *
 * @code{.c}
 * lh_atomic_fn_ptr_t on_fatal = lh_atomic_fn_ptr_initializer(abort);
 * @endcode
 *
 * @param fn Initial value: a `void (void)` function, a pointer to one, or ::lh_null.
 */
#define lh_atomic_fn_ptr_initializer(fn) lh_initializer(fn)

#endif /* LH_ATOMIC_FN_PTR_INITIALIZER_H */
