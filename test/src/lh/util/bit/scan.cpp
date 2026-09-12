#include <gtest/gtest.h>

#include <lh/util/bit/scan.h>

namespace
{

TEST(util_bit_scan, forward_lowest_bit)
{
    EXPECT_EQ(lh_bit_scan_forward(0x00000001u), 0U);
    EXPECT_EQ(lh_bit_scan_forward(0x00000100u), 8U);
    EXPECT_EQ(lh_bit_scan_forward(0x80000000u), 31U);
}

TEST(util_bit_scan, forward_ignores_higher_bits)
{
    // 0x180 has bits 7 and 8 set — the lowest one (7) must win.
    EXPECT_EQ(lh_bit_scan_forward(0x00000180u), 7U);
}

TEST(util_bit_scan, reverse_highest_bit)
{
    EXPECT_EQ(lh_bit_scan_reverse(0x00000001u), 0U);
    EXPECT_EQ(lh_bit_scan_reverse(0x00000180u), 8U);
    EXPECT_EQ(lh_bit_scan_reverse(0x80000000u), 31U);
}

TEST(util_bit_scan, reverse_ignores_lower_bits)
{
    // 0x180 has bits 7 and 8 set — the highest one (8) must win.
    EXPECT_EQ(lh_bit_scan_reverse(0x00000180u), 8U);
}

TEST(util_bit_scan, forward_and_reverse_agree_on_a_single_bit)
{
    for (lh_u32_t i = 0U; i < 32U; ++i)
    {
        const lh_u32_t x = 1U << i;
        EXPECT_EQ(lh_bit_scan_forward(x), i);
        EXPECT_EQ(lh_bit_scan_reverse(x), i);
    }
}

} // namespace
