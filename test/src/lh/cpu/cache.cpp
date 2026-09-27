#include <gtest/gtest.h>

#include <lh/cpu/cache.h>

namespace
{

TEST(cpu_cache, l3_size_is_zero_or_plausible)
{
    // 0 means "unknown". Anything else must look like a real L3: at least
    // 256KB, at most 1GB, and a whole number of 64-byte lines — a misdecoded
    // CPUID field lands far outside that.
    const lh_usize_t size = lh_cpu_cache_get_l3_size();
    if (size != 0U)
    {
        EXPECT_GE(size, static_cast<lh_usize_t>(256U * 1024U));
        EXPECT_LE(size, static_cast<lh_usize_t>(1024U) * 1024U * 1024U);
        EXPECT_EQ(size % 64U, 0U);
    }
}

TEST(cpu_cache, l3_size_is_stable)
{
    EXPECT_EQ(lh_cpu_cache_get_l3_size(), lh_cpu_cache_get_l3_size());
}

} // namespace
