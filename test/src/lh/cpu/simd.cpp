#include <gtest/gtest.h>

#include <lh/cpu/simd.h>
#include <lh/compiler/arch.h>
#include <lh/compiler/arch/family.h>

namespace
{

TEST(cpu_simd, has_sse2_returns_a_valid_bool)
{
    const lh_bool_t result = lh_cpu_simd_has_sse2();
    EXPECT_TRUE(result == lh_bool_true || result == lh_bool_false);
}

TEST(cpu_simd, has_ssse3_returns_a_valid_bool)
{
    const lh_bool_t result = lh_cpu_simd_has_ssse3();
    EXPECT_TRUE(result == lh_bool_true || result == lh_bool_false);
}

TEST(cpu_simd, has_avx2_returns_a_valid_bool)
{
    const lh_bool_t result = lh_cpu_simd_has_avx2();
    EXPECT_TRUE(result == lh_bool_true || result == lh_bool_false);
}

#if LH_COMPILER_ARCH_FAMILY_IS_X86 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
TEST(cpu_simd, sse2_is_always_true_on_x86_64)
{
    // SSE2 is part of the mandatory baseline ISA on x86-64 — every x86-64 CPU has
    // it, so lh_cpu_simd_has_sse2 must report true unconditionally here, with no real
    // CPUID check involved.
    EXPECT_EQ(lh_cpu_simd_has_sse2(), lh_bool_true);
}
#endif

TEST(cpu_simd, avx2_implies_sse2)
{
    // No real CPU has AVX2 without SSE2 — a true AVX2 result without a true SSE2
    // result would indicate a bug in one of the two detectors, not a real CPU.
    if (lh_cpu_simd_has_avx2())
    {
        EXPECT_EQ(lh_cpu_simd_has_sse2(), lh_bool_true);
    }
}

TEST(cpu_simd, avx2_implies_ssse3)
{
    // No real CPU has AVX2 without SSSE3 either (SSSE3 predates AVX2 by several
    // years in every CPU generation that shipped either).
    if (lh_cpu_simd_has_avx2())
    {
        EXPECT_EQ(lh_cpu_simd_has_ssse3(), lh_bool_true);
    }
}

TEST(cpu_simd, ssse3_implies_sse2)
{
    // No real CPU has SSSE3 without SSE2 — SSSE3 is a strict superset.
    if (lh_cpu_simd_has_ssse3())
    {
        EXPECT_EQ(lh_cpu_simd_has_sse2(), lh_bool_true);
    }
}

} // namespace
