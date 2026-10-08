/**
 * @file null.h
 * @brief Null pointer constant and null-check predicates.
 */

#ifndef LH_NULL_H
#define LH_NULL_H

#include <lh/compiler/cxx.h>
#include <lh/ptr.h>

/**
 * @def lh_null
 * @brief Null pointer constant: ::LH_PTR_T_MIN in C, `nullptr` in C++.
 *
 * In C this is the canonical null ::lh_ptr — a typed zero from `lh/ptr.h`, and
 * what ::lh_optional_ref and every pointer comparison in the library use.
 *
 * In C++ it is the language's own `nullptr` (::LH_COMPILER_CXX), and the
 * difference is not cosmetic. A zero *typed as `void *`* is not a null pointer
 * constant there, and C++ has no implicit conversion from `void *` to any
 * pointer type — `int *p = <void *>` is an error, not a conversion
 * (`invalid conversion from 'void*' to 'int *'`, measured on this toolchain,
 * every `-std` from c++11 to c++23). So the C spelling reaches a function
 * pointer, a typed pointer or a handle only through a cast at every one of
 * those places, and a test that wants to write `lh_null` there has to write
 * `reinterpret_cast<T *>(lh_null)` instead. The literal the language provides
 * has no such restriction, and the same spelling now works in both languages.
 *
 * Example usage:
 * @code{.c}
 * if (lh_null_eq(p)) { ... }
 * @endcode
 */
#ifdef LH_COMPILER_CXX
#    define lh_null nullptr
#else
#    define lh_null LH_PTR_T_MIN
#endif /* LH_COMPILER_CXX */

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

#endif /* LH_NULL_H */
