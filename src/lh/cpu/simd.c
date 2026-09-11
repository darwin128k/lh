#include <lh/cpu/simd.h>

#include <lh/compiler/arch.h>
#include <lh/compiler/arch/family.h>
#include <lh/compiler/type.h>
#include <lh/config.h>

#if (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC) &&                                                   \
    (LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2)
#    include <intrin.h>
#endif

lh_bool_t
lh_cpu_has_sse2(void)
{
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
#    if LH_COMPILER_ARCH_FAMILY_IS_X86 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
    /* SSE2 is part of the mandatory baseline ISA on x86-64 — no runtime check needed. */
    return lh_bool_true;
#    elif LH_COMPILER_TYPE_IS_GCC_LIKE
    return (lh_bool_t)__builtin_cpu_supports("sse2");
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    int info[4];
    __cpuid(info, 1);
    return (lh_bool_t)((info[3] >> 26) & 1); /* CPUID.1:EDX.SSE2 */
#    else
    return lh_bool_false;
#    endif
#else
    return lh_bool_false;
#endif
}

lh_bool_t
lh_cpu_has_avx2(void)
{
#if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
    /* Checks CPUID *and* that the OS has enabled AVX register state via XGETBV/XCR0,
     * not just the raw feature bit. */
    return (lh_bool_t)__builtin_cpu_supports("avx2");
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    /* Same check as __builtin_cpu_supports above, hand-rolled: MSVC has no
     * equivalent builtin. */
    int info[4];

    __cpuid(info, 0);
    if (info[0] < 7)
    {
        return lh_bool_false; /* CPUID leaf 7 (structured extended features) not available */
    }

    __cpuid(info, 1);
    if (!((info[2] >> 27) & 1) || !((info[2] >> 28) & 1))
    {
        return lh_bool_false; /* no OSXSAVE, or no AVX */
    }

    if ((_xgetbv(0) & 0x6) != 0x6)
    {
        return lh_bool_false; /* OS hasn't enabled XMM+YMM state (XCR0 bits 1-2) */
    }

    __cpuidex(info, 7, 0);
    return (lh_bool_t)((info[1] >> 5) & 1); /* CPUID.(EAX=7,ECX=0):EBX.AVX2 */
#    else
    return lh_bool_false;
#    endif
#else
    return lh_bool_false;
#endif
}
