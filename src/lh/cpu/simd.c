#include <lh/cpu/simd.h>

#include <lh/compiler/arch.h>
#include <lh/compiler/arch/family.h>
#include <lh/compiler/type.h>
#include <lh/config.h>

#if (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC) &&                                                   \
    (LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2)
#    include <intrin.h>

/* CPUID leaves (the __cpuid/__cpuidex "function_id" argument) used below. */
#    define LH_CPU_SIMD_CPUID_LEAF_MAX_FUNCTION 0     /* EAX: highest supported basic leaf */
#    define LH_CPU_SIMD_CPUID_LEAF_FEATURE_INFO 1     /* processor feature bits */
#    define LH_CPU_SIMD_CPUID_LEAF_EXTENDED_FEATURES 7 /* AVX2 and newer feature bits */

/* __cpuid/__cpuidex's own fixed signature: void __cpuid(int cpuInfo[4], ...) —
 * always exactly one int per register, filled in this order: EAX, EBX, ECX, EDX. */
#    define LH_CPU_SIMD_CPUID_REGISTER_COUNT 4
#    define LH_CPU_SIMD_CPUID_EAX 0
#    define LH_CPU_SIMD_CPUID_EBX 1
#    define LH_CPU_SIMD_CPUID_ECX 2
#    define LH_CPU_SIMD_CPUID_EDX 3

/* Feature bit positions within the registers above. */
#    define LH_CPU_SIMD_CPUID_EDX_SSE2_BIT 26              /* leaf 1, EDX */
#    define LH_CPU_SIMD_CPUID_ECX_OSXSAVE_BIT 27           /* leaf 1, ECX */
#    define LH_CPU_SIMD_CPUID_ECX_AVX_BIT 28               /* leaf 1, ECX */
#    define LH_CPU_SIMD_CPUID_EXTENDED_FEATURES_EBX_AVX2_BIT 5 /* leaf 7 sub-leaf 0, EBX */

/* _xgetbv's control-register index, and the XCR0 bits it reports back. */
#    define LH_CPU_SIMD_XCR0 0
#    define LH_CPU_SIMD_XCR0_SSE_STATE_BIT 1 /* XMM register state enabled by the OS */
#    define LH_CPU_SIMD_XCR0_AVX_STATE_BIT 2 /* YMM register state enabled by the OS */

#endif

lh_bool_t
lh_cpu_simd_has_sse2(void)
{
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
#    if LH_COMPILER_ARCH_FAMILY_IS_X86 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
    /* SSE2 is part of the mandatory baseline ISA on x86-64 — no runtime check needed. */
    return lh_bool_true;
#    elif LH_COMPILER_TYPE_IS_GCC_LIKE
    return (lh_bool_t)__builtin_cpu_supports("sse2");
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    int info[LH_CPU_SIMD_CPUID_REGISTER_COUNT];
    __cpuid(info, LH_CPU_SIMD_CPUID_LEAF_FEATURE_INFO);
    return (lh_bool_t)((info[LH_CPU_SIMD_CPUID_EDX] >> LH_CPU_SIMD_CPUID_EDX_SSE2_BIT) & 1);
#    else
    return lh_bool_false;
#    endif
#else
    return lh_bool_false;
#endif
}

lh_bool_t
lh_cpu_simd_has_avx2(void)
{
#if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
    /* Checks CPUID *and* that the OS has enabled AVX register state via XGETBV/XCR0,
     * not just the raw feature bit. */
    return (lh_bool_t)__builtin_cpu_supports("avx2");
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    /* Same check as __builtin_cpu_supports above, hand-rolled: MSVC has no
     * equivalent builtin. */
    int info[LH_CPU_SIMD_CPUID_REGISTER_COUNT];

    __cpuid(info, LH_CPU_SIMD_CPUID_LEAF_MAX_FUNCTION);
    if (info[LH_CPU_SIMD_CPUID_EAX] < LH_CPU_SIMD_CPUID_LEAF_EXTENDED_FEATURES)
    {
        return lh_bool_false; /* CPUID leaf 7 (structured extended features) not available */
    }

    __cpuid(info, LH_CPU_SIMD_CPUID_LEAF_FEATURE_INFO);
    if (!((info[LH_CPU_SIMD_CPUID_ECX] >> LH_CPU_SIMD_CPUID_ECX_OSXSAVE_BIT) & 1) ||
        !((info[LH_CPU_SIMD_CPUID_ECX] >> LH_CPU_SIMD_CPUID_ECX_AVX_BIT) & 1))
    {
        return lh_bool_false; /* no OSXSAVE, or no AVX */
    }

    if ((_xgetbv(LH_CPU_SIMD_XCR0) &
         ((1U << LH_CPU_SIMD_XCR0_SSE_STATE_BIT) | (1U << LH_CPU_SIMD_XCR0_AVX_STATE_BIT))) !=
        ((1U << LH_CPU_SIMD_XCR0_SSE_STATE_BIT) | (1U << LH_CPU_SIMD_XCR0_AVX_STATE_BIT)))
    {
        return lh_bool_false; /* OS hasn't enabled XMM+YMM state */
    }

    __cpuidex(info, LH_CPU_SIMD_CPUID_LEAF_EXTENDED_FEATURES, 0);
    return (lh_bool_t)((info[LH_CPU_SIMD_CPUID_EBX] >> LH_CPU_SIMD_CPUID_EXTENDED_FEATURES_EBX_AVX2_BIT) &
                        1);
#    else
    return lh_bool_false;
#    endif
#else
    return lh_bool_false;
#endif
}
