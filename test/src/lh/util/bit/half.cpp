#include <gtest/gtest.h>

#include <lh/util/bit/half.h>

namespace
{

TEST(util_bit_half, make_u64_joins_high_and_low)
{
    EXPECT_EQ(lh_bit_make_u64(0x01234567u, 0x89ABCDEFu), 0x0123456789ABCDEFULL);
    EXPECT_EQ(lh_bit_make_u64(0u, 0xFFFFFFFFu), 0x00000000FFFFFFFFULL);
    EXPECT_EQ(lh_bit_make_u64(0xFFFFFFFFu, 0u), 0xFFFFFFFF00000000ULL);
}

TEST(util_bit_half, get_high_and_low_split)
{
    EXPECT_EQ(lh_bit_get_high_u32(0x0123456789ABCDEFULL), 0x01234567u);
    EXPECT_EQ(lh_bit_get_low_u32(0x0123456789ABCDEFULL), 0x89ABCDEFu);
}

TEST(util_bit_half, split_then_make_round_trips)
{
    const lh_u64_t x = 0xFEDCBA9876543210ULL;
    EXPECT_EQ(lh_bit_make_u64(lh_bit_get_high_u32(x), lh_bit_get_low_u32(x)), x);
}

} // namespace
