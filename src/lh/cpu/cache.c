#include <lh/cpu/cache.h>

#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/compiler/arch/family.h>
#include <lh/compiler/type.h>
#include <lh/cpu/vendor.h>
#include <lh/numeric/fixed/types.h>

#if LH_COMPILER_ARCH_FAMILY_IS_X86 &&                                                              \
    (LH_COMPILER_TYPE_IS_GCC_LIKE || (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC))
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
#        include <cpuid.h>
#    else
#        include <intrin.h>
#    endif

#    define LH_CPU_CACHE_HAVE_CPUID 1

/* Intel: deterministic cache parameters, one sub-leaf per cache until the type
 * field reads 0 ("no more caches"). */
#    define LH_CPU_CACHE_CPUID_LEAF_DETERMINISTIC 4U
#    define LH_CPU_CACHE_LEAF4_TYPE_MASK 0x1FU    /* EAX[4:0], 0 = no more caches */
#    define LH_CPU_CACHE_LEAF4_LEVEL_SHIFT 5      /* EAX[7:5] */
#    define LH_CPU_CACHE_LEAF4_LEVEL_MASK 0x7U
#    define LH_CPU_CACHE_LEAF4_WAYS_SHIFT 22      /* EBX[31:22], minus 1 */
#    define LH_CPU_CACHE_LEAF4_PARTITIONS_SHIFT 12 /* EBX[21:12], minus 1 */
#    define LH_CPU_CACHE_LEAF4_PARTITIONS_MASK 0x3FFU
#    define LH_CPU_CACHE_LEAF4_LINE_MASK 0xFFFU /* EBX[11:0], minus 1; ECX = sets - 1 */
/* Real CPUs describe 4-5 caches; the bound only guards against a hypervisor
 * that never reports the terminating null entry. */
#    define LH_CPU_CACHE_LEAF4_MAX_SUBLEAVES 16U

/* AMD: L3 size in EDX[31:18], in 512KB units. */
#    define LH_CPU_CACHE_CPUID_LEAF_AMD_L2_L3 0x80000006U
#    define LH_CPU_CACHE_AMD_L3_SIZE_SHIFT 18
#    define LH_CPU_CACHE_AMD_L3_UNIT (lh_cast_static(lh_usize_t, 512U) * 1024U)

#    define LH_CPU_CACHE_LEVEL_L3 3U

#    define LH_CPU_CACHE_EAX 0
#    define LH_CPU_CACHE_EBX 1
#    define LH_CPU_CACHE_ECX 2
#    define LH_CPU_CACHE_EDX 3

/* CPUID (leaf, sub-leaf) into regs[EAX..EDX], false when the leaf is past the
 * highest one its range (basic or extended) supports. */
static lh_bool_t
lh_cpu_cache_cpuid(lh_u32_t leaf, lh_u32_t subleaf, lh_u32_t regs[4])
{
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
    unsigned eax, ebx, ecx, edx;

    /* Checks the range's max leaf itself (leaf & 0x80000000). */
    if (!__get_cpuid_count(leaf, subleaf, &eax, &ebx, &ecx, &edx))
    {
        return lh_bool_false;
    }
    regs[LH_CPU_CACHE_EAX] = lh_cast_static(lh_u32_t, eax);
    regs[LH_CPU_CACHE_EBX] = lh_cast_static(lh_u32_t, ebx);
    regs[LH_CPU_CACHE_ECX] = lh_cast_static(lh_u32_t, ecx);
    regs[LH_CPU_CACHE_EDX] = lh_cast_static(lh_u32_t, edx);
#    else
    int info[4];

    __cpuid(info, lh_cast_static(int, leaf & 0x80000000U));
    if (lh_cast_static(lh_u32_t, info[LH_CPU_CACHE_EAX]) < leaf)
    {
        return lh_bool_false;
    }
    __cpuidex(info, lh_cast_static(int, leaf), lh_cast_static(int, subleaf));
    regs[LH_CPU_CACHE_EAX] = lh_cast_static(lh_u32_t, info[LH_CPU_CACHE_EAX]);
    regs[LH_CPU_CACHE_EBX] = lh_cast_static(lh_u32_t, info[LH_CPU_CACHE_EBX]);
    regs[LH_CPU_CACHE_ECX] = lh_cast_static(lh_u32_t, info[LH_CPU_CACHE_ECX]);
    regs[LH_CPU_CACHE_EDX] = lh_cast_static(lh_u32_t, info[LH_CPU_CACHE_EDX]);
#    endif
    return lh_bool_true;
}

static lh_usize_t
lh_cpu_cache_l3_size_intel(void)
{
    lh_u32_t regs[4];
    lh_u32_t subleaf;

    for (subleaf = 0U; subleaf < LH_CPU_CACHE_LEAF4_MAX_SUBLEAVES; ++subleaf)
    {
        lh_u32_t level;

        if (!lh_cpu_cache_cpuid(LH_CPU_CACHE_CPUID_LEAF_DETERMINISTIC, subleaf, regs) ||
            (regs[LH_CPU_CACHE_EAX] & LH_CPU_CACHE_LEAF4_TYPE_MASK) == 0U)
        {
            break;
        }

        level = (regs[LH_CPU_CACHE_EAX] >> LH_CPU_CACHE_LEAF4_LEVEL_SHIFT) &
                LH_CPU_CACHE_LEAF4_LEVEL_MASK;
        if (level == LH_CPU_CACHE_LEVEL_L3)
        {
            const lh_usize_t ways =
                lh_cast_static(lh_usize_t, regs[LH_CPU_CACHE_EBX] >> LH_CPU_CACHE_LEAF4_WAYS_SHIFT) + 1U;
            const lh_usize_t partitions =
                lh_cast_static(lh_usize_t, (regs[LH_CPU_CACHE_EBX] >> LH_CPU_CACHE_LEAF4_PARTITIONS_SHIFT) &
                                               LH_CPU_CACHE_LEAF4_PARTITIONS_MASK) +
                1U;
            const lh_usize_t line =
                lh_cast_static(lh_usize_t, regs[LH_CPU_CACHE_EBX] & LH_CPU_CACHE_LEAF4_LINE_MASK) + 1U;
            const lh_usize_t sets = lh_cast_static(lh_usize_t, regs[LH_CPU_CACHE_ECX]) + 1U;

            return ways * partitions * line * sets;
        }
    }
    return 0U;
}

static lh_usize_t
lh_cpu_cache_l3_size_amd(void)
{
    lh_u32_t regs[4];

    if (!lh_cpu_cache_cpuid(LH_CPU_CACHE_CPUID_LEAF_AMD_L2_L3, 0U, regs))
    {
        return 0U;
    }
    return lh_cast_static(lh_usize_t, regs[LH_CPU_CACHE_EDX] >> LH_CPU_CACHE_AMD_L3_SIZE_SHIFT) *
           LH_CPU_CACHE_AMD_L3_UNIT;
}

#else
#    define LH_CPU_CACHE_HAVE_CPUID 0
#endif

lh_usize_t
lh_cpu_cache_get_l3_size(void)
{
#if LH_CPU_CACHE_HAVE_CPUID
    /* AMD does not implement leaf 4; other x86 vendors that do (Intel, and the
     * VIA/Zhaoxin parts that follow its layout) get it, the rest report 0. */
    return lh_cpu_vendor_is_amd() ? lh_cpu_cache_l3_size_amd() : lh_cpu_cache_l3_size_intel();
#else
    return 0U;
#endif
}
