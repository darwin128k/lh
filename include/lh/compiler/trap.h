/**
 * @file trap.h
 * @brief Macro that stops the process right here, with no function call.
 *
 * `lh_compiler_trap()` compiles to a single instruction that the CPU
 * refuses to execute (`ud2` on x86, `brk` on arm64), so the process dies at
 * the exact spot — a debugger or crash dump shows the faulting line. Unlike
 * calling an error handler, it adds no call to the surrounding function,
 * which therefore stays small enough for the compiler to inline and merge.
 *
 * Unsupported compilers fail at compile time via the `#else` branch below.
 */

#ifndef LH_COMPILER_TRAP_H
#define LH_COMPILER_TRAP_H

#include <lh/compiler/type.h>

#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
/**
 * @def lh_compiler_trap
 * @brief Abort via GCC/Clang's trap builtin (`ud2` / `brk`).
 */
#    define lh_compiler_trap() __builtin_trap()

#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    include <intrin.h>
/**
 * @def lh_compiler_trap
 * @brief Abort via MSVC's fast-fail intrinsic
 *        (`FAST_FAIL_FATAL_APP_EXIT`, x86 / x64 / arm64).
 */
#    define lh_compiler_trap() __fastfail(7)

#else
#    error "Compiler does not support a trap intrinsic"
#endif

#endif /* LH_COMPILER_TRAP_H */
