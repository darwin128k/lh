#include <lh/cpu/id.h>

#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/compiler/arch/family.h>
#include <lh/compiler/type.h>

#if LH_COMPILER_ARCH_FAMILY_IS_X86 &&                                                              \
    (LH_COMPILER_TYPE_IS_GCC_LIKE || (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC))
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
#        include <cpuid.h>
#    else
#        include <intrin.h>
#    endif
#    define LH_CPU_ID_AVAILABLE 1
#else
#    define LH_CPU_ID_AVAILABLE 0
#endif

lh_bool_t
lh_cpu_id(lh_u32_t leaf, lh_u32_t subleaf, lh_u32_t regs[LH_CPU_ID_REGISTER_COUNT])
{
    lh_assert_runtime_ref(regs);
#if LH_CPU_ID_AVAILABLE
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
    unsigned eax;
    unsigned ebx;
    unsigned ecx;
    unsigned edx;

    /* Checks the range's max leaf itself (leaf & 0x80000000). */
    if (!__get_cpuid_count(leaf, subleaf, &eax, &ebx, &ecx, &edx))
    {
        return lh_bool_false;
    }
    regs[LH_CPU_ID_EAX] = lh_cast_static(lh_u32_t, eax);
    regs[LH_CPU_ID_EBX] = lh_cast_static(lh_u32_t, ebx);
    regs[LH_CPU_ID_ECX] = lh_cast_static(lh_u32_t, ecx);
    regs[LH_CPU_ID_EDX] = lh_cast_static(lh_u32_t, edx);
#    else
    int info[LH_CPU_ID_REGISTER_COUNT];

    __cpuid(info, lh_cast_static(int, leaf & 0x80000000U));
    if (lh_cast_static(lh_u32_t, info[LH_CPU_ID_EAX]) < leaf)
    {
        return lh_bool_false;
    }
    __cpuidex(info, lh_cast_static(int, leaf), lh_cast_static(int, subleaf));
    regs[LH_CPU_ID_EAX] = lh_cast_static(lh_u32_t, info[LH_CPU_ID_EAX]);
    regs[LH_CPU_ID_EBX] = lh_cast_static(lh_u32_t, info[LH_CPU_ID_EBX]);
    regs[LH_CPU_ID_ECX] = lh_cast_static(lh_u32_t, info[LH_CPU_ID_ECX]);
    regs[LH_CPU_ID_EDX] = lh_cast_static(lh_u32_t, info[LH_CPU_ID_EDX]);
#    endif
    return lh_bool_true;
#else
    (void)leaf;
    (void)subleaf;
    return lh_bool_false;
#endif
}
