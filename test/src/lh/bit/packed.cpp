#include <gtest/gtest.h>

#include <lh/bit/packed.h>

namespace
{

TEST(bit_packed, max_is_all_ones_of_the_width)
{
    EXPECT_EQ(lh_bit_packed_max(1U), 1U);
    EXPECT_EQ(lh_bit_packed_max(4U), 15U);
    EXPECT_EQ(lh_bit_packed_max(8U), 255U);
}

TEST(bit_packed, bytes_round_up_to_a_whole_byte)
{
    EXPECT_EQ(lh_bit_packed_bytes(14U, 4U), 7U);
    EXPECT_EQ(lh_bit_packed_bytes(9U, 1U), 2U);
    EXPECT_EQ(lh_bit_packed_bytes(3U, 2U), 1U);
    EXPECT_EQ(lh_bit_packed_bytes(0U, 8U), 0U);
}

TEST(bit_packed, get_reads_fields_high_bit_first)
{
    const lh_byte_t row[] = {0xA5, 0x3C};

    EXPECT_EQ(lh_bit_packed_get(row, 0U, 1U), 1U);
    EXPECT_EQ(lh_bit_packed_get(row, 1U, 1U), 0U);
    EXPECT_EQ(lh_bit_packed_get(row, 9U, 1U), 0U);
    EXPECT_EQ(lh_bit_packed_get(row, 10U, 1U), 1U);
    EXPECT_EQ(lh_bit_packed_get(row, 0U, 2U), 2U);
    EXPECT_EQ(lh_bit_packed_get(row, 3U, 2U), 1U);
    EXPECT_EQ(lh_bit_packed_get(row, 0U, 4U), 0xAU);
    EXPECT_EQ(lh_bit_packed_get(row, 3U, 4U), 0xCU);
    EXPECT_EQ(lh_bit_packed_get(row, 1U, 8U), 0x3CU);
}

} // namespace
