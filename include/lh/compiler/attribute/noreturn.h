/**
 * @file noreturn.h
 * @brief Compiler-portable noreturn attribute macro.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_NORETURN — a single macro
 * that marks a function as never returning to its caller
 * (e.g. abort, exit, longjmp wrappers).
 */

#ifndef LH_COMPILER_ATTRIBUTE_NORETURN_H
#define LH_COMPILER_ATTRIBUTE_NORETURN_H

#include <lh/compiler/type.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_NORETURN
 * @brief Mark a function as never returning to its caller.
 *
 * Informs the compiler that the decorated function does not return
 * (e.g. it calls `abort()`, `exit()`, or throws unconditionally).
 *
 * Enables dead-code elimination after call sites and suppresses
 * spurious "missing return" warnings.
 * Expands to the appropriate compiler-specific annotation:
 *
 * | Compiler    | Expansion                    |
 * |-------------|------------------------------|
 * | GCC / Clang | `__attribute__((noreturn))`  |
 * | MSVC        | `__declspec(noreturn)`       |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_NORETURN void fatal(const char *msg);
 * @endcode
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_NORETURN __attribute__((noreturn))

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_NORETURN __declspec(noreturn)

#else
#    error "Compiler does not support noreturn attribute"
#endif

#endif /* LH_COMPILER_ATTRIBUTE_NORETURN_H */
