/**
 * @file vendor.h
 * @brief Runtime detection of the x86 CPU vendor (Intel vs AMD).
 *
 * Unlike ::lh_cpu_simd_has_sse2 and friends (lh/cpu/simd.h), this is not about
 * whether a feature is *usable* — it is a straight identity question, read once from
 * CPUID leaf 0's vendor string ("GenuineIntel" / "AuthenticAMD", packed into
 * EBX:EDX:ECX in that order). Not currently used anywhere in this library — some
 * microarchitectural trade-offs (e.g. REP MOVSB's throughput relative to a hand-
 * written SIMD loop) turned out to vary by vendor, but by the time this was added,
 * this project's own SIMD tiers had already closed that particular gap on their own.
 * Kept as a general-purpose primitive for whoever needs it next.
 */

#ifndef LH_CPU_VENDOR_H
#define LH_CPU_VENDOR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether this CPU identifies itself as a genuine Intel part.
 *
 * @return ::lh_bool_true if CPUID leaf 0's vendor string is "GenuineIntel",
 *         ::lh_bool_false otherwise (including on non-x86 targets, or where CPUID
 *         itself could not be read).
 */
lh_bool_t
lh_cpu_vendor_is_intel(void);

/**
 * @brief Whether this CPU identifies itself as an AMD part.
 *
 * @return ::lh_bool_true if CPUID leaf 0's vendor string is "AuthenticAMD",
 *         ::lh_bool_false otherwise (including on non-x86 targets, or where CPUID
 *         itself could not be read).
 */
lh_bool_t
lh_cpu_vendor_is_amd(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CPU_VENDOR_H */
