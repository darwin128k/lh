#include <gtest/gtest.h>

#include <lh/numeric/parse/bytes.h>
#include <lh/util/addr.h>

TEST(numeric_parse_bytes, unpacks_four_bytes_high_first)
{
    lh_byte_t out[4] = {0, 0, 0, 0};
    lh_numeric_parse_bytes(0x11558880u, out, 4);
    EXPECT_EQ(out[0], 0x11);
    EXPECT_EQ(out[1], 0x55);
    EXPECT_EQ(out[2], 0x88);
    EXPECT_EQ(out[3], 0x80);
}

TEST(numeric_parse_bytes, unpacks_three_bytes_from_low_width)
{
    lh_byte_t out[3] = {0, 0, 0};
    lh_numeric_parse_bytes(0x115588u, out, 3);
    EXPECT_EQ(out[0], 0x11);
    EXPECT_EQ(out[1], 0x55);
    EXPECT_EQ(out[2], 0x88);
}

TEST(numeric_parse_bytes, unpacks_one_byte)
{
    lh_byte_t out = 0;
    lh_numeric_parse_bytes(0xABu, lh_addr_of(out), 1);
    EXPECT_EQ(out, 0xAB);
}
