/**
 * @file unreachable.h
 * @brief Macro that tells the compiler a code path can never execute.
 *
 * In real programs some branches are logically impossible
 * (e.g. after a function that never returns,
 * or the default arm of a `switch` you believe is exhaustive).
 *
 * **Unreachable** hints let the optimizer treat that point as
 * having no successor: it may drop dead code, tighten warnings,
 * and assume invariants hold after the hint.
 *
 * This is only a promise to the compiler.
 * If control **does** reach `lh_compiler_unreachable()`,
 * behavior is undefined — do not use it as a runtime error handler.
 * Unsupported compilers fail at compile time via the `#else` branch below.
 */

#ifndef LH_COMPILER_UNREACHABLE_H
#define LH_COMPILER_UNREACHABLE_H

#include <lh/compiler/type.h>

#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
/**
 * @def lh_compiler_unreachable
 * @brief Unreachable code hint for GCC/Clang compilers
 * @details Uses built-in function to mark unreachable code paths,
 *          enabling dead code elimination and optimization.
 */
#    define lh_compiler_unreachable() __builtin_unreachable()

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
/**
 * @def lh_compiler_unreachable
 * @brief Unreachable code directive for MSVC
 * @details Employs assumption intrinsic to optimize code flow
 *          by indicating unreachable execution paths.
 */
#    define lh_compiler_unreachable() __assume(0)

#else
#    pragma error "Compiler does not support unreachable hint functionality"
#endif

#endif // LH_COMPILER_UNREACHABLE_H