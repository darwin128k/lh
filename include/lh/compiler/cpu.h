/**
 * @file cpu.h
 * @brief Macro wrapping the compiler's own "does this CPU support feature X" builtin.
 *
 * GCC/Clang provide `__builtin_cpu_supports(feature)` directly: it checks CPUID for
 * the named x86 feature string (and, for AVX-family features, that the OS has also
 * enabled the corresponding extended register state via XGETBV/XCR0 — not just the
 * raw CPUID bit). MSVC has no equivalent builtin at all; callers needing the same
 * check under MSVC implement it directly via `__cpuid`/`__cpuidex` + `_xgetbv`
 * instead (see `lh/cpu/simd.c` for both), so this macro is only ever used — and only
 * ever needs to be defined — under GCC/Clang.
 */

#ifndef LH_COMPILER_CPU_H
#define LH_COMPILER_CPU_H

#include <lh/compiler/type.h>

#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
/**
 * @def lh_compiler_cpu_has_feature(feature)
 * @brief Whether the running CPU has @p feature.
 *
 * @param feature A GCC/Clang feature-name string literal (e.g. `"sse2"`, `"avx2"`).
 *
 * Expands to `__builtin_cpu_supports(feature)`. GCC/Clang-only — there is no MSVC
 * equivalent; MSVC callers must check the same feature directly via CPUID/XGETBV
 * (see `lh_cpu_simd_has_sse2`/`lh_cpu_simd_has_avx2` in `lh/cpu/simd.c`).
 *
 * Example usage:
 * @code{.c}
 * if (lh_compiler_cpu_has_feature("avx2")) { ... }
 * @endcode
 */
#    define lh_compiler_cpu_has_feature(feature) __builtin_cpu_supports(feature)
#endif /* LH_COMPILER_TYPE_IS_GCC_LIKE */

#endif /* LH_COMPILER_CPU_H */
