/**
 * @file unused.h
 * @brief Public macro for silencing unused value / parameter warnings.
 *
 * Provides ::LH_ATTRIBUTE_UNUSED — alias for ::LH_COMPILER_ATTRIBUTE_UNUSED.
 * Use **inside a function body** as a statement; see ::LH_COMPILER_ATTRIBUTE_UNUSED.
 */

#ifndef LH_ATTRIBUTE_UNUSED_H
#define LH_ATTRIBUTE_UNUSED_H

#include <lh/compiler/attribute/unused.h>

/**
 * @def LH_ATTRIBUTE_UNUSED(expr)
 * @brief Same as ::LH_COMPILER_ATTRIBUTE_UNUSED(@p expr).
 *
 * Example usage:
 * @code{.c}
 * void foo(int x)
 * {
 *     LH_ATTRIBUTE_UNUSED(x);
 * }
 * @endcode
 */
#define LH_ATTRIBUTE_UNUSED(expr) LH_COMPILER_ATTRIBUTE_UNUSED(expr)

#endif // LH_ATTRIBUTE_UNUSED_H