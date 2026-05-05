/**
 * @file static.h
 * @brief Compile-time check: fail the build if a condition cannot be true.
 *
 * A **compile-time assertion** (also called a static assert) asks the compiler
 * to verify an expression **while translating** the program, before any code
 * runs.
 * If the expression is false, compilation stops with an error and your
 * message is usually shown in the diagnostic — so broken assumptions are
 * caught immediately instead of failing mysteriously at runtime.
 *
 * This differs from a normal runtime `assert(...)`, which checks the condition
 * only when that line executes (and may be disabled in release builds).
 *
 * Use ::lh_assert_static when the value must follow from constants known at
 * compile time — for example validating a configuration macro, an array bound,
 * or a type/size relationship that must never be violated.
 *
 * Under the hood the expansion uses C11 `_Static_assert`, C++ `static_assert`,
 * or the MSVC equivalent, depending on the language and compiler.
 */

#ifndef LH_ASSERT_STATIC_H
#define LH_ASSERT_STATIC_H

#include <lh/compiler/cxx.h>
#include <lh/compiler/type.h>
#include <lh/compiler/extern/c.h>
#include <lh/compiler/attribute/builtin.h>

/**
 * @def lh_assert_static(expr, msg)
 * @brief Emit a compile-time error if `expr` is zero (false).
 *
 * @param expr Integer constant expression; must be non-zero for a successful build.
 * @param msg  String literal shown in the compiler error (C `_Static_assert` /
 *             C++ `static_assert` message).
 *
 * Typical uses: guard `sizeof`, enum values, or library options (e.g. “stack
 * depth must be positive”) so misconfiguration cannot produce a silently
 * broken binary.
 */
#ifdef LH_COMPILER_CXX
#    define lh_assert_static(expr, msg) static_assert(expr, msg)
#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    define lh_assert_static(expr, msg) static_assert(expr, msg)
#else
#    define lh_assert_static(expr, msg) _Static_assert(expr, msg)
#endif

#endif // LH_ASSERT_STATIC_H
