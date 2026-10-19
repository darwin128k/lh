/**
 * @file vendor.h
 * @brief Runtime detection of the x86 CPU vendor (Intel vs AMD).
 *
 * Unlike ::lh_cpu_simd_has_sse2 and friends (lh/cpu/simd.h), this is not about
 * whether a feature is *usable* — it is a straight identity question, read once from
 * CPUID leaf 0's vendor string ("GenuineIntel" / "AuthenticAMD", packed into
 * EBX:EDX:ECX in that order, via ::lh_cpu_id). ::lh_cpu_cache_get_l3_size uses
 * it, since Intel and AMD describe their caches through different CPUID leaves.
 */

#ifndef LH_CPU_VENDOR_H
#define LH_CPU_VENDOR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/** True when the CPUID vendor string (EBX, EDX, ECX of leaf 0) is @p want_ebx, @p want_edx, @p
 * want_ecx. */
lh_bool_t
lh_cpu_vendor_matches(lh_u32_t want_ebx, lh_u32_t want_edx, lh_u32_t want_ecx);

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
