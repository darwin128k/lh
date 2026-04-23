/**
 * @file force_inline.h
 * @brief Library alias: strongly ask the compiler to inline a function.
 *
 * **Inlining** means the compiler replaces a function call
 * with the function body at each call site (like pasting the code),
 * avoiding call/return overhead and enabling further optimizations.
 *
 * Compilers already inline small functions when they think it pays off;
 * **force inline** tells them to do it even when their usual “cost/benefit”
 * heuristic would skip it.
 *
 * Use it sparingly for very small, hot helpers (e.g. accessors, one-liners in tight loops).
 * Overuse can **grow** the binary if the same large body is duplicated at many sites.
 * It is not a substitute for a good algorithm.
 *
 * ::LH_ATTRIBUTE_FORCE_INLINE is the `lh` naming layer;
 * it expands to the same tokens as ::LH_COMPILER_ATTRIBUTE_FORCE_INLINE.
 *
 * See the latter for the exact spelling per compiler
 * (GCC/Clang `always_inline`, MSVC `__forceinline`, etc.).
 */

#ifndef LH_ATTRIBUTE_FORCE_INLINE_H
#define LH_ATTRIBUTE_FORCE_INLINE_H

#include <lh/compiler/attribute/force_inline.h>

/**
 * @def LH_ATTRIBUTE_FORCE_INLINE
 * @brief Force-inline the function it decorates (compiler-specific expansion).
 *
 * Place it on the same line as the function return type (before the name),
 * like a normal `inline` qualifier.
 *
 * @see LH_COMPILER_ATTRIBUTE_FORCE_INLINE
 */
#define LH_ATTRIBUTE_FORCE_INLINE LH_COMPILER_ATTRIBUTE_FORCE_INLINE

#endif // LH_ATTRIBUTE_FORCE_INLINE_H
