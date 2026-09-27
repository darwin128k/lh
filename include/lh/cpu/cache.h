/**
 * @file cache.h
 * @brief Runtime detection of the x86 CPU's cache sizes.
 * Read from CPUID: Intel's deterministic cache parameters (leaf 4), AMD's
 * L2/L3 descriptor (extended leaf 0x80000006). Used by lh_memory_std_copy /
 * lh_memory_std_set to decide when a copy or fill is large enough to bypass the
 * cache with non-temporal stores — a fixed byte count cannot fit both a CPU
 * whose L3 is 4MB and one whose L3 is 32MB.
 */

#ifndef LH_CPU_CACHE_H
#define LH_CPU_CACHE_H

#include <lh/compiler/extern/c.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Size of this CPU's last-level (L3) cache, in bytes.
 * The whole L3 as CPUID reports it, not a per-core share: Intel's leaf 4 L3
 * descriptor, or AMD's leaf 0x80000006 L3 size field (the latter is only
 * specified to a 512KB granularity). Performs CPUID on every call — callers
 * that need it on a hot path should cache the result.
 * @return The L3 size in bytes, or 0 when it cannot be determined: no L3, a
 *         vendor using neither descriptor, CPUID unavailable, or a non-x86
 *         target.
 */
lh_usize_t
lh_cpu_cache_get_l3_size(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CPU_CACHE_H */
