/**
 * @file simd.h
 * @brief Runtime detection of x86 SIMD instruction set support.
 *
 * Whether the compiler can even compile a given tier's intrinsics for this target
 * at all is a build-time toolchain fact — decided once, at CMake configure time, by
 * a compile-only probe (cmake/check_simd.cmake) and recorded as
 * `LH_LIBRARY_OPTION_SIMD_HAVE_{SSE2,AVX2}` in `lh/config.h`. Whether the CPU
 * actually running the resulting binary supports that tier is a separate,
 * runtime-only question — the two differ under cross-compilation, or simply
 * because a binary built for a wide baseline ends up running on older hardware.
 * ::lh_cpu_has_sse2 / ::lh_cpu_has_avx2 answer that runtime question; both are safe
 * to call unconditionally and return ::lh_bool_false whenever the corresponding
 * build-time option is off, regardless of the actual CPU.
 */

#ifndef LH_CPU_SIMD_H
#define LH_CPU_SIMD_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether this CPU supports SSE2.
 *
 * SSE2 is part of the mandatory baseline ISA on x86-64 (every x86-64 CPU has it, by
 * architecture definition) — returns ::lh_bool_true there without a real check. On
 * 32-bit x86, where SSE2 is optional, this checks CPUID for real.
 *
 * @return ::lh_bool_true if SSE2 is usable on this CPU, ::lh_bool_false otherwise
 *         (including whenever `LH_LIBRARY_OPTION_SIMD_HAVE_SSE2` is off).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_cpu_has_sse2(void);

/**
 * @brief Whether this CPU supports AVX2.
 *
 * AVX2 is never part of any baseline ISA, so this is always a real check: CPUID for
 * the feature bit *and* confirmation (via XCR0/XGETBV) that the OS has enabled AVX
 * register state, not just that the CPU has the raw feature bit.
 *
 * @return ::lh_bool_true if AVX2 is usable on this CPU, ::lh_bool_false otherwise
 *         (including whenever `LH_LIBRARY_OPTION_SIMD_HAVE_AVX2` is off).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_cpu_has_avx2(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CPU_SIMD_H */
