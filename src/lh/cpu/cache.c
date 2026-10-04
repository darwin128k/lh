#include <lh/cpu/cache.h>

#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/cpu/id.h>
#include <lh/cpu/vendor.h>
#include <lh/numeric/fixed/types.h>

/* Intel: deterministic cache parameters, one sub-leaf per cache until the type
 * field reads 0 ("no more caches"). */
#define LH_CPU_CACHE_CPUID_LEAF_DETERMINISTIC 4U
#define LH_CPU_CACHE_LEAF4_TYPE_MASK 0x1FU     /* EAX[4:0], 0 = no more caches */
#define LH_CPU_CACHE_LEAF4_LEVEL_SHIFT 5       /* EAX[7:5] */
#define LH_CPU_CACHE_LEAF4_LEVEL_MASK 0x7U
#define LH_CPU_CACHE_LEAF4_WAYS_SHIFT 22       /* EBX[31:22], minus 1 */
#define LH_CPU_CACHE_LEAF4_PARTITIONS_SHIFT 12 /* EBX[21:12], minus 1 */
#define LH_CPU_CACHE_LEAF4_PARTITIONS_MASK 0x3FFU
#define LH_CPU_CACHE_LEAF4_LINE_MASK 0xFFFU /* EBX[11:0], minus 1; ECX = sets - 1 */
/* Real CPUs describe 4-5 caches; the bound only guards against a hypervisor
 * that never reports the terminating null entry. */
#define LH_CPU_CACHE_LEAF4_MAX_SUBLEAVES 16U

/* AMD: L3 size in EDX[31:18], in 512KB units. */
#define LH_CPU_CACHE_CPUID_LEAF_AMD_L2_L3 0x80000006U
#define LH_CPU_CACHE_AMD_L3_SIZE_SHIFT 18
#define LH_CPU_CACHE_AMD_L3_UNIT (lh_cast_static(lh_usize_t, 512U) * 1024U)

#define LH_CPU_CACHE_LEVEL_L3 3U

static lh_usize_t
lh_cpu_cache_l3_size_intel(void)
{
    lh_u32_t regs[4];
    lh_u32_t subleaf;

    for (subleaf = 0U; subleaf < LH_CPU_CACHE_LEAF4_MAX_SUBLEAVES; ++subleaf)
    {
        lh_u32_t level;

        if (!lh_cpu_id(LH_CPU_CACHE_CPUID_LEAF_DETERMINISTIC, subleaf, regs) ||
            (regs[LH_CPU_ID_EAX] & LH_CPU_CACHE_LEAF4_TYPE_MASK) == 0U)
        {
            break;
        }

        level = (regs[LH_CPU_ID_EAX] >> LH_CPU_CACHE_LEAF4_LEVEL_SHIFT) &
                LH_CPU_CACHE_LEAF4_LEVEL_MASK;
        if (level == LH_CPU_CACHE_LEVEL_L3)
        {
            const lh_u32_t ebx = regs[LH_CPU_ID_EBX];
            const lh_usize_t ways =
                lh_cast_static(lh_usize_t, ebx >> LH_CPU_CACHE_LEAF4_WAYS_SHIFT) + 1U;
            const lh_usize_t partitions =
                lh_cast_static(lh_usize_t, (ebx >> LH_CPU_CACHE_LEAF4_PARTITIONS_SHIFT) &
                                               LH_CPU_CACHE_LEAF4_PARTITIONS_MASK) +
                1U;
            const lh_usize_t line =
                lh_cast_static(lh_usize_t, ebx & LH_CPU_CACHE_LEAF4_LINE_MASK) + 1U;
            const lh_usize_t sets = lh_cast_static(lh_usize_t, regs[LH_CPU_ID_ECX]) + 1U;

            return ways * partitions * line * sets;
        }
    }
    return 0U;
}

static lh_usize_t
lh_cpu_cache_l3_size_amd(void)
{
    lh_u32_t regs[4];

    if (!lh_cpu_id(LH_CPU_CACHE_CPUID_LEAF_AMD_L2_L3, 0U, regs))
    {
        return 0U;
    }
    return lh_cast_static(lh_usize_t, regs[LH_CPU_ID_EDX] >> LH_CPU_CACHE_AMD_L3_SIZE_SHIFT) *
           LH_CPU_CACHE_AMD_L3_UNIT;
}

lh_usize_t
lh_cpu_cache_get_l3_size(void)
{
    /* AMD does not implement leaf 4; other x86 vendors that do (Intel, and the
     * VIA/Zhaoxin parts that follow its layout) get it, the rest report 0.
     * No CPUID at all (another architecture) reports 0 the same way. */
    return lh_cpu_vendor_is_amd() ? lh_cpu_cache_l3_size_amd() : lh_cpu_cache_l3_size_intel();
}
