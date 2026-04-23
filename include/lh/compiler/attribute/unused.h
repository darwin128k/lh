/**
 * @file unused.h
 * @brief Compiler-portable “mark as used” macro for silencing unused warnings.
 *
 * Provides ::LH_COMPILER_ATTRIBUTE_UNUSED — expands to `((void)(expr))` on all supported
 * compilers. Intended **only inside function bodies** (expression statement), not as a
 * declaration attribute on parameters or types.
 */

#ifndef LH_COMPILER_ATTRIBUTE_UNUSED_H
#define LH_COMPILER_ATTRIBUTE_UNUSED_H

/**
 * @def LH_COMPILER_ATTRIBUTE_UNUSED(expr)
 * @brief Silence “unused” diagnostics by evaluating @p expr as a void expression.
 *
 * Expands to `((void)(expr))`. Place at statement level, typically first line of the
 * function, for intentionally unused parameters or locals. Works the same on GCC, Clang,
 * and MSVC.
 *
 * Does **not** cover unused file-scope functions or variables; for those use compiler
 * attributes directly (e.g. `__attribute__((unused))`), remove the dead code, or reference
 * the symbol in a way your toolchain accepts.
 *
 * Example usage:
 * @code{.c}
 * void foo(int x)
 * {
 *     LH_COMPILER_ATTRIBUTE_UNUSED(x);
 * }
 * @endcode
 */
#define LH_COMPILER_ATTRIBUTE_UNUSED(expr) ((void)(expr))

#endif // LH_COMPILER_ATTRIBUTE_UNUSED_H
