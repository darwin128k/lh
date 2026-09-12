#include <gtest/gtest.h>

#include <lh/util/bit/endian.h>

namespace
{

TEST(util_bit_endian, pack_be16_writes_high_byte_first)
{
    lh_uchar_t bytes[2];
    lh_bit_pack_be16(0x1234u, bytes);
    EXPECT_EQ(bytes[0], 0x12u);
    EXPECT_EQ(bytes[1], 0x34u);
}

TEST(util_bit_endian, pack_be16_handles_zero_and_max)
{
    lh_uchar_t bytes[2];

    lh_bit_pack_be16(0x0000u, bytes);
    EXPECT_EQ(bytes[0], 0x00u);
    EXPECT_EQ(bytes[1], 0x00u);

    lh_bit_pack_be16(0xFFFFu, bytes);
    EXPECT_EQ(bytes[0], 0xFFu);
    EXPECT_EQ(bytes[1], 0xFFu);
}

TEST(util_bit_endian, unpack_be16_reads_high_byte_first)
{
    const lh_uchar_t bytes[2] = {0x12u, 0x34u};
    EXPECT_EQ(lh_bit_unpack_be16(bytes), 0x1234u);
}

TEST(util_bit_endian, pack_and_unpack_be16_round_trip)
{
    for (lh_u32_t v = 0U; v <= 0xFFFFU; v += 4099U)
    {
        lh_uchar_t bytes[2];
        const lh_u16_t value = static_cast<lh_u16_t>(v);

        lh_bit_pack_be16(value, bytes);
        EXPECT_EQ(lh_bit_unpack_be16(bytes), value);
    }
}

} // namespace
