/**
 * @file ptr.h
 * @brief Helpers for fixed C array objects: size, begin, and one-past-end pointer.
 *
 * Macros use `sizeof` on **array objects** (not pointers).
 * The name *ptr* follows the low-level pointer helper naming used by util/str/ptr
 * and util/wstr/ptr.
 */

#ifndef LH_UTIL_ARRAY_PTR_H
#define LH_UTIL_ARRAY_PTR_H

/**
 * @def lh_array_ptr_get_size(x)
 * @brief Number of elements in the array object @p x.
 *
 * Expands to `(sizeof(x) / sizeof(x[0]))`.
 * Valid only when @p x is an **array** (or `struct` containing an array member used as `s.a`);
 * if @p x is a pointer (including after array decay in a function parameter),
 * `sizeof(x)` is the pointer size and the result is wrong.
 *
 * @param x Array object or subobject of array type
 *          (not a pointer to the first element alone).
 *
 * Example usage:
 * @code{.c}
 * int buf[10];
 * lh_array_ptr_get_size(buf)   // → 10
 * @endcode
 */
#define lh_array_ptr_get_size(x) (sizeof(x) / sizeof(x[0]))

/**
 * @def lh_array_ptr_get_begin(x)
 * @brief The array object @p x at index 0 (unchanged expression).
 *
 * Expands to `(x)`.
 * In value context the array decays to a pointer to its first element,
 * same as writing `buf` instead of `&buf[0]`.
 *
 * Same preconditions as ::lh_array_ptr_get_size:
 * @p x must be an array object, not a bare pointer.
 *
 * @param x Array object or subobject of array type.
 *
 * Example usage:
 * @code{.c}
 * int buf[10];
 * for (int *p = lh_array_ptr_get_begin(buf); p != lh_array_ptr_get_end(buf); ++p) { }
 * @endcode
 */
#define lh_array_ptr_get_begin(x) (x)

/**
 * @def lh_array_ptr_get_end(x)
 * @brief Pointer one past the last element of the array object @p x.
 *
 * Expands to `((x) + lh_array_ptr_get_size(x))`.
 * The result has the same type as @p x after array decay (pointer to element type).
 * Same preconditions as ::lh_array_ptr_get_size: @p x must be an array object, not a bare pointer.
 *
 * @param x Array object or subobject of array type.
 *
 * Example usage:
 * @code{.c}
 * int buf[10];
 * for (int *p = lh_array_ptr_get_begin(buf); p != lh_array_ptr_get_end(buf); ++p) { }
 * @endcode
 */
#define lh_array_ptr_get_end(x) ((x) + lh_array_ptr_get_size(x))

#endif /* LH_UTIL_ARRAY_PTR_H */
