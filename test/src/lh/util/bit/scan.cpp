#include <gtest/gtest.h>

#include <lh/util/bit/scan.h>

namespace
{

TEST(util_bit_scan, forward_u32_lowest_bit)
{
    EXPECT_EQ(lh_bit_scan_forward_u32(0x00000001u), 0U);
    EXPECT_EQ(lh_bit_scan_forward_u32(0x00000100u), 8U);
    EXPECT_EQ(lh_bit_scan_forward_u32(0x80000000u), 31U);
}

TEST(util_bit_scan, forward_u32_ignores_higher_bits)
{
    // 0x180 has bits 7 and 8 set — the lowest one (7) must win.
    EXPECT_EQ(lh_bit_scan_forward_u32(0x00000180u), 7U);
}

TEST(util_bit_scan, reverse_u32_highest_bit)
{
    EXPECT_EQ(lh_bit_scan_reverse_u32(0x00000001u), 0U);
    EXPECT_EQ(lh_bit_scan_reverse_u32(0x00000180u), 8U);
    EXPECT_EQ(lh_bit_scan_reverse_u32(0x80000000u), 31U);
}

TEST(util_bit_scan, reverse_u32_ignores_lower_bits)
{
    // 0x180 has bits 7 and 8 set — the highest one (8) must win.
    EXPECT_EQ(lh_bit_scan_reverse_u32(0x00000180u), 8U);
}

TEST(util_bit_scan, forward_and_reverse_u32_agree_on_a_single_bit)
{
    for (lh_u32_t i = 0U; i < 32U; ++i)
    {
        const lh_u32_t x = 1U << i;
        EXPECT_EQ(lh_bit_scan_forward_u32(x), i);
        EXPECT_EQ(lh_bit_scan_reverse_u32(x), i);
    }
}

TEST(util_bit_scan, forward_and_reverse_u8_agree_on_a_single_bit)
{
    for (lh_u32_t i = 0U; i < 8U; ++i)
    {
        const lh_u8_t x = static_cast<lh_u8_t>(1U << i);
        EXPECT_EQ(lh_bit_scan_forward_u8(x), i);
        EXPECT_EQ(lh_bit_scan_reverse_u8(x), i);
    }
}

TEST(util_bit_scan, u8_ignores_bits_outside_its_own_width)
{
    // 0x81 has bits 0 and 7 set; forward must find bit 0, reverse bit 7 — neither
    // should be confused by the u32 widening this delegates to internally.
    EXPECT_EQ(lh_bit_scan_forward_u8(0x81u), 0U);
    EXPECT_EQ(lh_bit_scan_reverse_u8(0x81u), 7U);
}

TEST(util_bit_scan, forward_and_reverse_u16_agree_on_a_single_bit)
{
    for (lh_u32_t i = 0U; i < 16U; ++i)
    {
        const lh_u16_t x = static_cast<lh_u16_t>(1U << i);
        EXPECT_EQ(lh_bit_scan_forward_u16(x), i);
        EXPECT_EQ(lh_bit_scan_reverse_u16(x), i);
    }
}

TEST(util_bit_scan, u16_ignores_bits_outside_its_own_width)
{
    // 0x8001 has bits 0 and 15 set.
    EXPECT_EQ(lh_bit_scan_forward_u16(0x8001u), 0U);
    EXPECT_EQ(lh_bit_scan_reverse_u16(0x8001u), 15U);
}

TEST(util_bit_scan, forward_and_reverse_u64_agree_on_a_single_bit)
{
    for (lh_u32_t i = 0U; i < 64U; ++i)
    {
        const lh_u64_t x = lh_u64_t(1) << i;
        EXPECT_EQ(lh_bit_scan_forward_u64(x), i);
        EXPECT_EQ(lh_bit_scan_reverse_u64(x), i);
    }
}

TEST(util_bit_scan, u64_forward_crosses_the_32_bit_half_boundary)
{
    // Only a bit in the high half is set — forward must not stop at 32 zero
    // bits in the low half and must report an index >= 32.
    EXPECT_EQ(lh_bit_scan_forward_u64(lh_u64_t(0x100000000ULL)), 32U);
}

TEST(util_bit_scan, u64_reverse_crosses_the_32_bit_half_boundary)
{
    // Only a bit in the low half is set — reverse must not assume the high half
    // is where the answer lives just because the value is 64 bits wide.
    EXPECT_EQ(lh_bit_scan_reverse_u64(lh_u64_t(0x00000001ULL)), 0U);
    EXPECT_EQ(lh_bit_scan_reverse_u64(lh_u64_t(0x100000001ULL)), 32U);
}

} // namespace
