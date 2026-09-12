#include <lh/cpu/vendor.h>

#include <lh/cast/static.h>
#include <lh/compiler/arch/family.h>
#include <lh/compiler/type.h>
#include <lh/numeric/fixed/types.h>

#if LH_COMPILER_ARCH_FAMILY_IS_X86
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
#        include <cpuid.h>
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif

/* CPUID leaf 0's vendor string is 12 ASCII bytes packed into three registers in
 * this exact order: EBX, EDX, ECX (not EBX/ECX/EDX). Each constant below is those
 * four characters packed little-endian into a u32, matching how the register
 * itself holds them (e.g. "Genu" -> 'G' in the lowest byte). */
#    define LH_CPU_VENDOR_CPUID_LEAF_VENDOR 0

#    define LH_CPU_VENDOR_INTEL_EBX ((lh_u32_t)0x756e6547) /* "Genu" */
#    define LH_CPU_VENDOR_INTEL_EDX ((lh_u32_t)0x49656e69) /* "ineI" */
#    define LH_CPU_VENDOR_INTEL_ECX ((lh_u32_t)0x6c65746e) /* "ntel" */

#    define LH_CPU_VENDOR_AMD_EBX ((lh_u32_t)0x68747541) /* "Auth" */
#    define LH_CPU_VENDOR_AMD_EDX ((lh_u32_t)0x69746e65) /* "enti" */
#    define LH_CPU_VENDOR_AMD_ECX ((lh_u32_t)0x444d4163) /* "cAMD" */

static lh_bool_t
lh_cpu_vendor_matches(lh_u32_t want_ebx, lh_u32_t want_edx, lh_u32_t want_ecx)
{
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
    unsigned eax, ebx, ecx, edx;

    if (!__get_cpuid(LH_CPU_VENDOR_CPUID_LEAF_VENDOR, &eax, &ebx, &ecx, &edx))
    {
        return lh_bool_false; /* CPUID itself unavailable (pre-Pentium 32-bit x86) */
    }

    return lh_cast_static(lh_bool_t, (lh_cast_static(lh_u32_t, ebx) == want_ebx) &&
                                          (lh_cast_static(lh_u32_t, edx) == want_edx) &&
                                          (lh_cast_static(lh_u32_t, ecx) == want_ecx));
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    int info[4];
    __cpuid(info, LH_CPU_VENDOR_CPUID_LEAF_VENDOR);

    return lh_cast_static(lh_bool_t, (lh_cast_static(lh_u32_t, info[1]) == want_ebx) &&
                                          (lh_cast_static(lh_u32_t, info[3]) == want_edx) &&
                                          (lh_cast_static(lh_u32_t, info[2]) == want_ecx));
#    else
    return lh_bool_false;
#    endif
}

#endif /* LH_COMPILER_ARCH_FAMILY_IS_X86 */

lh_bool_t
lh_cpu_vendor_is_intel(void)
{
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    return lh_cpu_vendor_matches(LH_CPU_VENDOR_INTEL_EBX, LH_CPU_VENDOR_INTEL_EDX,
                                  LH_CPU_VENDOR_INTEL_ECX);
#else
    return lh_bool_false;
#endif
}

lh_bool_t
lh_cpu_vendor_is_amd(void)
{
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    return lh_cpu_vendor_matches(LH_CPU_VENDOR_AMD_EBX, LH_CPU_VENDOR_AMD_EDX, LH_CPU_VENDOR_AMD_ECX);
#else
    return lh_bool_false;
#endif
}
