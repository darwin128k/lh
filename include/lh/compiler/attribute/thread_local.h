/**
 * @file thread_local.h
 * @brief Compiler-portable thread-local storage attribute macro.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_THREAD_LOCAL — a single macro
 * that declares a variable with thread-local storage duration,
 * giving each thread its own independent copy.
 */

#ifndef LH_COMPILER_ATTRIBUTE_THREAD_LOCAL_H
#define LH_COMPILER_ATTRIBUTE_THREAD_LOCAL_H

#include <lh/compiler/type.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_THREAD_LOCAL
 * @brief Declare a variable with thread-local storage duration.
 *
 * Each thread gets its own independent copy of the decorated variable.
 * Expands to the appropriate compiler-specific keyword:
 *
 * | Compiler    | Expansion              |
 * |-------------|------------------------|
 * | GCC / Clang | `__thread`             |
 * | MSVC        | `__declspec(thread)`   |
 *
 * Example usage:
 * @code{.c}
 * LH_COMPILER_ATTRIBUTE_THREAD_LOCAL int error_code;
 * @endcode
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_THREAD_LOCAL __thread

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_THREAD_LOCAL __declspec(thread)

#else
#    error "Compiler does not support thread-local storage attribute"
#endif

#endif /* LH_COMPILER_ATTRIBUTE_THREAD_LOCAL_H */
