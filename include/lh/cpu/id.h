/**
 * @file id.h
 * @brief Read one x86 CPUID leaf.
 *
 * Vendor, cache and SIMD detection all start here. The four registers come
 * back in EAX, EBX, ECX, EDX order (::LH_CPU_ID_EAX through
 * ::LH_CPU_ID_EDX). A leaf past the highest one its range supports — the
 * basic range, or the extended range at `0x80000000` — is not read. Where
 * CPUID does not exist (another architecture, or a compiler without it) the
 * read fails.
 */

#ifndef LH_CPU_ID_H
#define LH_CPU_ID_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

/**
 * @def LH_CPU_ID_EAX
 * @brief Index of EAX in the register array from ::lh_cpu_id.
 */
#define LH_CPU_ID_EAX 0

/**
 * @def LH_CPU_ID_EBX
 * @brief Index of EBX in the register array from ::lh_cpu_id.
 */
#define LH_CPU_ID_EBX 1

/**
 * @def LH_CPU_ID_ECX
 * @brief Index of ECX in the register array from ::lh_cpu_id.
 */
#define LH_CPU_ID_ECX 2

/**
 * @def LH_CPU_ID_EDX
 * @brief Index of EDX in the register array from ::lh_cpu_id.
 */
#define LH_CPU_ID_EDX 3

/**
 * @def LH_CPU_ID_REGISTER_COUNT
 * @brief How many registers ::lh_cpu_id writes: EAX, EBX, ECX, EDX.
 */
#define LH_CPU_ID_REGISTER_COUNT 4

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief CPUID leaf @p leaf, sub-leaf @p subleaf, into @p regs.
 *
 * @param regs Four registers, indexed by ::LH_CPU_ID_EAX and the three
 *        that follow. Written only when the read succeeds.
 * @return ::lh_bool_false when CPUID cannot be read or @p leaf is past the
 *         highest leaf of its range.
 */
lh_bool_t
lh_cpu_id(lh_u32_t leaf, lh_u32_t subleaf, lh_u32_t regs[LH_CPU_ID_REGISTER_COUNT]);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CPU_ID_H */
