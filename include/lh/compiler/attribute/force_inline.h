/**
 * @file force_inline.h
 * @brief Compiler-portable force-inline attribute macro.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_FORCE_INLINE — a single macro
 * that instructs the compiler to always inline a function,
 * bypassing its cost-benefit heuristic.
 */

#ifndef LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H
#define LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H

#include <lh/compiler/type.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_FORCE_INLINE
 * @brief Force the compiler to inline the decorated function.
 *
 * Overrides the compiler's inlining heuristic
 * and guarantees inlining regardless of optimization level.
 * Expands to the appropriate compiler-specific annotation:
 *
 * | Compiler    | Expansion                               |
 * |-------------|-----------------------------------------|
 * | GCC / Clang | `__attribute__((always_inline)) inline` |
 * | MSVC        | `__forceinline`                         |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_FORCE_INLINE int add(int a, int b) { return a + b; }
 * @endcode
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_FORCE_INLINE __attribute__((always_inline)) inline

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_FORCE_INLINE __forceinline
#else
#    error "Compiler does not support force inlining attribute"
#endif

#endif // LH_COMPILER_ATTRIBUTE_FORCE_INLINE_H
