/**
 * @file of.h
 * @brief Pointer type construction: ::lh_ptr_of and ::lh_cptr_of.
 *
 * Split out of `lh/util/ptr.h` so `lh/byte/ptr.h` can build its pointer aliases
 * without including `lh/util/ptr.h`, which itself includes `lh/byte/ptr.h`.
 * Depends on nothing.
 */

#ifndef LH_UTIL_PTR_OF_H
#define LH_UTIL_PTR_OF_H

/**
 * @def lh_ptr_of(T)
 * @brief Pointer type to @p T.
 *
 * Expands to `T *`.
 * Used to construct pointer type expressions where a typedef is not applicable.
 *
 * @param T Base type.
 *
 * Example usage:
 * @code{.c}
 * lh_ptr_of(int)      // → int *
 * lh_ptr_of(lh_void)  // → void *
 * @endcode
 */
#define lh_ptr_of(T) T *

/**
 * @def lh_cptr_of(T)
 * @brief Pointer type to `const` @p T.
 *
 * Expands to `const T *` — the same expansion as `lh_ptr_of(const T)`.
 *
 * @param T Base type (without `const` or `*`).
 *
 * Example usage:
 * @code{.c}
 * lh_cptr_of(int)           // → const int *
 * lh_cptr_of(lh_schar_t)    // → const lh_schar_t *
 * @endcode
 */
#define lh_cptr_of(T) lh_ptr_of(const T)

#endif /* LH_UTIL_PTR_OF_H */
