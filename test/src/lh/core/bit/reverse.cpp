#include <gtest/gtest.h>

#include <lh/core/bit/reverse.h>

namespace
{

TEST(util_bit_reverse, mirrors_low_bits_within_width)
{
    EXPECT_EQ(lh_bit_reverse_u32(0x1u, 1U), 0x1u);
    EXPECT_EQ(lh_bit_reverse_u32(0x1u, 5U), 0x10u);
    EXPECT_EQ(lh_bit_reverse_u32(0x6u, 3U), 0x3u);
    EXPECT_EQ(lh_bit_reverse_u32(0x30u, 8U), 0x0Cu);
}

TEST(util_bit_reverse, drops_bits_above_width_and_handles_zero_and_full_width)
{
    EXPECT_EQ(lh_bit_reverse_u32(0xF1u, 4U), 0x8u);
    EXPECT_EQ(lh_bit_reverse_u32(0xFFFFFFFFu, 0U), 0x0u);
    EXPECT_EQ(lh_bit_reverse_u32(0x00000001u, 32U), 0x80000000u);
    EXPECT_EQ(lh_bit_reverse_u32(lh_bit_reverse_u32(0x12345678u, 32U), 32U), 0x12345678u);
}

} // namespace
