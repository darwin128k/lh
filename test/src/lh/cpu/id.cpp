#include <gtest/gtest.h>

#include <lh/compiler/arch/family.h>
#include <lh/cpu/id.h>

namespace
{

TEST(cpu_id, missing_leaf_leaves_the_registers)
{
    lh_u32_t regs[LH_CPU_ID_REGISTER_COUNT];
    for (int i = 0; i < LH_CPU_ID_REGISTER_COUNT; ++i)
    {
        regs[i] = 0xA5A5A5A5U;
    }

    EXPECT_FALSE(lh_cpu_id(0x7FFFFFFFU, 0U, regs));
    for (int i = 0; i < LH_CPU_ID_REGISTER_COUNT; ++i)
    {
        EXPECT_EQ(regs[i], 0xA5A5A5A5U);
    }
}

TEST(cpu_id, leaf_zero_follows_the_target)
{
    lh_u32_t regs[LH_CPU_ID_REGISTER_COUNT] = {};

#if LH_COMPILER_ARCH_FAMILY_IS_X86
    ASSERT_TRUE(lh_cpu_id(0U, 0U, regs));

    lh_u32_t again[LH_CPU_ID_REGISTER_COUNT] = {};
    ASSERT_TRUE(lh_cpu_id(0U, 0U, again));
    EXPECT_EQ(regs[LH_CPU_ID_EAX], again[LH_CPU_ID_EAX]);
    EXPECT_EQ(regs[LH_CPU_ID_EBX], again[LH_CPU_ID_EBX]);
    EXPECT_EQ(regs[LH_CPU_ID_ECX], again[LH_CPU_ID_ECX]);
    EXPECT_EQ(regs[LH_CPU_ID_EDX], again[LH_CPU_ID_EDX]);
    // EBX:EDX:ECX is the 12-byte vendor string. An empty one is a misread.
    EXPECT_NE(regs[LH_CPU_ID_EBX] | regs[LH_CPU_ID_EDX] | regs[LH_CPU_ID_ECX], 0U);
#else
    EXPECT_FALSE(lh_cpu_id(0U, 0U, regs));
#endif
}

} // namespace
