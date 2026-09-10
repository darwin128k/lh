/**
 * @file force_inline.h
 * @brief Compiler-portable force-inline attribute macro.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_FORCE_INLINE — a single macro that marks
 * a function definition as always inlined at every call site. Unlike plain
 * `inline` (a hint the compiler is free to ignore), this is a mandate —
 * use it only for small, hot functions where a real call has been measured
 * to cost more than the code-size tradeoff is worth.
 *
 * Already includes the storage/inline keywords appropriate for a
 * header-defined function — write it in place of `static inline`.
 */

#ifndef LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H
#define LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H

#include <lh/compiler/type.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_FORCE_INLINE
 * @brief Mandate inlining at every call site; use like `static inline`.
 *
 * Expands to the appropriate compiler-specific spelling:
 *
 * | Compiler    | Expansion                                    |
 * |-------------|-----------------------------------------------|
 * | GCC / Clang | `static inline __attribute__((always_inline))` |
 * | MSVC        | `static __forceinline`                         |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_FORCE_INLINE
 * int square(int x) { return x * x; }
 * @endcode
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_FORCE_INLINE static inline __attribute__((always_inline))

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_FORCE_INLINE static __forceinline

#else
#    error "Compiler does not support a force-inline attribute"
#endif

#endif /* LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H */
