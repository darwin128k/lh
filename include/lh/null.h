/**
 * @file null.h
 * @brief Null pointer constant and null-check predicates.
 */

#ifndef LH_NULL_H
#define LH_NULL_H

#include <lh/ptr.h>

/**
 * @def lh_null
 * @brief Null pointer constant typed as ::lh_ptr.
 *
 * Alias for ::LH_PTR_T_MIN from `lh/ptr.h` — the canonical null ::lh_ptr
 * for use with ::lh_optional_ref and pointer comparisons throughout the library.
 *
 * Example usage:
 * @code{.c}
 * lh_version_unpack(self, &major, lh_null, lh_null);
 * if (lh_null_eq(p)) { ... }
 * @endcode
 */
#define lh_null LH_PTR_T_MIN

/**
 * @def lh_null_eq(ptr)
 * @brief Check whether @p ptr is a null pointer.
 *
 * Expands to a boolean-valued integer expression:
 * 1 if @p ptr equals ::lh_null, 0 otherwise.
 *
 * @param ptr Pointer expression.
 *
 * Example usage:
 * @code{.c}
 * if (lh_null_eq(p)) { ... }
 * @endcode
 */
#define lh_null_eq(ptr) lh_math_eq(ptr, lh_null)

/**
 * @def lh_null_ne(ptr)
 * @brief Check whether @p ptr is not a null pointer.
 *
 * Logical complement of ::lh_null_eq.
 *
 * @param ptr Pointer expression.
 *
 * Example usage:
 * @code{.c}
 * if (lh_null_ne(p)) { ... }
 * @endcode
 */
#define lh_null_ne(ptr) lh_math_ne(ptr, lh_null)

#endif // LH_NULL_H
