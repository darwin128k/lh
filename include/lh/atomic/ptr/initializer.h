/**
 * @file initializer.h
 * @brief Brace-enclosed initializer for ::lh_atomic_ptr_t.
 */

#ifndef LH_ATOMIC_PTR_INITIALIZER_H
#define LH_ATOMIC_PTR_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_atomic_ptr_initializer(pointer)
 * @brief Produces a brace-enclosed initializer for ::lh_atomic_ptr_t holding
 *        @p pointer.
 *
 * For a static object: it holds @p pointer before any thread runs, so no
 * publication is needed. Example:
 *
 * @code{.c}
 * lh_atomic_ptr_t current = lh_atomic_ptr_initializer(lh_null);
 * @endcode
 *
 * @param pointer Initial value, an ::lh_ptr (or ::lh_null).
 */
#define lh_atomic_ptr_initializer(pointer) lh_initializer(pointer)

#endif /* LH_ATOMIC_PTR_INITIALIZER_H */
