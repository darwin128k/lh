#include <gtest/gtest.h>

#include <lh/cpu/vendor.h>

namespace
{

TEST(cpu_vendor, is_intel_returns_a_valid_bool)
{
    const lh_bool_t result = lh_cpu_vendor_is_intel();
    EXPECT_TRUE(result == lh_bool_true || result == lh_bool_false);
}

TEST(cpu_vendor, is_amd_returns_a_valid_bool)
{
    const lh_bool_t result = lh_cpu_vendor_is_amd();
    EXPECT_TRUE(result == lh_bool_true || result == lh_bool_false);
}

TEST(cpu_vendor, never_both_intel_and_amd)
{
    // No real CPU identifies as both vendors — this would only trip if one of
    // the two detectors misdecoded the CPUID vendor string.
    EXPECT_FALSE(lh_cpu_vendor_is_intel() && lh_cpu_vendor_is_amd());
}

} // namespace
