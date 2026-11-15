/**
 * @file noinline.h
 * @brief Compiler-portable noinline attribute.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_NOINLINE. Put it on a function that must
 * stay a call. Inlining a long walk into every caller makes that caller
 * larger than the instruction cache and the frame slower.
 */

#ifndef LH_COMPILER_ATTRIBUTE_NOINLINE_H
#define LH_COMPILER_ATTRIBUTE_NOINLINE_H

#include <lh/core/compiler/type.h>

/**
 * @def LH_COMPILER_ATTRIBUTE_NOINLINE
 * @brief The function stays a call.
 *
 * | Compiler    | Expansion                    |
 * |-------------|------------------------------|
 * | GCC / Clang | `__attribute__((noinline))`  |
 * | MSVC        | `__declspec(noinline)`       |
 */
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
#    define LH_COMPILER_ATTRIBUTE_NOINLINE __attribute__((noinline))

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define LH_COMPILER_ATTRIBUTE_NOINLINE __declspec(noinline)

#else
#    error "Compiler does not support a noinline attribute"
#endif

#endif /* LH_COMPILER_ATTRIBUTE_NOINLINE_H */
