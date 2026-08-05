/**
 * @file builtin.h
 * @brief Compiler-portable builtin attribute macro.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_BUILTIN — a single macro
 * that instructs the compiler to always inline a function,
 * bypassing its cost-benefit heuristic.
 */

#ifndef LH_COMPILER_ATTRIBUTE_BUILTIN_H
#define LH_COMPILER_ATTRIBUTE_BUILTIN_H

#include <lh/compiler/type.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_BUILTIN
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
 * LH_COMPILER_ATTRIBUTE_BUILTIN int add(int a, int b) { return a + b; }
 * @endcode
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_BUILTIN __attribute__((always_inline)) inline

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_BUILTIN __forceinline
#else
#    error "Compiler does not support force inlining attribute"
#endif

#endif /* LH_COMPILER_ATTRIBUTE_BUILTIN_H */
