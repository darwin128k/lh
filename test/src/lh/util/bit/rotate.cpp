#include <gtest/gtest.h>

#include <lh/util/bit/rotate.h>

namespace
{

TEST(util_bit_rotate, left_u32_by_one)
{
    EXPECT_EQ(lh_bit_rotate_left_u32(0x80000001u, 1U), 0x00000003u);
}

TEST(util_bit_rotate, right_u32_inverts_left)
{
    const lh_u32_t x = 0x12345678u;
    EXPECT_EQ(lh_bit_rotate_right_u32(lh_bit_rotate_left_u32(x, 7U), 7U), x);
}

TEST(util_bit_rotate, u32_by_zero_and_width_are_noop)
{
    const lh_u32_t x = 0xA5A5A5A5u;
    EXPECT_EQ(lh_bit_rotate_left_u32(x, 0U), x);
    EXPECT_EQ(lh_bit_rotate_left_u32(x, 32U), x);
    EXPECT_EQ(lh_bit_rotate_right_u32(x, 0U), x);
    EXPECT_EQ(lh_bit_rotate_right_u32(x, 32U), x);
}

TEST(util_bit_rotate, left_u64_by_one)
{
    EXPECT_EQ(lh_bit_rotate_left_u64(0x8000000000000001ULL, 1U), 0x0000000000000003ULL);
}

TEST(util_bit_rotate, right_u64_inverts_left)
{
    const lh_u64_t x = 0x0123456789ABCDEFULL;
    EXPECT_EQ(lh_bit_rotate_right_u64(lh_bit_rotate_left_u64(x, 13U), 13U), x);
}

} // namespace
