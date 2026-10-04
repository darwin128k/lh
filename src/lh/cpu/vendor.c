#include <lh/cpu/vendor.h>

#include <lh/cast/static.h>
#include <lh/cpu/id.h>

/* CPUID leaf 0's vendor string is 12 ASCII bytes packed into three registers in
 * this exact order: EBX, EDX, ECX (not EBX/ECX/EDX). Each constant below is those
 * four characters packed little-endian into a u32, matching how the register
 * itself holds them (e.g. "Genu" -> 'G' in the lowest byte). */
#define LH_CPU_VENDOR_CPUID_LEAF_VENDOR 0

#define LH_CPU_VENDOR_INTEL_EBX (lh_cast_static(lh_u32_t, 0x756e6547)) /* "Genu" */
#define LH_CPU_VENDOR_INTEL_EDX (lh_cast_static(lh_u32_t, 0x49656e69)) /* "ineI" */
#define LH_CPU_VENDOR_INTEL_ECX (lh_cast_static(lh_u32_t, 0x6c65746e)) /* "ntel" */

#define LH_CPU_VENDOR_AMD_EBX (lh_cast_static(lh_u32_t, 0x68747541)) /* "Auth" */
#define LH_CPU_VENDOR_AMD_EDX (lh_cast_static(lh_u32_t, 0x69746e65)) /* "enti" */
#define LH_CPU_VENDOR_AMD_ECX (lh_cast_static(lh_u32_t, 0x444d4163)) /* "cAMD" */

static lh_bool_t
lh_cpu_vendor_matches(lh_u32_t want_ebx, lh_u32_t want_edx, lh_u32_t want_ecx)
{
    lh_u32_t regs[LH_CPU_ID_REGISTER_COUNT];

    if (!lh_cpu_id(LH_CPU_VENDOR_CPUID_LEAF_VENDOR, 0U, regs))
    {
        return lh_bool_false; /* CPUID itself unavailable, or leaf 0 unreadable */
    }
    return lh_cast_static(lh_bool_t, (regs[LH_CPU_ID_EBX] == want_ebx) &&
                                         (regs[LH_CPU_ID_EDX] == want_edx) &&
                                         (regs[LH_CPU_ID_ECX] == want_ecx));
}

lh_bool_t
lh_cpu_vendor_is_intel(void)
{
    return lh_cpu_vendor_matches(LH_CPU_VENDOR_INTEL_EBX, LH_CPU_VENDOR_INTEL_EDX,
                                 LH_CPU_VENDOR_INTEL_ECX);
}

lh_bool_t
lh_cpu_vendor_is_amd(void)
{
    return lh_cpu_vendor_matches(LH_CPU_VENDOR_AMD_EBX, LH_CPU_VENDOR_AMD_EDX,
                                 LH_CPU_VENDOR_AMD_ECX);
}
